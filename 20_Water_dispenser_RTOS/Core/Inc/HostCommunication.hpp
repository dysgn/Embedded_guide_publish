/*
 * HostCommunication.hpp
 *
 *  Created on: 12 Aug 2026
 *      Author: dysgn
 */

#ifndef HOSTCOMMUNICATION_HPP_
#define HOSTCOMMUNICATION_HPP_

#include <list>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Routing.h"
#include "FreeRTOS_Sockets.h"
#include "Value32.hpp"
#include "SystemDef.hpp"

/// Maximum number of arguments
#define MAX_ARG_COUNT	4
// Maximum number of returned data items
#define MAX_RET_COUNT	4

class Operation;

struct __attribute__ ((__packed__)) CommandPacket
{
	uint16_t m_transactionId;

	uint16_t m_payloadLength;

	uint16_t m_deviceId;

	uint16_t m_opcode;

	uint16_t m_argCount;

	Value32 m_args[MAX_ARG_COUNT];
};

struct __attribute__ ((__packed__)) ResponsePacket
{
	uint16_t m_transactionId;

	uint8_t m_status;

	uint8_t m_payloadLength;

	uint16_t m_deviceId;

	uint16_t m_opcode;

	uint16_t m_retCount;

	uint32_t m_rets[MAX_RET_COUNT];
};

/** Host communication class (singleton)
 */
class HostCommunication final
{
private:

	static NetworkInterface_t s_Interfaces[1];

	static NetworkEndPoint_t s_EndPoints[1];

	static constexpr uint8_t s_IPAddress[4]      = BOARD_IP;
	static constexpr uint8_t s_NetMask[4]        = { 255, 255, 255, 0 };
	static constexpr uint8_t s_GatewayAddress[4] = { 0, 0, 0, 0 };
	static constexpr uint8_t s_DNSServerAddress[4] = { 8, 8, 8, 8 };
	static constexpr uint8_t s_MACAddress[6]     = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9A };

	static const uint16_t ServerPort = BOARD_PORT;
	/// Singleton instance
	static HostCommunication *s_Instance;

	Socket_t m_listeningSocket = FREERTOS_INVALID_SOCKET;
    Socket_t m_ConnectedSocket = FREERTOS_INVALID_SOCKET;

    freertos_sockaddr m_hostAddress;

	/** @brief Constructor
	 * Constructor is private to prevent creation of alternative objects
	 * @param port		UART port
	 */
	HostCommunication();

public:

	/** @brief Public interface for creating HostCommunication singleton
	 * @param port	UART port
	 */
	static void CreateHostPort();

	/// Get pointer to HostCommunication singleton object
	static HostCommunication *GetPort();

	void Initialize();

	static void ReceiveThread();

	static void SendThread();
};

#endif /* HOSTCOMMUNICATION_HPP_ */
