#ifndef USB_SERIAL_H_
#define USB_SERIAL_H_

#include "main.h"

#ifdef USE_USB_OTG_FS
void usb_printf(const char* text, ...);
void usb_serial_init(void);
#endif


#endif
