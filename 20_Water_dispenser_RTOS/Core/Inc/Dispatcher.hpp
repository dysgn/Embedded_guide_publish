/*
 * Dispatcher.hpp
 *
 *  Created on: 24 сент. 2026 г.
 *      Author: dysgn
 */

#ifndef INC_DISPATCHER_HPP_
#define INC_DISPATCHER_HPP_

#include "FreeRTOS.h"
#include "queue.h"
#include "HostCommunication.hpp"
#include "Operation.hpp"

#define CMD_QUEUE_SIZE	8

#define RESP_QUEUE_SIZE	32

class Dispatcher final
{
	static CommandPacket s_incomingQueueBuffer[CMD_QUEUE_SIZE];

	static StaticQueue_t s_incomingStaticQueueBuffer;

	static QueueHandle_t s_IncomingCmds;

	static Operation *s_completedOperationsBuffer[RESP_QUEUE_SIZE];

	static StaticQueue_t s_completedStaticQueueBuffer;

	static QueueHandle_t s_completedOpsQueue;
public:
	static void RunCommands();

	static void NewCommand(CommandPacket *cmd);

	static void InitResponseQueue();

	static void GetNextResponse(ResponsePacket &response);

	static void NewResponse(Operation *op);

	static int PurgeQueue();
};




#endif /* INC_DISPATCHER_HPP_ */
