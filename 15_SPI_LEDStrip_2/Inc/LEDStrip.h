/*
 * LEDStrip.h
 *
 *  Created on: 29 июн. 2026 г.
 *      Author: dysgn
 */
#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <stdint.h>

#define STRIP_LENGTH	60

void LEDStrip_Init(void);
void LEDStrip_Clear();
int LEDStrip_SetLED(uint32_t ledIdx, uint32_t brightness, uint32_t red, uint32_t green, uint32_t blue);
void LEDStrip_Fill(uint32_t brightness, uint32_t red, uint32_t green, uint32_t blue);
int LEDStrip_Rotate(int dist);
void LEDStrip_UpdateStrip(void);
void LEDStrip_SetWave(int32_t step, uint32_t periodMs);
void LEDStrip_HandleWave(void);

#endif
