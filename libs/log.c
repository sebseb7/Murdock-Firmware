#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

#include "usb_serial.h"
#include "leds.h"
#include "ff.h"
#include "microsd_spi.h"


static uint16_t sd_card_available = 0;
static FATFS FATFS_Obj;
static FIL file;

void log_init(void)
{
	disk_initialize(0);
	delay(1);
	int result = disk_initialize(0);
	if(result == 0)
	{
		led_on(LED_SDCARD);
		sd_card_available = 1;
		usb_printf("SD ok\n");
		f_mount(0, &FATFS_Obj);
		int result = f_open(&file, "0:log.txt", FA_OPEN_ALWAYS | FA_WRITE);
		if(result != 0)
		{
			usb_printf("file open error\n");
			led_fastBlink(LED_SDCARD);
			sd_card_available = 0;
			return;
		}
		result = f_sync(&file);
		if(result != 0)
		{
			usb_printf("file sync error\n");
			led_fastBlink(LED_SDCARD);
			sd_card_available = 0;
			return;
		}
		result = f_lseek(&file, file.fsize);
		if(result != 0)
		{
			usb_printf("file lseek error\n");
			led_fastBlink(LED_SDCARD);
			sd_card_available = 0;
			return;
		}
		
	}
	else
	{
		usb_printf("no SD\n");
	}
}
void log_printf(const char* text, ...)
{
	char line[256];
	va_list args;
	va_start(args,text);
	vsnprintf(line,256,text,args);
	va_end(args);
		
	if(sd_card_available == 0)
	{
		return;
	}
	
	unsigned int len = strlen(line);

	char prefix[256];

	uint32_t millis = get_systick() / 10;
	uint32_t seconds = millis / 1000;
	uint32_t fract = millis-(seconds*1000);

	snprintf(prefix,256,"[%5lu.%03lu] ",seconds,fract);
	int lenpr = strlen(prefix);

	
	unsigned int bw=0;
	f_write(&file, prefix, lenpr, &bw);	
			
	unsigned int result = f_write(&file, line, len, &bw);	

	if(result != 0)
	{
		usb_printf("file write error\n");
		led_fastBlink(LED_SDCARD);
		sd_card_available = 0;
		return;
	}
	if(bw != len)
	{
		usb_printf("file write bytes mismatch\n");
		led_fastBlink(LED_SDCARD);
		sd_card_available = 0;
		return;
	}

	result = f_sync(&file);
	
	if(result != 0)
	{
		usb_printf("file sync error\n");
		led_fastBlink(LED_SDCARD);
		sd_card_available = 0;
		return;
	}
}
	
	
