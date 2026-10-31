/*
 * AppThreads.h
 *
 *  Created on: 23 сент. 2026 г.
 *      Author: dysgn
 */

#ifndef INC_APPTHREADS_H_
#define INC_APPTHREADS_H_

#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "event_groups.h"

extern const osThreadAttr_t networkRcvTask_attributes;
extern const osThreadAttr_t networkSndTask_attributes;
extern const osThreadAttr_t dispatcherCmdTask_attributes;
extern const osThreadAttr_t periodicHandlerTask_attributes;

extern EventGroupHandle_t systemEvtGrp;

#define BIT_SYSTICK_FIRED		(1 << 0)


#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

void NetworkStartup();
void NetworkReceiveThread(void *param);
void NetworkSendThread(void *param);

void DispatcherCommandThread(void *param);

void PeriodicHandlerThread(void *param);

void systemInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INC_APPTHREADS_H_ */
