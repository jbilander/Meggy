#pragma once

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

/* -----------------------------------------------------------------------
 * Pin mapping (from schematic):
 *
 *   PC0-PC7  (pins 35-42) = D0-D7
 *   PB0-PB7  (pins 10-17) = D8-D15
 *   PA0-PA7  (pins 51-44) = A0-A7
 *   PD0-PD7  (pins 25-32) = A8-A15
 *   PF5      (pin  56)    = A16
 *   PF6      (pin  55)    = A17
 *   PF7      (pin  54)    = A18
 *   PE0      (pin  33)    = /WE   (10k pull-up)
 *   PE1      (pin  34)    = /RST  (10k pull-up)
 *   PF3      (pin  58)    = /OE   (10k pull-up)
 *   PF4      (pin  57)    = RY/BY (10k pull-up, input)
 *   /CE  on flash = tied to GND (always asserted)
 *   /BYTE on flash = tied to VCC (word mode, 16-bit)
 *
 * NOTE: PF4-PF7 are shared with the AVR JTAG interface.  JTAG is
 * disabled at runtime by flash_init() using an inline-assembly
 * double-write to the JTD bit — the only safe method when the
 * AT90USB1286 is soldered and accessible only via USB DFU.
 * No external programmer or fuse change is required.
 *
 * Address space: 2MB = 0x200000 bytes = 0x100000 16-bit words
 * AMD/Fujitsu command set (standard for M29F160FT)
 * ----------------------------------------------------------------------- */

/* Result codes */
#define FLASH_OK          0x00
#define FLASH_ERROR       0x01
#define FLASH_TIMEOUT     0x02

/* Flash geometry */
#define FLASH_SIZE_WORDS  0x100000UL   /* 1M 16-bit words = 2MB */
#define FLASH_SIZE_BYTES  0x200000UL   /* 2 MB                  */

/* Expected IDs for M29F160FT55N3E2 (ST/Micron, top-boot) */
#define FLASH_MFR_ST      0x0020
#define FLASH_DEV_160FT   0x22D2

void     flash_init(void);
void     flash_hw_reset(void);   /* hardware reset via /RST pin */
void     flash_bus_release(void); /* release bus to high-Z for Amiga */
void     flash_reset(void);
uint32_t flash_read_id(void);           /* returns (MFR<<16)|DEV */
bool     flash_is_ready(void);          /* polls RY/BY pin       */

uint8_t  flash_read_buffer(uint32_t word_addr,
                            uint16_t *buf,
                            uint16_t  count);

/* Non-blocking erase start — call flash_is_ready() to poll completion */
void     flash_chip_erase_start(void);
void     flash_sector_erase_start(uint32_t word_addr);

/* Blocking word-program loop for up to 256 words (~2.5ms at 10µs/word) */
uint8_t  flash_program_buffer(uint32_t       word_addr,
                               const uint16_t *buf,
                               uint16_t        count);
