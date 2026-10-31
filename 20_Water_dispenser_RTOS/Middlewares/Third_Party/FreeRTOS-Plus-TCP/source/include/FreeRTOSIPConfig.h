/*
 * FreeRTOSIPConfig.h
 *
 *  Created on: 21 сент. 2026 г.
 *      Author: dysgn
 */

#ifndef FREERTOS_IP_CONFIG_H
#define FREERTOS_IP_CONFIG_H

// Cortex CPUs are little endian
#define ipconfigBYTE_ORDER						  pdFREERTOS_LITTLE_ENDIAN

#define ipconfigTCP_WIN_SEG_COUNT    ( 16 )

#define ipconfigNETWORK_MTU           1500

/* Buffer allocation configuration for STM32F7 D-Cache alignment */
#define ipconfigBUFFER_ALLOCATION_STYLE           1

/* Enable the network event hook callback */
#define ipconfigUSE_NETWORK_EVENT_HOOK            1

/* Set to 1 to use DHCP or 0 to force static IP settings from the Endpoint structure */
#define ipconfigUSE_DHCP                          1

/* Core TCP/IP stack task management configuration */
#define ipconfigIP_TASK_PRIORITY                  ( configMAX_PRIORITIES - 2 )
#define ipconfigIP_TASK_STACK_SIZE_WORDS          ( configMINIMAL_STACK_SIZE * 5 )

/* The length of the network event command queue */
#define ipconfigEVENT_QUEUE_LENGTH                ( ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS + 20 )

/* Support locking to allow full duplex operation */
#define ipconfigSOCK_LOCK_SUPPORT      1

#define ipconfigZERO_COPY_TX_DRIVER		1

#define ipconfigZERO_COPY_RX_DRIVER		1

#define ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES    ipconfigDISABLE

/* Convert OS ticks to milliseconds */
#ifndef pdTICKS_TO_MS
    #define pdTICKS_TO_MS( xTicks )    ( ( ( uint32_t ) ( xTicks ) * 1000U ) / ( uint32_t ) configTICK_RATE_HZ )
#endif

#endif /* FREERTOS_IP_CONFIG_H */
