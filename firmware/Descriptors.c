#include "Descriptors.h"

/*
 * DTYPE_CSInterface is not needed for a plain bulk device.
 * Kept as a safety define in case any included header references it.
 */
#ifndef DTYPE_CSInterface
#define DTYPE_CSInterface  0x24
#endif

/* -----------------------------------------------------------------------
 * Device descriptor
 * VID 0x03EB = Atmel/Microchip, PID 0x2044 = LUFA generic bulk demo
 * Class 0xFF = vendor-specific — WinUSB works cleanly with this.
 * ----------------------------------------------------------------------- */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t),
                                .Type = DTYPE_Device},
    .USBSpecification       = VERSION_BCD(1,1,0),
    .Class                  = USB_CSCP_VendorSpecificClass,
    .SubClass               = USB_CSCP_NoDeviceSubclass,
    .Protocol               = USB_CSCP_NoDeviceProtocol,
    .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,
    .VendorID               = 0x03EB,
    .ProductID              = 0x2044,
    .ReleaseNumber          = VERSION_BCD(0,1,0),
    .ManufacturerStrIndex   = STRING_ID_Manufacturer,
    .ProductStrIndex        = STRING_ID_Product,
    .SerialNumStrIndex      = NO_DESCRIPTOR,
    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS,
};

/* -----------------------------------------------------------------------
 * Configuration descriptor — one interface, two bulk endpoints
 * ----------------------------------------------------------------------- */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
    .Config = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t),
                                    .Type = DTYPE_Configuration},
        .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
        .TotalInterfaces        = 1,
        .ConfigurationNumber    = 1,
        .ConfigurationStrIndex  = NO_DESCRIPTOR,
        .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED |
                                    USB_CONFIG_ATTR_SELFPOWERED),
        .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100),
    },

    .DataInterface = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t),
                                    .Type = DTYPE_Interface},
        .InterfaceNumber        = INTERFACE_ID_Data,
        .AlternateSetting       = 0,
        .TotalEndpoints         = 2,
        .Class                  = USB_CSCP_VendorSpecificClass,
        .SubClass               = 0x00,
        .Protocol               = 0x00,
        .InterfaceStrIndex      = NO_DESCRIPTOR,
    },

    /* IN endpoint: device → host (responses) */
    .DataInEndpoint = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                                    .Type = DTYPE_Endpoint},
        .EndpointAddress        = BULK_IN_EPADDR,
        .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC |
                                    ENDPOINT_USAGE_DATA),
        .EndpointSize           = BULK_EP_SIZE,
        .PollingIntervalMS      = 0x05,
    },

    /* OUT endpoint: host → device (commands) */
    .DataOutEndpoint = {
        .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                                    .Type = DTYPE_Endpoint},
        .EndpointAddress        = BULK_OUT_EPADDR,
        .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC |
                                    ENDPOINT_USAGE_DATA),
        .EndpointSize           = BULK_EP_SIZE,
        .PollingIntervalMS      = 0x05,
    },
};

/* -----------------------------------------------------------------------
 * String descriptors
 * ----------------------------------------------------------------------- */
const USB_Descriptor_String_t PROGMEM LanguageString =
    USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

const USB_Descriptor_String_t PROGMEM ManufacturerString =
    USB_STRING_DESCRIPTOR(L"Meggy");

const USB_Descriptor_String_t PROGMEM ProductString =
    USB_STRING_DESCRIPTOR(L"Meggy Flash Programmer");

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                     const uint16_t wIndex,
                                     const void** const DescriptorAddress)
{
    (void)wIndex;

    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);
    const void*    Address          = NULL;
    uint16_t       Size             = NO_DESCRIPTOR;

    switch (DescriptorType) {
        case DTYPE_Device:
            Address = &DeviceDescriptor;
            Size    = sizeof(USB_Descriptor_Device_t);
            break;
        case DTYPE_Configuration:
            Address = &ConfigurationDescriptor;
            Size    = sizeof(USB_Descriptor_Configuration_t);
            break;
        case DTYPE_String:
            switch (DescriptorNumber) {
                case STRING_ID_Language:
                    Address = &LanguageString;
                    Size    = pgm_read_byte(&LanguageString.Header.Size);
                    break;
                case STRING_ID_Manufacturer:
                    Address = &ManufacturerString;
                    Size    = pgm_read_byte(&ManufacturerString.Header.Size);
                    break;
                case STRING_ID_Product:
                    Address = &ProductString;
                    Size    = pgm_read_byte(&ProductString.Header.Size);
                    break;
            }
            break;
    }

    *DescriptorAddress = Address;
    return Size;
}
