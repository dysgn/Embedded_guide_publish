/***************************
 * MainLoop.c
 *
 * Main loop processing
 *
 */
#include "main.h"

uint16_t activeLED = RED_LED;
GPIO_TypeDef *activeLEDPort = RED_LED_PORT;

static void CheckButton(void)
{
	static GPIO_PinState buttonPrev = GPIO_PIN_RESET;
	GPIO_PinState buttonCur;

	buttonCur = HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin);
	if (buttonCur == GPIO_PIN_SET && buttonPrev == GPIO_PIN_RESET)
	{
		HAL_GPIO_WritePin(activeLEDPort, activeLED, GPIO_PIN_RESET);
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
	buttonPrev = buttonCur;
}

void MainLoop(void)
{
	// Clear the timer
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	// Turn the Red LED On
	HAL_GPIO_WritePin(activeLEDPort, activeLED, GPIO_PIN_SET);
	// Wait until timer counts the desired number of ticks
	while(__HAL_TIM_GET_COUNTER(&htim2) < ON_TIME * SystemCoreClock)
	{
		CheckButton();
	}

	// Clear the timer again
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	// Turn the Red LED Off
	HAL_GPIO_WritePin(activeLEDPort, activeLED, GPIO_PIN_RESET);
	// Wait until timer reaches the threshold for Off time
	while(__HAL_TIM_GET_COUNTER(&htim2) < OFF_TIME * SystemCoreClock)
	{
		CheckButton();
	}
}
