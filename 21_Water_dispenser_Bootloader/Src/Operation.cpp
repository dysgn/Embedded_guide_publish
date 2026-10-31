/*
 * Operation.cpp
 *
 *  Created on: 13 авг. 2026 г.
 *      Author: dysgn
 */

#include <cstdio>
#include <cstring>
#include "Operation.hpp"
#include "HostCommunication.hpp"
#include "Device.hpp"
#include "crc16.hpp"

Operation Operation::s_OperationPool[MAX_OPERATIONS];
std::list<Operation *> Operation::s_freeOps;
std::list<Operation *> Operation::s_parsedOps;
std::list<Operation *> Operation::s_activeOps;
std::list<Operation *> Operation::s_completedOps;
uint32_t Operation::s_nextAsyncId = Operation::AsyncIdStart;

void Operation::InitOperations()
{
	for (int i = 0; i < MAX_OPERATIONS; i++)
	{
		s_freeOps.push_back(&s_OperationPool[i]);

		s_OperationPool[i].m_listIterator = std::prev(s_freeOps.end());
	}
}

Operation* Operation::CreateOperation(int retry, uint16_t transactionId,
		uint16_t deviceId, uint16_t opId, int argCount, Value32 args[])
{
	// prevent double execution
	if (retry > 0)
	{
		// check active operations
		for (auto it = s_activeOps.begin(); it != s_activeOps.end(); ++it)
		{
			if ((*it)->GetTransactionId() == transactionId)
			{
				// found one already executing, drop retry on the floor
				return nullptr;
			}
		}
		// check finished operations
		for (auto it = s_completedOps.begin(); it != s_completedOps.end(); ++it)
		{
			if ((*it)->GetTransactionId() == transactionId)
			{
				// found one already completed, drop retry on the floor
				return nullptr;
			}
		}
	}
	// Disable interrupts
	uint32_t priMask = __get_PRIMASK();
	__disable_irq();

	Operation *op = s_freeOps.front();
	s_parsedOps.splice(s_parsedOps.end(), s_freeOps, op->m_listIterator);

	// enable interrupts to their previous state
	__set_PRIMASK(priMask);

	op->m_transId = transactionId;
	op->m_deviceId = deviceId;
	op->m_opcode = opId;
	op->m_argCount = argCount;
	for (int i = 0; i < argCount; i++)
	{
		op->m_arguments[i] = args[i];
	}
	op->m_status = ErrorStatus::Pending;

	op->m_device = Device::FindDevice(op->m_deviceId);

	return op;
}

Operation* Operation::CreateAsyncMessage(uint16_t deviceId, uint16_t opId, Operation::ErrorStatus status, int argCount, Value32 args[])
{
	// Disable interrupts
	uint32_t priMask = __get_PRIMASK();
	__disable_irq();

	Operation *op = s_freeOps.front();
	s_completedOps.splice(s_completedOps.end(), s_freeOps, op->m_listIterator);

	// enable interrupts to their previous state
	__set_PRIMASK(priMask);

	op->m_transId = static_cast<uint16_t>(s_nextAsyncId);
	s_nextAsyncId++;
	if (s_nextAsyncId > AsyncIdEnd)
	{
		s_nextAsyncId = AsyncIdStart;
	}
	op->m_deviceId = deviceId;
	op->m_opcode = opId;
	op->m_status = status;
	op->m_retCount = argCount;
	for (int i = 0; i < argCount; i++)
	{
		op->m_retData[i] = args[i];
	}
	op->m_device = Device::FindDevice(op->m_deviceId);
	return op;
}

void Operation::Execute()
{
	// Disable interrupts
	uint32_t priMask = __get_PRIMASK();
	__disable_irq();

	s_activeOps.splice(s_activeOps.end(), s_parsedOps, m_listIterator);

	// enable interrupts to their previous state
	__set_PRIMASK(priMask);

	if (m_device->IsBusy(this))
	{
		Complete(ErrorStatus::DeviceIsBusy);
		return;
	}
	m_device->Execute(this);
}

void Operation::Complete(ErrorStatus errorCode, int retCount, Value32 retData[])
{
	m_status = errorCode;

	m_retCount = retCount;

	for (int i = 0; i < retCount; i++)
	{
		m_retData[i] = retData[i];
	}
	m_retRetries = 0;

	// Disable interrupts
	uint32_t priMask = __get_PRIMASK();
	__disable_irq();

	s_completedOps.splice(s_completedOps.end(), s_activeOps, m_listIterator);

	// enable interrupts to their previous state
	__set_PRIMASK(priMask);
}

void Operation::Complete(ErrorStatus errorCode, int retLength, const char *retData)
{
	m_status = errorCode;

	m_retCount = retLength;

	m_retData[0].Ptr = retData;

	m_retRetries = 0;

	// Disable interrupts
	uint32_t priMask = __get_PRIMASK();
	__disable_irq();

	s_completedOps.splice(s_completedOps.end(), s_activeOps, m_listIterator);

	// enable interrupts to their previous state
	__set_PRIMASK(priMask);
}


void Operation::CompleteBeforeExecution(ErrorStatus errorCode)
{
	m_status = errorCode;

	m_retCount = 0;
	m_retRetries = 0;

	// Disable interrupts
	uint32_t priMask = __get_PRIMASK();
	__disable_irq();

	s_completedOps.splice(s_completedOps.end(), s_parsedOps, m_listIterator);

	// enable interrupts to their previous state
	__set_PRIMASK(priMask);
}

void Operation::Discard()
{
	// Disable interrupts
	uint32_t priMask = __get_PRIMASK();
	__disable_irq();

	s_freeOps.splice(s_freeOps.end(), s_completedOps, m_listIterator);

	// enable interrupts to their previous state
	__set_PRIMASK(priMask);
}

uint8_t Operation::ConvertToHexDigit(uint8_t value)
{
	if (value < 10)
	{
		return static_cast<uint8_t>('0' + value);
	}
	else if (value < 16)
	{
		return static_cast<uint8_t>('A' + value - 10);
	}
	else
	{
		// shouldn't happen
		return 'F';
	}

}

uint32_t Operation::FormatResponseMessage(uint8_t *outputBuffer)
{
	outputBuffer[0] = STX;
	sprintf(reinterpret_cast<char *>(&outputBuffer[1]), "%04X", m_transId);
	outputBuffer[5] = ConvertToHexDigit(m_retRetries);
	sprintf(reinterpret_cast<char *>(&outputBuffer[6]), "%02X", m_status);
	// outputBuffer[8-9] will be filled after the payload will be formatted
	*reinterpret_cast<uint16_t *>(&outputBuffer[10]) = m_deviceId;
	*reinterpret_cast<uint16_t *>(&outputBuffer[12]) = m_opcode;
	outputBuffer[14] = ConvertToHexDigit(m_retCount);
	const char *retFormat = nullptr;
	if (m_device != nullptr)
	{
		retFormat = m_device->ResponseFormat(m_opcode);
	}
	else
	{
		// formatting for invalid device
		m_retCount = 0;
	}
	size_t offset = 15;
	if (retFormat != nullptr)
	{
		if (retFormat[0] == 'p')
		{
			// 'p' format excludes usage of other items besides first one
			memcpy(&outputBuffer[offset], m_retData[0].Ptr, m_retCount);
			offset += m_retCount;
		}
		else
		{
			for (int i = 0; i < m_retCount; i++)
			{
				switch (retFormat[i])
				{
				case 'i':
					offset += sprintf(reinterpret_cast<char *>(&outputBuffer[offset]), "%ld ", m_retData[i].Int);
					break;

				case 'u':
					offset += sprintf(reinterpret_cast<char *>(&outputBuffer[offset]), "%lu ", m_retData[i].Uint);
					break;

				case 'x':
					offset += sprintf(reinterpret_cast<char *>(&outputBuffer[offset]), "%08lX ", m_retData[i].Uint);
					break;

				case 'f':
					offset += sprintf(reinterpret_cast<char *>(&outputBuffer[offset]), "%f ", m_retData[i].Float);
					break;

				case 'c':
					memcpy(&outputBuffer[offset], m_retData[i].Char, 4);
					offset += 4;
					break;

				case '\x0':			// length mismatch, error in code
				default:			// any other character is garbage
					assert_param(0);
					break;
				}
			}
		}
	}
	// now we know the length and can put it into the packet
	char sizebuf[4];
	sprintf(sizebuf, "%02X", offset - 10);
	memcpy(&outputBuffer[8], sizebuf, 2);
	uint16_t crc = calculate_crc16(outputBuffer, offset);
	offset += sprintf(reinterpret_cast<char *>(&outputBuffer[offset]), "%04X", crc);
	outputBuffer[offset] = ETX;
	return offset + 1;
}

void Operation::ReportCompleted()
{
	while (!s_completedOps.empty())
	{
		auto *resp = HostCommunication::s_freeResponses.front();
		auto *op = s_completedOps.front();
		resp->m_size = op->FormatResponseMessage(resp->m_response);
		resp->m_transactionId = op->GetTransactionId();
		resp->m_retryAttempt = 0;
		resp->m_nextRetryTick = 0;
		HostCommunication::s_outstandingResponses.splice(HostCommunication::s_outstandingResponses.end(),
													HostCommunication::s_freeResponses,
													resp->m_iterator);
		op->Discard();
	}
}
