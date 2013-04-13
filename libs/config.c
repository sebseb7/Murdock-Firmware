#include "eeprom.h"
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x0, 0x1, 0x2};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};


void dummy(void)
{
  FLASH_Unlock();
  EE_Init();
  EE_ReadVariable(VirtAddVarTab[0], &VarDataTab[0]);
  EE_ReadVariable(VirtAddVarTab[1], &VarDataTab[1]);
  EE_ReadVariable(VirtAddVarTab[2], &VarDataTab[2]);
  FLASH_Lock();

}
