#ifndef _ARMMATH_H
#define _ARMMATH_H

#include "main.h"
#include <stdint.h>

uint16_t sini(uint16_t x);
uint16_t cosi(uint16_t x);
double pythagoras( double side1, double side2 );
float pythagorasf( float side1, float side2 );
uint16_t randr(uint16_t start,uint16_t end);
float _sinf(float theta);
float _cosf(float theta);

__attribute__( ( always_inline ) ) __INLINE float __VSQRTF(float op1)
{
#if defined(__arm__) && defined (__ARM_PCS_VFP)
	float result;
	__ASM volatile ("vsqrt.f32 %0, %1" : "=w" (result) : "w" (op1) );
	return(result);
#else
	return sqrtf(op1);
#endif
}

#endif
