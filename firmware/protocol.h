#pragma once

#include <stdint.h>
#include <stdbool.h>

/* -----------------------------------------------------------------------
 * Meggy binary protocol over USB bulk endpoints
 *
 * COMMAND  (host → device, OUT endpoint):
 *   [0x4D][0x47][CMD][A2][A1][A0][L1][L0][DATA (L*2 bytes)][CRC_H][CRC_L]
 *
 * RESPONSE (device → host, IN endpoint):
 *   [0x4D][0x47][STATUS][L1][L0][DATA (L*2 bytes)][CRC_H][CRC_L]
 *
 * CRC16-CCITT (poly 0x1021, init 0xFFFF) over all bytes except the CRC.
 * ----------------------------------------------------------------------- */

#define PROTO_MAGIC0    0x4D
#define PROTO_MAGIC1    0x47

#define CMD_IDENTIFY        0x01
#define CMD_READ            0x02
#define CMD_CHIP_ERASE      0x03
#define CMD_SECTOR_ERASE    0x04
#define CMD_PROGRAM         0x05
#define CMD_GET_STATUS      0x06
#define CMD_FLASH_RESET     0x07
#define CMD_AVR_RESET       0x08  /* watchdog reset of AVR */

#define STATUS_OK           0x00
#define STATUS_ERROR        0x01
#define STATUS_BUSY         0x02
#define STATUS_CRC_ERROR    0x03
#define STATUS_TIMEOUT      0x04

/* Max words per READ or PROGRAM — must match pyusb tool */
#define MAX_WORDS_PER_CMD   256

void protocol_task(void);
