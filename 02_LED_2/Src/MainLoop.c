/***************************
 * MainLoop.c
 *
 * Main loop processing
 *
 */
#include "main.h"

void MainLoop(void)
{
	// Clear the timer
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	// Turn the Red LED On
	HAL_GPIO_WritePin(RED_LED_PORT, RED_LED, GPIO_PIN_SET);
	// Wait until timer counts the desired number of ticks
	while(__HAL_TIM_GET_COUNTER(&htim2) < ON_TIME * SystemCoreClock)
	{
	}

	// Clear the timer again
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	// Turn the Red LED Off
	HAL_GPIO_WritePin(RED_LED_PORT, RED_LED, GPIO_PIN_RESET);
	// Wait until timer reaches the threshold for Off time
	while(__HAL_TIM_GET_COUNTER(&htim2) < OFF_TIME * SystemCoreClock)
	{
	}
}
