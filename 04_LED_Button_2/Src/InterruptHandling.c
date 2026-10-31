/*
 * InterruptHandling.c
 *
 *  Created on: 29 мая 2026 г.
 *      Author: dysgn
 */

#include "main.h"

static void HandleButtonPress(void)
{
	HAL_GPIO_WritePin((GPIO_TypeDef*)GPIOB_BASE, activeLED, GPIO_PIN_RESET);
	switch (activeLED)
	{
	case RED_LED:
		activeLED = BLUE_LED;
		activeLEDPort = BLUE_LED_PORT;
		break;

	case BLUE_LED:
		activeLED = GREEN_LED;
		activeLEDPort = GREEN_LED_PORT;
		break;

	default:
		activeLED = RED_LED;
		activeLEDPort = RED_LED_PORT;
		break;
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == USER_Btn_Pin)
	{
		HandleButtonPress();
	}
}
