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
		f_mount(0, &FATFS_Obj);

		char filename[200];
		{
			int filecounter = 0;
			FILINFO filetest;
			sprintf(filename,"0:log%05u.txt",filecounter);
			while(f_stat(filename,&filetest) == FR_OK)
			{
				filecounter++;
				sprintf(filename,"0:log%05u.txt",filecounter);
			}
		}
		int result = f_open(&file, filename, FA_CREATE_NEW | FA_WRITE);
		if(result != 0)
		{
			led_fastBlink(LED_SDCARD);
			sd_card_available = 0;
			return;
		}
		result = f_sync(&file);
		if(result != 0)
		{
			led_fastBlink(LED_SDCARD);
			sd_card_available = 0;
			return;
		}
		result = f_lseek(&file, file.fsize);
		if(result != 0)
		{
			led_fastBlink(LED_SDCARD);
			sd_card_available = 0;
			return;
		}
		//usb_printfi_buffered("SD filename: %s\n",filename);
		led_on(LED_SDCARD);
		sd_card_available = 1;
		
	}
	{
		//usb_printf_buffered("NO SD");
	}
}
void log_printf(const char* text, ...)
{
	char tmp[256];
	va_list args;
	va_start(args,text);
	vsnprintf(tmp,256,text,args);
	va_end(args);
		
	if(sd_card_available == 0)
	{
		return;
	}
	
	char line[256];

	uint32_t millis = get_systick() / 10;
	uint32_t seconds = millis / 1000;
	uint32_t fract = millis-(seconds*1000);

	snprintf(line,256,"[%5lu.%03lu] %s",seconds,fract,tmp);
	unsigned int len = strlen(line);
	
	unsigned int bw=0;
			
	unsigned int result = f_write(&file, line, len, &bw);	

	if(result != 0)
	{
		led_fastBlink(LED_SDCARD);
		sd_card_available = 0;
		return;
	}
	if(bw != len)
	{
		led_fastBlink(LED_SDCARD);
		sd_card_available = 0;
		return;
	}

	result = f_sync(&file);
	
	if(result != 0)
	{
		led_fastBlink(LED_SDCARD);
		sd_card_available = 0;
		return;
	}
}
	
	
