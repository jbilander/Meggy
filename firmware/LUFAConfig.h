#pragma once

#if (ARCH == ARCH_AVR8)
    #include <avr/power.h>
#endif

/* Lock device state into GPIOR0 register for faster access */
#define DEVICE_STATE_AS_GPIOR         0

/* Fixed endpoint sizes — AT90USB1286 control EP is 8 bytes */
#define FIXED_CONTROL_ENDPOINT_SIZE   8
#define FIXED_NUM_CONFIGURATIONS      1

/* Store descriptors in flash (PROGMEM) */
#define USE_FLASH_DESCRIPTORS

/* Full-speed device, enable internal regulator, auto-configure PLL */
#define USE_STATIC_OPTIONS  (USB_DEVICE_OPT_FULLSPEED | \
                             USB_OPT_REG_ENABLED      | \
                             USB_OPT_AUTO_PLL)

/* Automatic SOF (Start Of Frame) handling */
#define USB_DEVICE_AUTOMATIC_SOF      1
