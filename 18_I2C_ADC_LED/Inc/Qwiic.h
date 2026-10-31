/*
 * Qwiic.h
 *
 *  Created on: 22 июл. 2026 г.
 *      Author: dysgn
 */

#ifndef QWIIC_H_
#define QWIIC_H_

#include "stm32f7xx_hal.h"

void Qwiic_SetLEDColor(uint8_t idx, uint8_t red, uint8_t green, uint8_t blue);
void Qwiic_SetLEDBrightness(uint8_t idx, uint8_t brightness);
void Qwiic_SetAllColor(uint8_t red, uint8_t green, uint8_t blue);
void Qwiic_SetAllBrightness(uint8_t brightness);


#endif /* QWIIC_H_ */
