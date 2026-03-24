/*
 * Meggy — LED blink test firmware
 * ================================
 * Purpose: Verify DFU programming works before loading the real firmware.
 * No LUFA, no flash driver — just the bare AVR.
 *
 * Hardware:
 *   Connect an LED + 330 ohm resistor between PE4 (pin 18) and GND.
 *   PE4 is the OC3B pin, free/NC on Meggy Rev A.
 *
 * Expected behaviour:
 *   LED blinks at ~1 Hz (500 ms on, 500 ms off).
 *   If the LED blinks, DFU programming is confirmed working.
 *
 * Build:
 *   avr-gcc -mmcu=at90usb1286 -DF_CPU=16000000UL -Os \
 *           -o blink.elf blink.c
 *   avr-objcopy -O ihex blink.elf blink.hex
 *
 * Program:
 *   (Hold HWB, tap RESET, release HWB — device enters DFU mode)
 *   dfu-programmer at90usb1286 erase
 *   dfu-programmer at90usb1286 flash blink.hex
 *   dfu-programmer at90usb1286 launch
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <util/delay.h>

int main(void)
{
    /* Disable watchdog left armed by DFU bootloader */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Full speed — 16 MHz */
    clock_prescale_set(clock_div_1);

    /* PE4 as output, start low */
    DDRA  |=  (1 << PA1);
    PORTA &= ~(1 << PA1);

    for (;;) {
        PORTA |=  (1 << PA1);   /* LED on  */
        _delay_ms(500);
        PORTA &= ~(1 << PA1);   /* LED off */
        _delay_ms(500);
    }
}
