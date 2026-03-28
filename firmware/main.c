#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include <LUFA/Drivers/USB/USB.h>

#include "Descriptors.h"
#include "flash.h"
#include "protocol.h"

int main(void)
{
    /* Disable watchdog left active by the DFU bootloader */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Full speed 16 MHz */
    clock_prescale_set(clock_div_1);

    /* Initialise flash I/O pins */
    flash_init();

    /* Start USB stack */
    USB_Init();

    /* Enable global interrupts */
    sei();

    for (;;) {
        protocol_task();
        USB_USBTask();
    }
}

void EVENT_USB_Device_Connect(void)    {}
void EVENT_USB_Device_Disconnect(void) {}

void EVENT_USB_Device_ConfigurationChanged(void)
{
    /* Configure the two bulk endpoints */
    Endpoint_ConfigureEndpoint(BULK_IN_EPADDR,  EP_TYPE_BULK,
                               BULK_EP_SIZE, 1);
    Endpoint_ConfigureEndpoint(BULK_OUT_EPADDR, EP_TYPE_BULK,
                               BULK_EP_SIZE, 1);
}

void EVENT_USB_Device_ControlRequest(void) {}
