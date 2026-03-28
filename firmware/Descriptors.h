#pragma once

#include <LUFA/Drivers/USB/USB.h>

/* Bulk endpoint addresses and size */
#define BULK_IN_EPADDR   (ENDPOINT_DIR_IN  | 3)   /* EP3 IN  — device→host */
#define BULK_OUT_EPADDR  (ENDPOINT_DIR_OUT | 4)   /* EP4 OUT — host→device */
#define BULK_EP_SIZE     64

typedef struct {
    USB_Descriptor_Configuration_Header_t  Config;
    USB_Descriptor_Interface_t             DataInterface;
    USB_Descriptor_Endpoint_t              DataInEndpoint;
    USB_Descriptor_Endpoint_t              DataOutEndpoint;
} USB_Descriptor_Configuration_t;

enum InterfaceDescriptors_t {
    INTERFACE_ID_Data = 0,
};

enum StringDescriptors_t {
    STRING_ID_Language     = 0,
    STRING_ID_Manufacturer = 1,
    STRING_ID_Product      = 2,
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                     const uint16_t wIndex,
                                     const void** const DescriptorAddress)
                                     ATTR_WARN_UNUSED_RESULT
                                     ATTR_NON_NULL_PTR_ARG(3);
