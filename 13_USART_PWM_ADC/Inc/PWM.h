/*
 * PWM.h
 *
 *  Created on: 18 июн. 2026 г.
 *      Author: dysgn
 */

#include "stm32f7xx_hal.h"

int SetPWMFrequency(TIM_HandleTypeDef *timerModule, float frequency_kHz);
int SetPWMDutyCycle(TIM_HandleTypeDef *timerModule, uint32_t channel, float dutyCyclePct);
void GetPWMSettings(TIM_HandleTypeDef *timerModule, uint32_t channel, float *frequency, float *dutyCycle);
