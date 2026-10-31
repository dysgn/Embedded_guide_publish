/*
 * ADC.c
 *
 *  Created on: 22 июн. 2026 г.
 *      Author: dysgn
 */
#include "stm32f7xx_hal.h"

#define ADC_RANGE	4095.0f

extern int adcResultsReady;
extern uint16_t adcValues[3];

void ADC_StartConversion(void);
