/*
 * I2CDispatcher.h
 *
 *  Created on: 16 июл. 2026 г.
 *      Author: dysgn
 */

#ifndef I2CDISPATCHER_H_
#define I2CDISPATCHER_H_

#include "stm32f7xx_hal.h"

typedef void (*I2CCallback)(uint32_t errorCode);

HAL_StatusTypeDef I2C_WriteBlocking(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef I2C_WriteNonBlocking(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, I2CCallback callback);
HAL_StatusTypeDef I2CWriteRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
					uint8_t *pCmd, uint16_t cmdSize, uint8_t *pData, uint16_t dataSize, I2CCallback callback);
HAL_StatusTypeDef I2CBusRecovery(void);
void I2CCheckBusAlive(void);


#endif /* I2CDISPATCHER_H_ */
