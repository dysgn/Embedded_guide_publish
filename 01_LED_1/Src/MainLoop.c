/***************************
 * MainLoop.c
 *
 * Main loop processing
 *
 */
#include "main.h"

void MainLoop(void)
{
	// Turn the Red LED On
	HAL_GPIO_WritePin(RED_LED_PORT, RED_LED, GPIO_PIN_SET);
	// Delay for some time
	for (volatile int i = 0; i < 10000000; i++)
	{
	}

	// Turn the Red LED On
	HAL_GPIO_WritePin(RED_LED_PORT, RED_LED, GPIO_PIN_RESET);
	// Delay for the same time
	for (volatile int i = 0; i < 10000000; i++)
	{
	}
}
