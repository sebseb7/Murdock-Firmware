#include <string.h>

#include "eeprom.h"
#include "config.h"

uint16_t VirtAddVarTab[NB_OF_VAR] = {1,2,3,4,5,6,7,8};

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


void save_qcorr(float w,float x,float y,float z)
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


	FLASH_Lock();


}

void load_qcorr(float * w, float * x, float * y , float * z)
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
	

}



