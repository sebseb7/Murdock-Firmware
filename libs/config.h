#ifndef CONFIG_H_
#define CONFIG_H_

#include "main.h"


#define CONFIG_QCAL_WH 1
#define CONFIG_QCAL_WL 2
#define CONFIG_QCAL_XH 3
#define CONFIG_QCAL_XL 4
#define CONFIG_QCAL_YH 5
#define CONFIG_QCAL_YL 6
#define CONFIG_QCAL_ZH 7
#define CONFIG_QCAL_ZL 8

void save_qcorr(float w,float x,float y,float z);
void load_qcorr(float * w, float * x, float * y , float * z);

#endif
