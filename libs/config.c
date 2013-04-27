#include "eeprom.h"
#include "config.h"

uint16_t VirtAddVarTab[NB_OF_VAR] = {1,2,3,4,5,6,7,8,9,10,11,12};

void dummy(void)
{
  FLASH_Unlock();
  EE_Init();
  uint16_t var = 0;
  EE_ReadVariable(CONFIG_QCAL_FLAT_XH, &var);
  FLASH_Lock();

}
