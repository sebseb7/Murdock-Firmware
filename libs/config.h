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
#define CONFIG_BIAS_ACC_X 9
#define CONFIG_BIAS_ACC_Y 10
#define CONFIG_BIAS_ACC_Z 11
#define CONFIG_BIAS_GYRO_X 12
#define CONFIG_BIAS_GYRO_Y 13
#define CONFIG_BIAS_GYRO_Z 14

/*
#define CONFIG_STICK_CENTER_ELEV_H
#define CONFIG_STICK_CENTER_ELEV_L
#define CONFIG_STICK_CENTER_AIL_H
#define CONFIG_STICK_CENTER_AIL_L
#define CONFIG_STICK_CENTER_RUDD_H
#define CONFIG_STICK_CENTER_RUDD_L
#define CONFIG_RCLOST_GAS_H
#define CONFIG_RCLOST_GAS_L
#define CONFIG_WINGTYPE

 config todo:
  - wing type
  - receiver type (sp1 , sp1+sp2 , sbus, ppm )
  - gain
  - stick center
*/

void save_config(float w,float x,float y,float z,int16_t gx,int16_t gy,int16_t gz,int16_t ax,int16_t ay,int16_t az);
void save_config_sd(float w,float x,float y,float z,int16_t gx,int16_t gy,int16_t gz,int16_t ax,int16_t ay,int16_t az);
void load_config(float * w, float * x, float * y , float * z,int16_t * gx,int16_t * gy,int16_t * gz,int16_t * ax,int16_t * ay,int16_t * az);

#endif
