/*
 * PWM.h
 *
 *  Created on: 18 июн. 2026 г.
 *      Author: dysgn
 */
#ifndef __PWM_H
#define __PWM_H

#include "stm32f7xx_hal.h"

int SetPWM(TIM_HandleTypeDef *timerModule, uint32_t channel, float frequency_kHz, float dutyCyclePct);

#endif
