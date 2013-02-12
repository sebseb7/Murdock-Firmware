#ifndef USB_SERIAL_H_
#define USB_SERIAL_H_

#ifdef USE_USB_OTG_FS
void usb_printf(const char* text, ...);
void usb_serial_init(void);
#endif


#endif
