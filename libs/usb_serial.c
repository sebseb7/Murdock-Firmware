#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#include "usb_serial.h"

#ifdef USE_USB_OTG_FS

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

extern uint8_t  APP_Rx_Buffer []; 
extern uint32_t APP_Rx_ptr_in;


void usb_serial_init(void)
{
	USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
			USB_OTG_HS_CORE_ID,
#else            
			USB_OTG_FS_CORE_ID,
#endif  
			&USR_desc, 
			&USBD_CDC_cb, 
			&USR_cb);

}

void usb_printf(const char* text, ...)
{
	char line[256];
	va_list args;
	va_start(args,text);
	vsnprintf(line,256,text,args);
	va_end(args);

	int len = strlen(line);

	char prefix[256];

	uint32_t millis = get_systick() / 10;
	uint32_t seconds = millis / 1000;
	uint32_t fract = millis-(seconds*1000);

	snprintf(prefix,256,"[%5lu.%03lu] ",seconds,fract);
	int lenpr = strlen(prefix);

	
	for(int i=0;i<lenpr;i++)
	{
		APP_Rx_Buffer[APP_Rx_ptr_in] = prefix[i];
		APP_Rx_ptr_in++;

		/* To avoid buffer overflow */
		if(APP_Rx_ptr_in >= APP_RX_DATA_SIZE)
		{
			APP_Rx_ptr_in = 0;
		}  
	}


	for(int i=0;i<len;i++)
	{
		APP_Rx_Buffer[APP_Rx_ptr_in] = line[i];
		APP_Rx_ptr_in++;

		/* To avoid buffer overflow */
		if(APP_Rx_ptr_in >= APP_RX_DATA_SIZE)
		{
			APP_Rx_ptr_in = 0;
		}  
	}
}
void write_usb(uint8_t byte)
{


		APP_Rx_Buffer[APP_Rx_ptr_in] = byte ;
		APP_Rx_ptr_in++;

		/* To avoid buffer overflow */
		if(APP_Rx_ptr_in >= APP_RX_DATA_SIZE)
		{
			APP_Rx_ptr_in = 0;
		}  
}
#endif
