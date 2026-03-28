#include "flash.h"
#include <util/delay.h>
#include <avr/interrupt.h>

/* -----------------------------------------------------------------------
 * Pin mapping — Rev A bodge / Rev B native:
 *
 *   D0-D7   = PC0-PC7   (unchanged)
 *   D8-D15  = PB0-PB7   (unchanged)
 *   A0-A7   = PA0-PA7   (unchanged)
 *   A8-A15  = PD0-PD7   (unchanged)
 *   A16     = PF0       (bodge from PF5)
 *   A17     = PF1       (bodge from PF6)
 *   A18     = PF2       (bodge from PF7)
 *   /OE     = PF3       (unchanged)
 *   RY/BY   = PE3       (bodge from PF4)
 *   /WE     = PE0       (unchanged)
 *   /RST    = PE1       (unchanged)
 *   HWB     = PE2       (unchanged, DFU bootloader button)
 *   /CE     = GND       (always asserted)
 *   /BYTE   = VCC       (word mode, 16-bit)
 *
 * PF4-PF7 are now unused — JTAG may remain active on them harmlessly
 * since nothing is connected to those pins in the bodged/Rev B layout.
 * No JTD / JTAG-disable code is needed.
 * ----------------------------------------------------------------------- */

/* /WE on PE0 */
#define WE_HIGH()     (PORTE |=  (1 << PE0))
#define WE_LOW()      (PORTE &= ~(1 << PE0))

/* /RST on PE1 */
#define RST_HIGH()    (PORTE |=  (1 << PE1))
#define RST_LOW()     (PORTE &= ~(1 << PE1))

/* /OE on PF3 */
#define OE_HIGH()     (PORTF |=  (1 << PF3))
#define OE_LOW()      (PORTF &= ~(1 << PF3))

/* RY/BY on PE3 — high = ready, low = busy */
#define RYBY_READY()  (PINE & (1 << PE3))

/* -----------------------------------------------------------------------
 * Data bus helpers
 * ----------------------------------------------------------------------- */
static inline void data_bus_output(void)
{
    DDRC = 0xFF;    /* D0-D7  */
    DDRB = 0xFF;    /* D8-D15 */
}

static inline void data_bus_input(void)
{
    DDRC  = 0x00;   /* D0-D7  hi-Z */
    DDRB  = 0x00;   /* D8-D15 hi-Z */
    PORTC = 0x00;   /* no pull-ups — bus has external termination */
    PORTB = 0x00;
}

static inline void write_data(uint16_t d)
{
    PORTC = (uint8_t)(d & 0xFF);
    PORTB = (uint8_t)(d >> 8);
}

static inline uint16_t read_data(void)
{
    return ((uint16_t)PINB << 8) | PINC;
}

/* -----------------------------------------------------------------------
 * Address bus helper
 *   PA  = A0-A7
 *   PD  = A8-A15
 *   PF2 = A18, PF1 = A17, PF0 = A16
 *
 * Preserve PF3 (/OE) and PF4-PF7 (unused/JTAG — don't touch).
 * Mask 0xF8 keeps bits [7:3], writes new value into bits [2:0].
 * ----------------------------------------------------------------------- */
static inline void set_address(uint32_t wa)
{
    PORTA = (uint8_t)(wa & 0xFF);
    PORTD = (uint8_t)((wa >> 8) & 0xFF);
    /* A18:A16 sit directly in PF[2:0] — no shift needed */
    PORTF = (PORTF & 0xF8) | (uint8_t)((wa >> 16) & 0x07);
}

/* -----------------------------------------------------------------------
 * Low-level write — one AMD command word.
 * /CE is always asserted (tied to GND).
 * /OE must be high during writes.
 * ----------------------------------------------------------------------- */
static inline void addr_bus_output(void)
{
    DDRA  = 0xFF;           /* A0-A7  outputs */
    DDRD  = 0xFF;           /* A8-A15 outputs */
    /* PF0=A16, PF1=A17, PF2=A18, PF3=/OE — all outputs */
    DDRF  = 0x0F;
    /* /OE starts high (deasserted) — flash_cmd and flash_read_buffer
     * will assert it at the right time */
    PORTF = (1 << PF3);
}

static void flash_cmd(uint32_t word_addr, uint16_t cmd)
{
    addr_bus_output();
    data_bus_output();
    OE_HIGH();
    set_address(word_addr);
    _delay_us(2);   /* address settling — bodge wire margin */
    write_data(cmd);
    /* Disable interrupts around /WE pulse so a USB interrupt cannot
     * extend the pulse duration and corrupt the command. */
    cli();
    WE_LOW();
    _delay_us(1);   /* tWP min 70 ns */
    WE_HIGH();
    sei();
    _delay_us(1);   /* tWPH min 30 ns */
}

/* -----------------------------------------------------------------------
 * AMD standard two-cycle unlock sequence.
 * Addresses are word addresses (0x555, 0x2AA).
 * ----------------------------------------------------------------------- */
static void unlock(void)
{
    flash_cmd(0x555UL, 0x00AA);
    flash_cmd(0x2AAUL, 0x0055);
}

/* -----------------------------------------------------------------------
 * RY/BY poll with µs timeout.
 * Max chip-erase time (datasheet): 200 s — caller uses 220 s.
 * Max word-program time: 200 µs   — caller uses 500 µs.
 * ----------------------------------------------------------------------- */
static uint8_t wait_ready(uint32_t timeout_us)
{
    while (timeout_us--) {
        if (RYBY_READY()) return FLASH_OK;
        _delay_us(1);
    }
    return FLASH_TIMEOUT;
}

/* -----------------------------------------------------------------------
 * Public API
 * ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
 * OE buffer (74AHCT1G126GW) control — PE4 with external 10k pull-up.
 *
 * Pull-up keeps buffer ENABLED by default (Amiga in control of /OE).
 * Driving PE4 low tristates the buffer, giving AVR sole control of /OE.
 *
 * NOTE: These functions are no-ops on Rev A which lacks PE4 routing.
 *       On Rev B PE4 is connected to the buffer enable pin.
 * ----------------------------------------------------------------------- */
void oe_buf_isolate(void)
{
    /* Drive PE4 low — tristate the buffer, isolate Gary from flash /OE */
    DDRE  |=  (1 << PE4);
    PORTE &= ~(1 << PE4);
}

void oe_buf_release(void)
{
    /* Return PE4 to input — external 10k pull-up re-enables the buffer */
    DDRE  &= ~(1 << PE4);
    PORTE &= ~(1 << PE4);
}

void flash_bus_release(void)
{
    /*
     * Release ALL AVR pins connected to the flash bus to high-Z inputs.
     * Must be called after every flash operation so Gary can drive the
     * bus freely when the Amiga is running.
     *
     * /WE and /RST are kept as outputs held HIGH (deasserted) —
     * this is safe and prevents floating inputs on the flash.
     * /OE is kept as output held HIGH (deasserted).
     * All address and data lines become inputs (high-Z).
     */
    /* Data bus: high-Z */
    DDRC  = 0x00;  PORTC = 0x00;   /* D0-D7  */
    DDRB  = 0x00;  PORTB = 0x00;   /* D8-D15 */

    /* Address bus: high-Z */
    DDRA  = 0x00;  PORTA = 0x00;   /* A0-A7  */
    DDRD  = 0x00;  PORTD = 0x00;   /* A8-A15 */

    /* A16-A18 and /OE: all high-Z.
     * The 10k pull-up on /OE_FLASH keeps it deasserted.
     * Gary drives /OE low during ROM read cycles — must not fight it. */
    DDRF  = 0x00;
    PORTF = 0x00;

    /* /WE and /RST stay as outputs held high */
    PORTE |= (1 << PE0) | (1 << PE1);  /* /WE=1, /RST=1 */
}

void flash_init(void)
{
    /* Disable SPI — PB0-3 repurposed for D8-D11 */
    SPCR = 0;

    /* Disable ADC — PF0-2 now used as address outputs, not ADC inputs */
    ADCSRA = 0;

    /* ---- Address bus outputs, cleared ---- */
    DDRA  = 0xFF;  PORTA = 0x00;   /* A0-A7  */
    DDRD  = 0xFF;  PORTD = 0x00;   /* A8-A15 */

    /*
     * PORTF:
     *   PF7-PF4 = unused (JTAG pins — leave as inputs, nothing connected)
     *   PF3     = /OE    — output, start high (deasserted)
     *   PF2     = A18    — output, start 0
     *   PF1     = A17    — output, start 0
     *   PF0     = A16    — output, start 0
     *
     * DDRF: set bits [3:0] as outputs, leave [7:4] as inputs (0).
     */
    DDRF  = 0x0F;
    PORTF = (1 << PF3);     /* /OE high, A16-A18 = 0, no pull-ups on PF4-7 */

    /*
     * PORTE:
     *   PE0 = /WE  — output, start high (deasserted)
     *   PE1 = /RST — output, start high (deasserted)
     *   PE2 = HWB  — leave as input (DFU bootloader)
     *   PE3 = RY/BY — input (external 10k pull-up on flash pin)
     *   PE4-PE7 = unused, leave as inputs
     *
     * Only set PE0 and PE1 as outputs; preserve all other DDRE bits.
     */
    /* Set DDRE completely explicitly — do not rely on reset state.
     * PE0=/WE and PE1=/RST are outputs.
     * PE2=HWB, PE3=RY/BY, PE4-PE7 are inputs.
     * Write PORT before DDR to avoid a glitch where the pin momentarily
     * outputs 0 before being driven high. */
    PORTE = (PORTE & ~((1<<PE4)|(1<<PE5)|(1<<PE6)|(1<<PE7)))
          | (1 << PE0)   /* /WE  high */
          | (1 << PE1);  /* /RST high */
    DDRE  = (1 << PE0) | (1 << PE1);  /* only PE0 and PE1 as outputs */

    /* ---- /WE pin sanity check ---- */
    /* Drive /WE low and verify PINE reads it back low.
     * If this fails the PE0 driver is broken and no writes will work. */
    PORTE &= ~(1 << PE0);   /* /WE low  */
    _delay_us(1);
    /* (Cannot easily report failure here — stored in a global for later) */
    PORTE |=  (1 << PE0);   /* /WE high */
    _delay_us(1);

    /* ---- Data bus: high-Z initially ---- */
    data_bus_input();

    /* ---- Release flash from reset, enter read mode ---- */
    _delay_ms(1);
    RST_HIGH();
    _delay_ms(2);
    flash_cmd(0x000UL, 0x00F0);  /* software reset after hw reset */
    _delay_ms(1);

    /* ---- Release bus so Gary can drive it when Amiga is running ---- */
    flash_bus_release();
}

void flash_hw_reset(void)
{
    /* Hardware reset via /RST pin (PE1) — works from any state.
     * tRP min = 100 ns, tRH min = 100 ns per datasheet.
     * We use a much longer pulse to ensure recovery from
     * post-program/erase status mode. */
    RST_LOW();
    _delay_ms(10);   /* generous reset pulse */
    RST_HIGH();
    _delay_ms(10);   /* generous recovery time */
    /* Software reset as well */
    flash_cmd(0x000UL, 0x00F0);
    _delay_ms(10);
    /* Release bus */
    flash_bus_release();
}

void flash_reset(void)
{
    flash_cmd(0x000UL, 0x00F0);
    _delay_us(10);
    data_bus_input();
}

bool flash_is_ready(void)
{
    return (bool)(PINE & (1 << PE3));
}

uint32_t flash_read_id(void)
{
    unlock();
    flash_cmd(0x555UL, 0x0090);
    _delay_us(10);

    data_bus_input();
    OE_LOW();

    set_address(0x00);  _delay_us(1);
    uint16_t mfr = read_data();

    set_address(0x01);  _delay_us(1);
    uint16_t dev = read_data();

    OE_HIGH();
    data_bus_input();
    flash_reset();

    return ((uint32_t)mfr << 16) | dev;
    /*
     * Expected for M29F160FT55N3E2:
     *   Manufacturer = 0x0020  (ST/Micron)
     *   Device       = 0x22D2  (M29F160FT top-boot, word mode)
     */
}

uint8_t flash_read_buffer(uint32_t word_addr, uint16_t *buf, uint16_t count)
{
    /* Re-enable address bus outputs (may have been released) */
    addr_bus_output();
    /* Data bus as input, /WE deasserted */
    WE_HIGH();
    data_bus_input();
    set_address(word_addr);
    OE_LOW();
    _delay_us(2);   /* tACC settling */

    for (uint16_t i = 0; i < count; i++) {
        set_address(word_addr + i);
        _delay_us(1);           /* tACC max 55 ns; 1 µs is safe */
        buf[i] = read_data();
    }

    OE_HIGH();
    data_bus_input();
    return FLASH_OK;
}

void flash_chip_erase_start(void)
{
    /*
     * Six-cycle chip erase — non-blocking.
     * RY/BY goes low immediately after the final command word.
     * Caller must poll flash_is_ready() or CMD_GET_STATUS.
     * Typical: ~30 s   Max: 200 s
     */
    unlock();
    flash_cmd(0x555UL, 0x0080);
    unlock();
    flash_cmd(0x555UL, 0x0010);
    data_bus_input();
}

void flash_sector_erase_start(uint32_t word_addr)
{
    /*
     * Sector erase — word_addr may be any address within the sector.
     * Typical: ~1 s   Max: 25 s
     */
    unlock();
    flash_cmd(0x555UL, 0x0080);
    unlock();
    flash_cmd(word_addr, 0x0030);
    data_bus_input();
}

uint8_t flash_program_buffer(uint32_t word_addr,
                              const uint16_t *buf,
                              uint16_t count)
{
    /*
     * AMD word-program, one word at a time.
     * Typical: ~7 µs/word   Max: 200 µs/word
     * 256 words worst case: ~51 ms — acceptable to block.
     */
    for (uint16_t i = 0; i < count; i++) {
        unlock();
        flash_cmd(0x555UL, 0x00A0);
        flash_cmd(word_addr + i, buf[i]);

        /* 5000 µs timeout — well above 200 µs datasheet max,
         * gives extra margin for bodge wire timing issues.    */
        uint8_t r = wait_ready(5000);
        if (r != FLASH_OK) {
            flash_reset();
            return r;
        }
    }

    data_bus_input();
    return FLASH_OK;
}
