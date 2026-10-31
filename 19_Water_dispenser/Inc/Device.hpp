/*
 * Device.hpp
 *
 *  Created on: 14 авг. 2026 г.
 *      Author: dysgn
 */

#ifndef DEVICE_HPP_
#define DEVICE_HPP_

#include <map>
#include <cstdint>

class Operation;

// Swap endianness for two-character constant
#define SWAP(x)		((x << 8) | (x >> 8))

/*
 *
 */
class Device
{
private:
	/// Device Id
	uint16_t m_id;

	/// Map of all defined devices
	static std::map<uint16_t, Device *> s_deviceMap;

	/// Accessor function, used internally
	static std::map<uint16_t, Device *>& GetDeviceMap();

protected:
	/// Current operation
	Operation *m_operation = nullptr;

public:
	/** @brief Constructor
	 *  @param id		device Id
	 */
	Device(uint16_t id);

	/// @brief Destructor
	virtual ~Device() {}

	/** @brief Get device Id
	 * @return device Id
	 */
	uint16_t GetId() const { return m_id; }

	/** @brief Default implementation of Initialize method
	 */
	virtual void Initialize()
	{

	}

	/** @brief Execute an operation
	 *  @param op	Operation object
	 */
	virtual void Execute(Operation *op) = 0;

	/** @brief Check if the device is busy to perform requested operation
	 * @param requestedOp	operation checked for permission to execute
	 * @return true if device can NOT execute requested operation
	 */
	virtual bool IsBusy(Operation *requestedOp) const;

	/** @brief Get argument format string
	 * @param opcode	operation Id
	 * @return format of arguments
	 */
	virtual const char *ArgumentFormat(uint16_t opcode) const = 0;

	/** @brief Get format of response data
	 * @param opcode	operation Id
	 * @return format of response data
	 */
	virtual const char *ResponseFormat(uint16_t opcode) const = 0;

	/** @brief Function called at low interrupt priority with SysTick period
	 * By default does nothing
	 */
	virtual void PeriodicHandler() {};

	/** @brief Find device object by Id
	 */
	static Device *FindDevice(uint16_t deviceId);

	/** @brief Call Initialize() for all defined devices
	 */
	static void InitAll();
};

#endif /* DEVICE_HPP_ */
