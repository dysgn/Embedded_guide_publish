/*
 * InterruptHandling.c
 *
 *  Created on: 29 мая 2026 г.
 *      Author: dysgn
 */

#include "main.h"

static uint16_t activeLED = RED_LED;
static GPIO_TypeDef *activeLEDPort = RED_LED_PORT;

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

static void HandleBlink(void)
{
	static int stateIsOn = 0;

	if (stateIsOn)
	{
		// Turn the LED Off
		HAL_GPIO_WritePin(activeLEDPort, activeLED, GPIO_PIN_RESET);
		// Reload the timer for Off period
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, OFF_TIME * SystemCoreClock);
		stateIsOn = 0;
	}
	else
	{
		// Turn the LED Off
		HAL_GPIO_WritePin(activeLEDPort, activeLED, GPIO_PIN_SET);
		// Reload the timer for Off period
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ON_TIME * SystemCoreClock);
		stateIsOn = 1;
	}
	__HAL_TIM_SET_COUNTER(&htim2, 0);
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		HandleBlink();
	}
}
