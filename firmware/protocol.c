#include "protocol.h"
#include "Descriptors.h"
#include "flash.h"
#include <LUFA/Drivers/USB/USB.h>
#include <string.h>
#include <util/delay.h>

/* -----------------------------------------------------------------------
 * CRC-16/CCITT-FALSE  (poly 0x1021, init 0xFFFF, no reflection)
 * ----------------------------------------------------------------------- */
static uint16_t crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t bit = 0; bit < 8; bit++)
            crc = (crc & 0x8000) ? ((crc << 1) ^ 0x1021) : (crc << 1);
    }
    return crc;
}

static uint8_t  cmd_buf[8 + MAX_WORDS_PER_CMD * 2 + 2];
static uint8_t  tx_buf [5 + MAX_WORDS_PER_CMD * 2 + 2];
static uint16_t word_buf[MAX_WORDS_PER_CMD];
static bool     erase_in_progress = false;

/* -----------------------------------------------------------------------
 * Send exactly 'len' bytes from buf over the bulk IN endpoint.
 * Uses the simplest possible LUFA calls.
 * ----------------------------------------------------------------------- */
static bool bulk_send(const uint8_t *buf, uint16_t len)
{
    uint16_t offset = 0;

    while (offset < len) {
        /* Select IN endpoint and wait until it is ready */
        Endpoint_SelectEndpoint(BULK_IN_EPADDR);

        uint16_t t = 0xFFFF;
        while (!Endpoint_IsINReady()) {
            USB_USBTask();
            if (USB_DeviceState != DEVICE_STATE_Configured) return false;
            if (--t == 0) return false;
        }

        /* Fill the endpoint buffer one byte at a time */
        uint16_t space = BULK_EP_SIZE;
        while (space-- && offset < len)
            Endpoint_Write_8(buf[offset++]);

        /* Commit this packet to the host */
        Endpoint_ClearIN();
    }

    /* Send a ZLP if the transfer ended on a full packet boundary */
    if ((len % BULK_EP_SIZE) == 0) {
        Endpoint_SelectEndpoint(BULK_IN_EPADDR);
        uint16_t t = 0xFFFF;
        while (!Endpoint_IsINReady()) {
            USB_USBTask();
            if (--t == 0) return false;
        }
        Endpoint_ClearIN();
    }

    return true;
}

/* -----------------------------------------------------------------------
 * Receive exactly 'len' bytes from the bulk OUT endpoint.
 * ----------------------------------------------------------------------- */
static bool bulk_recv(uint8_t *buf, uint16_t len)
{
    uint16_t got = 0;

    while (got < len) {
        Endpoint_SelectEndpoint(BULK_OUT_EPADDR);

        /* Wait for a packet */
        uint16_t t = 0xFFFF;
        while (!Endpoint_IsOUTReceived()) {
            USB_USBTask();
            if (USB_DeviceState != DEVICE_STATE_Configured) return false;
            if (--t == 0) return false;
        }

        /* Drain bytes */
        while (got < len && Endpoint_BytesInEndpoint() > 0)
            buf[got++] = Endpoint_Read_8();

        /* Acknowledge packet when empty */
        if (Endpoint_BytesInEndpoint() == 0)
            Endpoint_ClearOUT();
    }
    return true;
}

/* -----------------------------------------------------------------------
 * Send a protocol response packet.
 * ----------------------------------------------------------------------- */
static void send_response(uint8_t status,
                           const uint8_t *data,
                           uint16_t data_words)
{
    uint16_t data_bytes = data_words * 2;
    uint16_t pkt_len    = 5 + data_bytes;

    tx_buf[0] = PROTO_MAGIC0;
    tx_buf[1] = PROTO_MAGIC1;
    tx_buf[2] = status;
    tx_buf[3] = (uint8_t)(data_words >> 8);
    tx_buf[4] = (uint8_t)(data_words & 0xFF);

    if (data && data_bytes)
        memcpy(&tx_buf[5], data, data_bytes);

    uint16_t chk        = crc16(tx_buf, pkt_len);
    tx_buf[pkt_len]     = (uint8_t)(chk >> 8);
    tx_buf[pkt_len + 1] = (uint8_t)(chk & 0xFF);

    bulk_send(tx_buf, pkt_len + 2);
}

/* -----------------------------------------------------------------------
 * Main protocol task
 * ----------------------------------------------------------------------- */
void protocol_task(void)
{
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    if (erase_in_progress && flash_is_ready())
        erase_in_progress = false;

    /* Non-blocking check for incoming command */
    Endpoint_SelectEndpoint(BULK_OUT_EPADDR);
    if (!Endpoint_IsOUTReceived())
        return;

    /* Read header */
    if (!bulk_recv(cmd_buf, 8))
        return;

    if (cmd_buf[0] != PROTO_MAGIC0 || cmd_buf[1] != PROTO_MAGIC1)
        return;

    uint8_t  cmd  = cmd_buf[2];
    uint32_t addr = ((uint32_t)cmd_buf[3] << 16) |
                    ((uint32_t)cmd_buf[4] <<  8) |
                     (uint32_t)cmd_buf[5];
    uint16_t len  = ((uint16_t)cmd_buf[6] << 8) | cmd_buf[7];

    if (len > MAX_WORDS_PER_CMD) {
        send_response(STATUS_ERROR, NULL, 0);
        return;
    }

    /* Read payload for PROGRAM */
    uint16_t payload_bytes = (cmd == CMD_PROGRAM) ? (len * 2) : 0;
    if (payload_bytes) {
        if (!bulk_recv((uint8_t *)word_buf, payload_bytes)) {
            send_response(STATUS_ERROR, NULL, 0);
            return;
        }
        /* Copy payload into cmd_buf after the header for CRC */
        memcpy(cmd_buf + 8, word_buf, payload_bytes);
    }

    /* Read CRC */
    uint8_t crc_rx[2];
    if (!bulk_recv(crc_rx, 2)) {
        send_response(STATUS_ERROR, NULL, 0);
        return;
    }

    uint16_t crc_calc = crc16(cmd_buf, 8 + payload_bytes);
    uint16_t crc_got  = ((uint16_t)crc_rx[0] << 8) | crc_rx[1];

    if (crc_calc != crc_got) {
        send_response(STATUS_CRC_ERROR, NULL, 0);
        return;
    }

    /* Dispatch commands */
    switch (cmd) {

        case CMD_IDENTIFY: {
            uint32_t id = flash_read_id();
            word_buf[0] = (uint16_t)(id >> 16);
            word_buf[1] = (uint16_t)(id & 0xFFFF);
            send_response(STATUS_OK, (uint8_t *)word_buf, 2);
            break;
        }

        case CMD_READ: {
            if (addr + len > FLASH_SIZE_WORDS) {
                send_response(STATUS_ERROR, NULL, 0);
                break;
            }
            /* Hardware reset — unconditionally returns flash to read mode */
            flash_hw_reset();
            flash_read_buffer(addr, word_buf, len);
            send_response(STATUS_OK, (uint8_t *)word_buf, len);
            break;
        }

        case CMD_CHIP_ERASE: {
            if (erase_in_progress) { send_response(STATUS_BUSY,  NULL, 0); break; }
            oe_buf_isolate();
            flash_chip_erase_start();
            erase_in_progress = true;
            send_response(STATUS_BUSY, NULL, 0);
            break;
        }

        case CMD_SECTOR_ERASE: {
            if (erase_in_progress) { send_response(STATUS_BUSY,  NULL, 0); break; }
            oe_buf_isolate();
            flash_sector_erase_start(addr);
            erase_in_progress = true;
            send_response(STATUS_BUSY, NULL, 0);
            break;
        }

        case CMD_PROGRAM: {
            if (erase_in_progress) { send_response(STATUS_BUSY,  NULL, 0); break; }
            if (addr + len > FLASH_SIZE_WORDS) { send_response(STATUS_ERROR, NULL, 0); break; }
            oe_buf_isolate();
            uint8_t r = flash_program_buffer(addr, word_buf, len);
            flash_reset();
            oe_buf_release();
            send_response((r == FLASH_OK) ? STATUS_OK : STATUS_TIMEOUT, NULL, 0);
            break;
        }

        case CMD_GET_STATUS: {
            if (erase_in_progress) {
                if (flash_is_ready()) {
                    erase_in_progress = false;
                    flash_reset();
                    oe_buf_release();   /* erase done — return /OE to Gary */
                    send_response(STATUS_OK, NULL, 0);
                } else {
                    send_response(STATUS_BUSY, NULL, 0);
                }
            } else {
                send_response(STATUS_OK, NULL, 0);
            }
            break;
        }

        case CMD_FLASH_RESET: {
            erase_in_progress = false;
            flash_hw_reset();
            flash_read_buffer(0, word_buf, 2);
            send_response(STATUS_OK, (uint8_t *)word_buf, 2);
            break;
        }

        case CMD_AVR_RESET: {
            /* Trigger hardware reset via PE5 → /RESET.
             * PE5 is connected to the AVR /RESET pin on the PCB.
             * The HWB button must be held by the user before this is called
             * so that the bootloader enters DFU mode on the rising edge of reset.
             * Sequence:
             *   1. User holds HWB button
             *   2. Host sends CMD_AVR_RESET
             *   3. Firmware ACKs, releases bus, pulls PE5 low
             *   4. AVR resets with HWB held → DFU entry
             *   5. User releases HWB after DFU enumerates on host
             */
            send_response(STATUS_OK, NULL, 0);
            flash_bus_release();
            oe_buf_release();
            _delay_ms(50);   /* give USB time to flush ACK */
            /* Drive PE5 low to assert RESET */
            DDRE  |=  (1 << PE5);
            PORTE &= ~(1 << PE5);
            _delay_ms(10);   /* hold reset low long enough */
            /* PE5 released — AVR will have reset by now */
            PORTE |=  (1 << PE5);
            DDRE  &= ~(1 << PE5);
            break;
        }

        default:
            send_response(STATUS_ERROR, NULL, 0);
            break;
    }

    /* Release bus back to high-Z after every command so Gary
     * can drive the flash bus when the Amiga is running. */
    flash_bus_release();
}
