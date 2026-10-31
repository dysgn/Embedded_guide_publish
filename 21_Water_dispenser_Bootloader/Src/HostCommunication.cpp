/*
 * HostCommunication.cpp
 *
 *  Created on: 12 авг. 2026 г.
 *      Author: dysgn
 */

#include <cstdlib>
#include <charconv>
#include <list>
#include <cstring>
#include <cstdio>
#include "HostCommunication.hpp"
#include "crc16.hpp"
#include "Operation.hpp"
#include "Device.hpp"

// Host port is singleton
HostCommunication *HostCommunication::s_Instance = nullptr;
OutstandingResponse HostCommunication::s_responseBufferPool[MAX_OUTSTANDING_RESPONSES];
std::list<OutstandingResponse *> HostCommunication::s_freeResponses;
std::list<OutstandingResponse *> HostCommunication::s_outstandingResponses;

HostCommunication::HostCommunication(UART_HandleTypeDef *port) :
					m_UARTPort(port)
{
	if (s_Instance == nullptr)
	{
		s_Instance = this;
		for (int i = 0; i < MAX_OUTSTANDING_RESPONSES; i++)
		{
			s_freeResponses.push_back(&s_responseBufferPool[i]);
			s_responseBufferPool[i].m_iterator = std::prev(s_freeResponses.end());
		}
	}
}

void HostCommunication::CreateHostPort(UART_HandleTypeDef *port)
{
	s_Instance = new HostCommunication(port);
}

HostCommunication* HostCommunication::GetPort()
{
	return s_Instance;
}

void HostCommunication::StartListening()
{
	ReceiveBuffer *buffer = &m_receiveBuffer[m_receiveBufIdx];
	buffer->m_receiveIdx = 0;
	__HAL_UART_DISABLE_IT(m_UARTPort, UART_IT_RXNE);
	buffer->m_receiveState = ReceiveState::WaitSTX;
	HAL_UART_Receive_IT(m_UARTPort, &buffer->m_data[buffer->m_receiveIdx], 1);
	__HAL_UART_ENABLE_IT(m_UARTPort, UART_IT_RXNE);
}

bool  HostCommunication::VerifyPacket(uint32_t &tId, bool &ack, uint8_t &retry)
{
	ReceiveBuffer *buffer = &m_receiveBuffer[m_processingBufIdx];
	if (buffer->m_data[0] != STX || buffer->m_data[buffer->m_receiveIdx - 1] != ETX)
	{
		// framing wrong
		buffer->m_receiveState = ReceiveState::Idle;
		return false;
	}
	if (buffer->m_data[5] == 'K')
	{
		ack = true;
		std::from_chars(reinterpret_cast<char *>(&buffer->m_data[1]),
						reinterpret_cast<char *>(&buffer->m_data[5]),
						tId, 16);
		// finished with incoming packet
		buffer->m_receiveState = ReceiveState::Idle;
		return true;		// no other checks
	}
	// if not 'K', it's command packet and this position is retry count
	ack = false;	// 	don't set m_receiveState to Idle yet, we need to parse the packet
	retry = buffer->m_data[5];

	if (buffer->m_payloadLen + PACKET_FRAMING_OVERHEAD != buffer->m_receiveIdx)
	{
		// mismatch between expected and actual packet length - lost characters?
		buffer->m_receiveState = ReceiveState::Idle;
		return false;
	}
	if (buffer->m_payloadLen < MINIMUM_PAYLOAD_SIZE)
	{
		buffer->m_receiveState = ReceiveState::Idle;
		return false;
	}
	uint32_t packetCrc;
	std::from_chars(reinterpret_cast<char *>(&buffer->m_data[PACKET_HEADER_SIZE + buffer->m_payloadLen]),
					reinterpret_cast<char *>(&buffer->m_data[PACKET_HEADER_SIZE + buffer->m_payloadLen + 4]),
					packetCrc, 16);
	uint32_t actualCrc = calculate_crc16(buffer->m_data, PACKET_HEADER_SIZE + buffer->m_payloadLen);
	if (actualCrc != packetCrc)
	{
		buffer->m_receiveState = ReceiveState::Idle;
		return false;
	}
	// Now we can trust the packet content
	std::from_chars(reinterpret_cast<char *>(&buffer->m_data[1]),
					reinterpret_cast<char *>(&buffer->m_data[5]),
					tId, 16);
	return true;
}

void HostCommunication::SendAck(uint32_t tId, uint8_t onRetry)
{
	auto *ack = s_freeResponses.front();
	ack->m_size = ACK_SIZE;
	ack->m_transactionId = tId;
	ack->m_retryAttempt = 0;
	ack->m_nextRetryTick = 0;
	ack->m_response[0] = STX;
	sprintf(reinterpret_cast<char *>(&ack->m_response[1]), "%04lX", tId);
	ack->m_response[5] = 'K';
	ack->m_response[6] = onRetry;
	ack->m_response[7] = ETX;

	// Disable interrupts
	uint32_t priMask = __get_PRIMASK();
	__disable_irq();

	HostCommunication::s_outstandingResponses.splice(HostCommunication::s_outstandingResponses.end(),
												HostCommunication::s_freeResponses,
												ack->m_iterator);
	// enable interrupts to their previous state
	__set_PRIMASK(priMask);
}

Operation *HostCommunication::ParseCommandPacket()
{
	ReceiveBuffer *buffer = &m_receiveBuffer[m_processingBufIdx];
	// get retry count
	int retry = ParseHexDigit(buffer->m_data[5]);
	// get transaction Id
	uint32_t tId;
	std::from_chars(reinterpret_cast<char *>(&buffer->m_data[1]),
					reinterpret_cast<char *>(&buffer->m_data[5]),
					tId, 16);

	// get device Id and opcode
	uint16_t devId = *reinterpret_cast<uint16_t*>(&buffer->m_data[8]);
	uint16_t opcode = *reinterpret_cast<uint16_t*>(&buffer->m_data[10]);

	// find device object
	Device *device = Device::FindDevice(devId);
	if (device == nullptr)
	{
		// device object not found: must be wrong Id
		Operation *op = Operation::CreateOperation(0, tId,  devId,  opcode, 0, nullptr);
		op->CompleteBeforeExecution(Operation::ErrorStatus::InvalidDevice);
		return nullptr;
	}
	// get argument format for this operation
	const char *argFormat = device->ArgumentFormat(opcode);
	if (argFormat == nullptr)
	{
		// device object not found: must be wrong Id
		Operation *op = Operation::CreateOperation(0, tId,  devId,  opcode, 0, nullptr);
		op->CompleteBeforeExecution(Operation::ErrorStatus::InvalidCommand);
		return nullptr;
	}

	// parse argument
	Value32 args[MAX_ARGUMENTS];
	int argCount = ParseHexDigit(buffer->m_data[12]);
	if (strlen(argFormat) != static_cast<size_t>(argCount))
	{
		// incorrect number of arguments was passed, don't parse actual ones
		Operation *op = Operation::CreateOperation(0, tId,  devId,  opcode, 0, nullptr);
		op->CompleteBeforeExecution(Operation::ErrorStatus::InvalidArgumentCount);
		return nullptr;
	}
	char *ptr = reinterpret_cast<char *>(&buffer->m_data[13]), *eptr;
	for (int i = 0; i < argCount; i++)
	{
		switch (argFormat[i])
		{
		case 'i':
			// signed decimal integer
			args[i].Int = std::strtol(ptr, &eptr, 10);
			break;

		case 'u':
			// unsigned decimal integer
			args[i].Uint = std::strtoul(ptr, &eptr, 10);
			break;

		case 'x':
			// hexadecimal
			args[i].Uint = std::strtoul(ptr, &eptr, 16);
			break;

		case 'f':
			// floating-point
			args[i].Float = std::strtod(ptr, &eptr);
			break;

		case 'c':
			// 4-letter code
			memcpy(&args[i].Char, ptr, 4);
			eptr = ptr + 4;
			break;

		case 'p':
			// buffer is passed to operation as is
			// must always be the last argument, so eptr is irrelevant
			args[i].Ptr = ptr;
			break;
		}
		ptr = eptr + 1;
	}
	return Operation::CreateOperation(retry, tId, devId, opcode, argCount, args);
}

void HostCommunication::SendResponse(OutstandingResponse *resp)
{
	m_inTransmit = resp;
	HAL_UART_Transmit_DMA(m_UARTPort, resp->m_response, resp->m_size);
	resp->m_retryAttempt++;
	resp->m_response[5] = '0' + resp->m_retryAttempt;
	resp->m_nextRetryTick = HAL_GetTick() + RETRY_TIMEOUT;
}

void HostCommunication::ProcessReports()
{
	for (auto it = s_outstandingResponses.begin(); it != s_outstandingResponses.end(); ++it)
	{
		if (m_inTransmit != nullptr)
		{
			return;
		}
		if ((*it)->m_retryAttempt == 0)
		{
			// first retry, send immediately
			SendResponse(*it);
			return;
		}
		else
		{
			if ((int32_t)(HAL_GetTick() - (*it)->m_nextRetryTick) >= 0)
			{
				// time to retry send
				if ((*it)->m_retryAttempt < MAX_RESPONSE_RETRIES)
				{
					SendResponse(*it);
					return;
				}
				else
				{
					// host didn't Ack report. Too bad, drop it anyway
					// Disable interrupts
					uint32_t priMask = __get_PRIMASK();
					__disable_irq();

					s_freeResponses.splice(s_freeResponses.end(), s_outstandingResponses, (*it)->m_iterator);

					// enable interrupts to their previous state
					__set_PRIMASK(priMask);
					// we can't iterate through the list anymore, need to exit and continue on next entry
					return;
				}
			}
		}
	}
}

void HostCommunication::ReportIsAcked(uint32_t tId)
{
	for (auto it = s_outstandingResponses.begin(); it != s_outstandingResponses.end(); ++it)
	{
		if ((*it)->m_transactionId == tId && (*it)->m_retryAttempt < MAX_RESPONSE_RETRIES)
		{
			// Disable interrupts
			uint32_t priMask = __get_PRIMASK();
			__disable_irq();

			s_freeResponses.splice(s_freeResponses.end(), s_outstandingResponses, (*it)->m_iterator);

			// enable interrupts to their previous state
			__set_PRIMASK(priMask);
			return;
		}
	}
}

void HostCommunication::RxHandler(UART_HandleTypeDef *huart)
{
	if (huart == m_UARTPort)
	{
		ReceiveBuffer *buffer = &m_receiveBuffer[m_receiveBufIdx];
		switch (buffer->m_receiveState)
		{
		case ReceiveState::WaitSTX:
			// in this state we wait for first character of host packet to synchronize the receiving

			if (buffer->m_data[buffer->m_receiveIdx] == STX)
			{
				buffer->m_receiveState = ReceiveState::Header;
				buffer->m_receiveIdx = 1;
				HAL_UART_Receive_DMA(m_UARTPort, &buffer->m_data[buffer->m_receiveIdx], PACKET_HEADER_SIZE - 1);
			}
			else
			{
				// drop on the floor all characters but STX, we can't decode them anyway
				// resume listening
				HAL_UART_Receive_IT(m_UARTPort, &buffer->m_data[buffer->m_receiveIdx], 1);
			}
			break;

		case ReceiveState::Header:
			buffer->m_receiveIdx = PACKET_HEADER_SIZE;
			if (buffer->m_data[5] == 'K')
			{
				// Ack packet received
				buffer->m_receiveState = ReceiveState::Received;
				m_receiveBufIdx++;
				if (m_receiveBufIdx >= RCV_BUFFERS)
				{
					m_receiveBufIdx = 0;
				}
				StartListening();
				break;

			}
			std::from_chars(reinterpret_cast<char *>(&buffer->m_data[6]),
							reinterpret_cast<char *>(&buffer->m_data[8]),
							buffer->m_payloadLen, 16);
			buffer->m_receiveState = ReceiveState::Payload;
			HAL_UART_Receive_DMA(m_UARTPort, &buffer->m_data[buffer->m_receiveIdx], buffer->m_payloadLen + 5); // add to payload length CRC and ETX
			break;

		case ReceiveState::Payload:
			buffer->m_receiveIdx += huart->RxXferSize;
			buffer->m_receiveState = ReceiveState::Received;
			m_receiveBufIdx++;
			if (m_receiveBufIdx >= RCV_BUFFERS)
			{
				m_receiveBufIdx = 0;
			}
			StartListening();
			break;

		case ReceiveState::Idle:
		case ReceiveState::Received:
			break;
		}
	}
}

void HostCommunication::TxHandler(UART_HandleTypeDef *huart)
{
	if (m_inTransmit->m_size == ACK_SIZE)
	{
		// Remove immediately, Acks only sent once, no retries
		// This is the only priority context where responses lists are manipulated, we may not disable interrupts for this

		//s_freeResponses.splice(s_freeResponses.end(), s_outstandingResponses, m_inTransmit->m_iterator);
		m_inTransmit->m_retryAttempt = MAX_RESPONSE_RETRIES;
	}
	m_inTransmit = nullptr;
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HostCommunication::GetPort()->RxHandler(huart);
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	HostCommunication::GetPort()->TxHandler(huart);
}

