# Meggy
A 1MB Kickstart ROM for the A500+, using a 16 Mbit M29F160 parallel NOR flash, USB programmable from your PC

***

<a href="images/Meggy_revA_pic1.png">
<img src="images/Meggy_revA_pic1.png" width="500" height="286">
</a>
<br />
<a href="images/Meggy_revA_pic2.png">
<img src="images/Meggy_revA_pic2.png" width="500" height="266">
</a>
<br />


***

# Bill of Materials — Meggy Rev B

| Ref | Qty | Value | Description | Package | Notes | Mouser |
|-----|-----|-------|-------------|---------|-------|--------|
| U1 | 2 | — | Pin strip 1×21, 2.54mm pitch, 0.4mm square pins | — | <span title="Cut from 1×50 1.27mm pitch strip, remove every other pin to give 2.54mm pitch. Fits Amiga dual-wipe socket better than standard 0.47mm turned pins.">Pin strip info...</span> | [AliExpress example](https://www.aliexpress.com/item/4000979931385.html) |
| U2 | 1 | AT90USB1286-MU | AVR USB microcontroller | QFN-64 | | [556-AT90USB1286-MU](https://www.mouser.com/ProductDetail/556-AT90USB1286-MU) |
| U3 | 1 | M29F160FT55N3E2 | 2MB NOR Flash (top-boot) | TSOP-48 | | [913-M29F160FT55N3E2](https://www.mouser.com/ProductDetail/913-M29F160FT55N3E2) |
| U4 | 1 | 74AHCT1G126GW | Single tristate buffer | SC-70-5 / SOT-353 | <span title="Isolates Gary's /OE signal from the flash chip during USB programming. 10k pull-up on EN keeps buffer enabled by default even with no firmware loaded.">OE isolation...</span> | [771-AHCT1G126GW125](https://www.mouser.com/ProductDetail/771-AHCT1G126GW125) |
| D1, D2 | 2 | CDBU0520 | Schottky diode 0.5A 20V | SOD-523F / 0603 | <span title="Diode-OR between VBUS (USB 5V) and VCC (Amiga 5V). Prevents backfeed between the two supplies. Higher voltage wins.">Power OR...</span> | [750-CDBU0520](https://www.mouser.com/ProductDetail/750-CDBU0520) |
| X1 | 1 | 16 MHz | Crystal oscillator SG5032CCN 16.000000M-HJGA3 | 5×3.2mm SMD | | [732-5032CC16.0HJGA3](https://www.mouser.com/ProductDetail/732-5032CC16.0HJGA3) |
| SW1 | 1 | — | Sunrom Tactile Switch SMD | 3×4×2mm | <span title="Hold SW1 (HWB) then power cycle to enter DFU bootloader mode for firmware programming.">HWB / DFU entry...</span> | [AliExpress example](https://www.aliexpress.com/item/32855171871.html) |
| J1 | 1 | 10118194-0001LF | Micro USB-B connector, Amphenol FCI, 5-pin | SMD | | [649-10118194-0001LF](https://www.mouser.com/ProductDetail/649-10118194-0001LF) |
| J2 | 1 | MSK12D19 | SMD slide switch, 3-pin, 2.5mm pitch | SMD | <span title="Flash A19 select — open = slot 1 (pull-up), shorted to GND = slot 0. Pins bent to right angle for flush mounting. Alternatively a 3-pin 2.54mm right-angle pin header with external toggle switch.">Flash A19 slot select...</span> | [AliExpress example](https://www.aliexpress.com/item/1005006482584650.html) |
| RN1, RN2 | 2 | 10k | Resistor network 4×10kΩ isolated | 0603×4 (1206) | CAY16-103J4LF | [652-CAY16-103J4LF](https://www.mouser.com/ProductDetail/652-CAY16-103J4LF) |
| R1, R2 | 2 | 22Ω | CR0603-FX-22R0ELF — USB D+/D− series resistors | 0603 | | [652-CR0603FX-22R0ELF](https://www.mouser.com/ProductDetail/652-CR0603FX-22R0ELF) |
| R3 | 1 | 22Ω | CR0603-FX-22R0ELF — Clock series termination | 0603 | | [652-CR0603FX-22R0ELF](https://www.mouser.com/ProductDetail/652-CR0603FX-22R0ELF) |
| R4 | 1 | 100Ω | CR0603-FX-1000ELF — VBUS detection | 0603 | <span title="Series resistor between VBUS_5V and AVR VBUS pin 8 for USB bus voltage detection.">VBUS detect...</span> | [652-CR0603FX-1000ELF](https://www.mouser.com/ProductDetail/652-CR0603FX-1000ELF) |
| R5 | 1 | 10kΩ | CR0603-FX-1002ELF — OE_BUF_EN pull-up | 0603 | <span title="Pull-up to VCC on PE4 (OE_BUF_EN). Keeps the 74AHCT1G126GW buffer enabled by default so Gary controls /OE even with no firmware loaded.">OE_BUF_EN pull-up...</span> | [652-CR0603FX-1002ELF](https://www.mouser.com/ProductDetail/652-CR0603FX-1002ELF) |
| C1 | 1 | 1µF | CL10B105KA8NNND | 0603 | <span title="UCAP filter capacitor on AVR pin 7. Required for the AT90USB1286 internal USB voltage regulator.">UCAP filter...</span> | [187-CL10B105KA8NNND](https://www.mouser.com/ProductDetail/187-CL10B105KA8NNND) |
| C2, C3, C4, C5 | 4 | 100nF | CL10B104KA8NNNC — Bypass decoupling | 0603 | <span title="Bypass decoupling capacitors. Place as close as possible to the VCC pins of U2, U3 and U4.">Bypass decoupling...</span> | [187-CL10B104KA8NNNC](https://www.mouser.com/ProductDetail/187-CL10B104KA8NNNC) |

## Key ICs

**AT90USB1286-MU** — Atmel/Microchip AVR, 128KB flash, native full-speed USB, QFN-64. Runs the Meggy firmware and provides USB bulk transfer for flash programming. DFU bootloader in hardware boot section (no external programmer needed after initial setup).

**M29F160FT55N3E2** — Micron/ST 16Mbit (2MB) NOR flash, top-boot, 55ns, 5V, x16. Stores two 1MB Kickstart image slots selectable via the A19 switch. [Datasheet](https://www.mouser.com/datasheet/3/893/1/M29FxxFT_FB.pdf)

**74AHCT1G126GW** — Single buffer with tristate output (active-high enable). Isolates Gary's /OE signal from the flash chip during USB programming operations. 10k pull-up on EN ensures the buffer is enabled (Amiga in control) by default even with no firmware loaded.

**CDBU0520** — Comchip 0.5A 20V Schottky diode, SOD-523F (0603 footprint). Two used for power OR between VBUS (USB) and VCC (Amiga). Prevents backfeed between the two supplies.

## Power

The board can be powered from either:
- **VBUS** (USB 5V) — when connected to a PC for programming
- **VCC** (Amiga 5V) — when installed in the Amiga

D1 and D2 form a diode-OR, allowing both supplies to coexist safely. The higher voltage wins. Schottky forward drop is approximately 150–200mV at typical operating currents.

## Image Slot Selection (J2)

J2 controls flash A19 (pin 9), selecting between the two 1MB image slots:

| J2 | Flash A19 | Slot |
|----|-----------|------|
| Open | High (10k pull-up) | Slot 1 |
| Shorted to GND | Low | Slot 0 |

## Programming

No external programmer is required. The AT90USB1286 ships with a USB DFU bootloader in the hardware boot section which cannot be erased. To enter DFU mode:
1. Hold **SW1** (HWB)
2. Power cycle the board (unplug and replug USB, or cycle Amiga power)
3. Release **SW1**

Use `flash.bat` (Windows) with avrdude and the flip1 programmer protocol, or dfu-programmer on Linux/macOS.
