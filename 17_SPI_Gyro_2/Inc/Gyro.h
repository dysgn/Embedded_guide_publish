/*
 * Gyro.h
 *
 *  Created on: 6 июл. 2026 г.
 *      Author: dysgn
 */

#ifndef GYRO_H
#define GYRO_H

#include "stm32f7xx_hal.h"

void Gyro_Init(void);
void Gyro_Query(void);
int Gyro_DataReady(void);
void Gyro_Confirm(void);
void Gyro_Calculate(float *pitch, float *roll);

#endif
