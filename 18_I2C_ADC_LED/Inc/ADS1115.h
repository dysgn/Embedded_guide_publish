/*
 * ADS1115.h
 *
 *  Created on: 22 июл. 2026 г.
 *      Author: dysgn
 */

#ifndef ADS1115_H_
#define ADS1115_H_

#include "stm32f7xx_hal.h"

void ADS1115_Init(void);
int16_t ADS1115_GetValue(void);
int ADS1115_ValueReady(void);
void ADS1115_PeriodicService(void);

#endif /* ADS1115_H_ */
