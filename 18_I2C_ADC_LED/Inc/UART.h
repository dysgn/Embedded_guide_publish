/*
 * UART.h
 *
 *  Created on: 16 июн. 2026 г.
 *      Author: dysgn
 */
#ifndef UART_H
#define UART_H

#include <stdint.h>

extern int commandReceived;
extern uint8_t commandBuffer[256];
extern uint32_t commandLength;

int IsReceivingCommand();
void ReceiveCommand();
void SendMessage(uint8_t *message);

#endif
