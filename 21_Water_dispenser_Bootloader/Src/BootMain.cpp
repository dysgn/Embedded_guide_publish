/*
 * AppMain.cpp
 *
 *  Created on: 12 авг. 2026 г.
 *      Author: dysgn
 */

#include <list>
#include "main.h"
#include "BootMain.hpp"
#include "HostCommunication.hpp"
#include "Device.hpp"
#include "Operation.hpp"
#include "BootInfo.h"

Board s_board(BoardDeviceId);
extern BootInfo bootInfo;

extern "C" void BootMain()
{
	if (bootInfo.m_magicNumber != MAGIC_NUMBER || bootInfo.m_stayInBoot == 0)
	{
		// Hardware or power-on reset
		if (s_board.IsAppPresent())
		{
			// We have app, jump to it right away
			s_board.JumpToApp();
			// No bootloader code executes from here
		}
		else
		{
			// No app is present, stay in bootloader
			bootInfo.m_magicNumber = MAGIC_NUMBER;
			bootInfo.m_stayInBoot = 1;
			bootInfo.m_reason = ResetReason::NoApp;
			bootInfo.m_faultAddress = nullptr;
		}
	}
	else
	{
		// Application has written the information on why we got back to bootloader
		// Start bootloader in full
	}

	HostCommunication::CreateHostPort(&huart2);

	HostCommunication *port = HostCommunication::GetPort();
	Operation::InitOperations();
	Device::InitAll();

	port->StartListening();
	while (1)
	{
		if (port->ReadyToProcess())
		{
			uint32_t tId;
			bool ack;
			uint8_t retry;
			if (port->VerifyPacket(tId, ack, retry))
			{
				if (ack)
				{
					port->Processed();
					port->ReportIsAcked(tId);
				}
				else
				{
					port->SendAck(tId, retry);
					Operation *op = port->ParseCommandPacket();
					port->Processed();
					if (op != nullptr)
					{
						op->Execute();
					}
				}
			}
		}

		Operation::ReportCompleted();
		HostCommunication::GetPort()->ProcessReports();
	}
}

static std::list<Device *> s_periodicHandlers;

void AddPeriodicHandler(Device* handlerDevice)
{
	__disable_irq();
	s_periodicHandlers.push_back(handlerDevice);
	__enable_irq();
}

extern "C" void PeriodicService()
{
	for (auto it = s_periodicHandlers.begin(); it != s_periodicHandlers.end(); ++it)
	{
		(*it)->PeriodicHandler();
	}
}

