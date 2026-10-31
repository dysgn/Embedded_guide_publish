/*
 * HostCommunication.hpp
 *
 *  Created on: 12 Aug 2026
 *      Author: dysgn
 */

#ifndef HOSTCOMMUNICATION_HPP_
#define HOSTCOMMUNICATION_HPP_

#include <list>
#include "stm32f7xx_hal.h"

const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;

/// Packet header size
#define PACKET_HEADER_SIZE			8
/// Ack size is same as packet header
#define ACK_SIZE					PACKET_HEADER_SIZE
/// Packet size minus payload size
#define PACKET_FRAMING_OVERHEAD		13
/// Mandatory part of payload
#define MINIMUM_PAYLOAD_SIZE		4
/// Maximum length of command packet: max payload size 255 + framing
#define MAX_COMMAND_PACKET_SIZE		268
/// Number of allocated receive buffers
#define RCV_BUFFERS					4
/// Maximum size of response packet: max payload size 255 + framing
#define MAX_RESP_PACKET_SIZE		270
/// Size of buffer for outstanding responses
#define MAX_OUTSTANDING_RESPONSES	16
/// Maximum number of attempts to send response
#define MAX_RESPONSE_RETRIES		3
/// Retry timeout in ms
#define RETRY_TIMEOUT				100

class Operation;

/** Fully formatted response to finished Operation
 */
struct OutstandingResponse
{
	/// current retry attempt, 0 is the first try
	uint32_t m_retryAttempt;

	/// timer tick when to perform retry if not Ack'd
	uint32_t m_nextRetryTick;

	/// transaction Id of the operation
	uint32_t m_transactionId;

	/// full size of the packet to send
	uint32_t m_size;	// identify type by size: if it is 8, it's Ack

	/// formatted response packet
	uint8_t m_response[MAX_RESP_PACKET_SIZE];

	/// iterator to maintain list position
	std::list<OutstandingResponse *>::iterator m_iterator;
};

/** Host communication class (singleton)
 */
class HostCommunication final
{
private:

	/// UART port
	UART_HandleTypeDef *m_UARTPort;

	/// State of the receive
	enum ReceiveState
	{
		/// Not waiting for anything
		Idle,

		/// Waiting for start of the packet from host
		WaitSTX,

		/// Receiving header portion of the host packet
		Header,

		/// Receiving the rest of the host packet
		Payload,

		/// Packet is received in full
		Received
	};

	/// Host packet data structure
	struct ReceiveBuffer
	{
		/// Packet state
		ReceiveState m_receiveState = ReceiveState::Idle;

		/// Actual packet data
		uint8_t m_data[MAX_COMMAND_PACKET_SIZE];

		/// Byte counter of packet being received
		uint32_t m_receiveIdx = 0;

		/// Length of the payload
		uint32_t m_payloadLen;
	};

	/// Receive buffers
	ReceiveBuffer m_receiveBuffer[RCV_BUFFERS];

	/// Index of buffer to receive packet into
	uint32_t m_receiveBufIdx = 0;

	/// Index of buffer where received, but not yet handled packet resides
	uint32_t m_processingBufIdx = 0;

	/// Pointer to current response in the process of transmit
	OutstandingResponse *m_inTransmit = nullptr;

	/// Singleton instance
	static HostCommunication *s_Instance;

	/// Pool of response buffers
	static OutstandingResponse s_responseBufferPool[MAX_OUTSTANDING_RESPONSES];

	/// List of available response objects
	static std::list<OutstandingResponse *> s_freeResponses;

	/// List of response objects currently in use
	static std::list<OutstandingResponse *> s_outstandingResponses;

	/// @brief Convert ASCII character representing hexadecimal digit to binary
	int ParseHexDigit(uint8_t digit)
	{
		if (digit >= '0' && digit <= '9')
		{
			return digit - '0';
		}
		else // no checks, assume packet correctly formed;
		{
			return digit - 'A';
		}
	}

	/** @brief Constructor
	 * Constructor is private to prevent creation of alternative objects
	 * @param port		UART port
	 */
	HostCommunication(UART_HandleTypeDef *port);

	/** @brief Start transmission of the response packet
	 * @param resp	response object
	 */
	void SendResponse(OutstandingResponse *resp);

	friend class Operation;

public:

	/** @brief Public interface for creating HostCommunication singleton
	 * @param port	UART port
	 */
	static void CreateHostPort(UART_HandleTypeDef *port);

	/// Returns true if there's received packet ready to be processed
	bool ReadyToProcess() const { return m_receiveBuffer[m_processingBufIdx].m_receiveState == ReceiveState::Received; }

	/// Finish processing of a received packet
	void Processed()
	{
		m_receiveBuffer[m_processingBufIdx].m_receiveState = ReceiveState::Idle;
		m_processingBufIdx++;
		if (m_processingBufIdx >= RCV_BUFFERS)
		{
			m_processingBufIdx = 0;
		}
	}

	/// Destructor
	virtual ~HostCommunication() {}

	/// Get pointer to HostCommunication singleton object
	static HostCommunication *GetPort();

	/// @brief Start waiting for incoming packet
	void StartListening();

	/** Verify the integrity of received packet, and get its basic parameters
	 * @param [out]	tId		transaction Id
	 * @param [out] ack		true if the received packet is host's Ack to a response
	 * @param [out] retry	retry counter from incoming packet
	 */
	bool VerifyPacket(uint32_t &tId, bool &ack, uint8_t &retry);

	/** @brief Create response object for incoming packet's Ack and place it to a queue
	 * @param tId		transaction Id
	 * @param onRetry	retry count of the incoming packet
	 */
	void SendAck(uint32_t tId, uint8_t onRetry);

	/** @brief Parse incoming packet and create operation object
	 *
	 * The incoming packet is pointed to by m_processingBufIdx and must have ReceiveState::Received state
	 */
	Operation *ParseCommandPacket();

	/** @brief Idle state handler of outgoing response packets
	 *
	 * Called from the main loop, looks up the list of completed operations,
	 */
	void ProcessReports();

	/** @brief Confirm response object can be released
	 * @param tId	transaction Id of the response
	 */
	void ReportIsAcked(uint32_t tId);

	/// @brief handler of Rx interrupts, called from ISR
	void RxHandler(UART_HandleTypeDef *huart);

	/// @brief Handler of Tx interrupts, called from ISR
	void TxHandler(UART_HandleTypeDef *huart);
};

#endif /* HOSTCOMMUNICATION_HPP_ */
