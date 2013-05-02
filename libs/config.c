#include <string.h>

#include "eeprom.h"
#include "config.h"
#include "log.h"

#include "eeprom_fill.h"



uint16_t VirtAddVarTab[NB_OF_VAR] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};

static uint32_t pack(float source)
{
	uint32_t target;
	memcpy(&target, &source, sizeof(target));
	return target;
}
static float unpack(uint32_t source)
{
	float target;
	memcpy(&target, &source, sizeof(target));
	return target;
}


void save_config(float w,float x,float y,float z,int16_t gx,int16_t gy,int16_t gz,int16_t ax,int16_t ay,int16_t az)
{
	uint32_t wtmp,xtmp,ytmp,ztmp;

	wtmp = pack(w);
	xtmp = pack(x);
	ytmp = pack(y);
	ztmp = pack(z);

	FLASH_Unlock();
	EE_Init();

	EE_WriteVariable(CONFIG_QCAL_WL,wtmp & 0xffff);
	EE_WriteVariable(CONFIG_QCAL_XL,xtmp & 0xffff);
	EE_WriteVariable(CONFIG_QCAL_YL,ytmp & 0xffff);
	EE_WriteVariable(CONFIG_QCAL_ZL,ztmp & 0xffff);
	
	EE_WriteVariable(CONFIG_QCAL_WH,wtmp >> 16);
	EE_WriteVariable(CONFIG_QCAL_XH,xtmp >> 16);
	EE_WriteVariable(CONFIG_QCAL_YH,ytmp >> 16);
	EE_WriteVariable(CONFIG_QCAL_ZH,ztmp >> 16);

	EE_WriteVariable(CONFIG_BIAS_ACC_X,ax);
	EE_WriteVariable(CONFIG_BIAS_ACC_Y,ay);
	EE_WriteVariable(CONFIG_BIAS_ACC_Z,az);
	EE_WriteVariable(CONFIG_BIAS_GYRO_X,gx);
	EE_WriteVariable(CONFIG_BIAS_GYRO_Y,gy);
	EE_WriteVariable(CONFIG_BIAS_GYRO_Z,gz);


	FLASH_Lock();


}

void load_config(float * w, float * x, float * y , float * z,int16_t * gx,int16_t * gy,int16_t * gz,int16_t * ax,int16_t * ay,int16_t * az)
{
	uint32_t wtmp,xtmp,ytmp,ztmp;
	uint16_t wtmpl,xtmpl,ytmpl,ztmpl;
	uint16_t wtmph,xtmph,ytmph,ztmph;

	EE_ReadVariable(CONFIG_QCAL_WL,&wtmpl);
	EE_ReadVariable(CONFIG_QCAL_XL,&xtmpl);
	EE_ReadVariable(CONFIG_QCAL_YL,&ytmpl);
	EE_ReadVariable(CONFIG_QCAL_ZL,&ztmpl);
	EE_ReadVariable(CONFIG_QCAL_WH,&wtmph);
	EE_ReadVariable(CONFIG_QCAL_XH,&xtmph);
	EE_ReadVariable(CONFIG_QCAL_YH,&ytmph);
	EE_ReadVariable(CONFIG_QCAL_ZH,&ztmph);

	wtmp = (wtmph << 16) | wtmpl;
	xtmp = (xtmph << 16) | xtmpl;
	ytmp = (ytmph << 16) | ytmpl;
	ztmp = (ztmph << 16) | ztmpl;

	*w = unpack(wtmp);	
	*x = unpack(xtmp);	
	*y = unpack(ytmp);	
	*z = unpack(ztmp);	
	
	EE_ReadVariable(CONFIG_BIAS_ACC_X,(uint16_t*)ax);
	EE_ReadVariable(CONFIG_BIAS_ACC_Y,(uint16_t*)ay);
	EE_ReadVariable(CONFIG_BIAS_ACC_Z,(uint16_t*)az);
	EE_ReadVariable(CONFIG_BIAS_GYRO_X,(uint16_t*)gx);
	EE_ReadVariable(CONFIG_BIAS_GYRO_Y,(uint16_t*)gy);
	EE_ReadVariable(CONFIG_BIAS_GYRO_Z,(uint16_t*)gz);

}

struct packed_config_vars_t
{
	union  
	{
		struct
		{
			float w;
			float x;
			float y;
			float z;
			int16_t gx;
			int16_t gy;
			int16_t gz;
			int16_t ax;
			int16_t ay;
			int16_t az;
		};
		uint8_t buffer[28];
	};
};

void save_config_sd(float w,float x,float y,float z,int16_t gx,int16_t gy,int16_t gz,int16_t ax,int16_t ay,int16_t az)
{
	struct packed_config_vars_t  packed_config_vars;

	packed_config_vars.w=w;
	packed_config_vars.x=x;
	packed_config_vars.y=y;
	packed_config_vars.z=z;
	packed_config_vars.gx=gx;
	packed_config_vars.gy=gy;
	packed_config_vars.gz=gz;
	packed_config_vars.ax=ax;
	packed_config_vars.ay=ay;
	packed_config_vars.az=az;

	write_to_file("config.dat",packed_config_vars.buffer,28);


}

