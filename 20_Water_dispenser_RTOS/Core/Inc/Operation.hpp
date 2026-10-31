/*
 * Operation.hpp
 *
 *  Created on: 13 авг. 2026 г.
 *      Author: dysgn
 */

#ifndef OPERATION_HPP_
#define OPERATION_HPP_

#include "FreeRTOS.h"
#include <list>
#include <stdint.h>
#include "semphr.h"
#include "Value32.hpp"

#define MAX_OPERATIONS	32
#define MAX_ARGUMENTS	3
#define MAX_RET_DATA	3

class Device;

/*
 * Operation passport
 */
class Operation final
{
public:
	/// Error code enumeration
	enum ErrorStatus
	{
		/// Operation completed successfully
		Success = 0,

		/// Operation is pending
		Pending = 1,

		/// Operation was stopped by command
		Stopped = 2,

		/// Operation not performed: invalid device
		InvalidDevice = 0x81,

		/// Operation not performed: invalid command
		InvalidCommand = 0x82,

		/// Operation not performed: incorrect number of arguments
		InvalidArgumentCount = 0x82,

		/// Operation not performed: invalid argument
		InvalidArgument = 0x84,

		/// Operation not performed: device is performing another operation
		DeviceIsBusy = 0x85,

		/// Timeout during operation execution
		Timeout = 0x86,

		/// Pump specific: can't maintain the specified flow rate
		FlowRateError = 0x87,
	};

private:
	/// transaction Id
	uint16_t m_transId;

	/// device Id
	uint16_t m_deviceId;

	/// operation Id
	uint16_t m_opcode;

	/// actual number of arguments
	int m_argCount;

	/// arguments
	Value32 m_arguments[MAX_ARGUMENTS];

	/// number of response data items
	int m_retCount;

	/// response data
	Value32 m_retData[MAX_RET_DATA];

	/// Operation status
	ErrorStatus m_status;

	/// Retry count for response message
	uint8_t m_retRetries;

	/// saved iterator
	std::list<Operation *>::iterator m_listIterator;

	/// Device object
	Device *m_device;

	/// statically allocated operation passports
	static Operation s_OperationPool[MAX_OPERATIONS];

	/// operations available for usage
	static std::list<Operation *> s_freeOps;

	/// operations just created from command, but not yet put to execution
	static std::list<Operation *> s_parsedOps;

	/// currently executing operations
	static std::list<Operation *> s_activeOps;

	/// completed operations
	static std::list<Operation *> s_completedOps;

	/// Mutex for list operations
	static SemaphoreHandle_t s_listMutex;

	/// Start of transaction Id range for async notifications
	static const uint32_t AsyncIdStart = 0xF000;

	/// End of transaction Id range for async notifications
	static const uint32_t AsyncIdEnd = 0xFFFF;

	/// Next transaction Id range for async notifications
	static uint32_t s_nextAsyncId;

public:
	/** @brief Constructor
	 */
	Operation() {}

	virtual ~Operation() {}

	/** @brief Init all data structures related to operation handling
	 */
	static void InitOperations();

	/** @brief Get Operation object filled with information from parsed command packet, placed in s_parsedOps list
	 * @param transactionId		transaction Id
	 * @param deviceId			device Id
	 * @param opId				operation Id (opcode)
	 * @param argCount			actual number of arguments
	 * @param args				array of arguments
	 * @return Operation object
	 */
	static Operation *CreateOperation(uint16_t transactionId, uint16_t deviceId, uint16_t opId,
					int argCount = 0, Value32 args[] = nullptr);

	/** @brief Get Operation object filled with async notification data, placed in s_completedOps list
	 * @param deviceId			device Id
	 * @param opId				operation Id (opcode)
	 * @param status			error code
	 * @param argCount			actual number of data items
	 * @param args				array of data items
	 * @return Operation object
	 */
	static Operation *CreateAsyncMessage(uint16_t deviceId, uint16_t opId, ErrorStatus status,
					int argCount = 0, Value32 args[] = nullptr);

	/** @brief Transaction Id getter
	 */
	uint16_t GetTransactionId() const { return m_transId; }

	/** @brief Device Id getter
	 */
	uint16_t GetDeviceId() const { return m_deviceId; }

	/** @brief Operation Id getter
	 */
	uint16_t GetOpId() const { return m_opcode; }

	/** @brief Argument count getter
	 */
	int GetArgCount() const { return m_argCount; }

	/** @brief Get argument as Value32
	 *  @param idx		argument index
	 */
	Value32 GetArgument(int idx) const { return m_arguments[idx]; }

	/** @brief Get argument as signed integer
	 *  @param idx		argument index
	 */
	int32_t GetIntArgument(int idx) const { return m_arguments[idx].Int; }

	/** @brief Get argument as unsigned integer
	 *  @param idx		argument index
	 */
	uint32_t GetUintArgument(int idx) const { return m_arguments[idx].Uint; }

	/** @brief Get argument as 32-bit float
	 *  @param idx		argument index
	 */
	float GetFloatArgument(int idx) const { return m_arguments[idx].Float; }

	/** @brief Get argument as pointer to character string
	 *  @param idx		argument index
	 */
	const char *GetCharArgument(int idx) const { return m_arguments[idx].Char; }

	/** @brief Get pointer to the array of arguments
	 */
	const char *GetArgArray() const { return reinterpret_cast<const char *>(&m_arguments[0]); }

	/** @brief Get status
	 */
	ErrorStatus GetStatus() const { return m_status; }

	/** @brief Get number of returned data items
	 */
	int GetRetCount() const { return m_retCount; }

	/** @brief Get return data item
	 * @param idx		item index
	 */
	Value32 GetRetDataItem(int idx) const { return m_retData[idx]; }

	/** @brief Get pointer to response data
	 */
	const void *GetRetArray() const { return reinterpret_cast<const void *>(&m_retData[0]); }

	/** @brief Execute the operation
	 */
	void Execute();

	/** @brief Set the operation as complete
	 * @param errorCode		final operation completion status
	 * @param retCount		number of returned data items
	 * @param retData		array of data items
	 */
	void Complete(ErrorStatus errorCode, int retCount = 0, Value32 retData[] = nullptr);

	/** @brief Complete the operation that didn't start
	 * @param errorCode		error (reason to not start the operation)
	 */
	void CompleteBeforeExecution(ErrorStatus errorCode);

	/// Finally release the Operation object
	void Discard();

	/// Clear the list of completed operations (host disconnected, so skip reports)
	static void ForgetReports();

};

#endif /* OPERATION_HPP_ */
