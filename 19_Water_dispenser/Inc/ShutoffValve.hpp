/*
 * ShutoffValve.h
 *
 *  Created on: 17 авг. 2026 г.
 *      Author: dysgn
 */

#ifndef SHUTOFFVALVE_HPP_
#define SHUTOFFVALVE_HPP_

#include "Device.hpp"
#include "Operation.hpp"
#include "stm32f7xx_hal.h"

/** ShutoffValve class
 *
 */
class ShutoffValve: public Device
{
	/// GPIO port of valve control pin
	GPIO_TypeDef *m_gpioPort;

	/// Bit index of valve control pin
	uint16_t m_gpioPin;

	/// Current state of the valve
	bool m_isOpen = false;

public:
	/** @brief Constructor
	 * @param id		device Id
	 * @param gpioPort	GPIO port of valve control pin
	 * @param gpioPin	Bit index of valve control pin
	 */
	ShutoffValve(uint16_t id, GPIO_TypeDef * gpioPort, uint16_t gpioPin) :
					Device(id),
					m_gpioPort(gpioPort),
					m_gpioPin(gpioPin)
	{
	}

	virtual ~ShutoffValve(){}

	/** @brief Valve initialization
	 */
	void Initialize() override;

	/** @brief Command dispatcher
	 *  @param op	Operation object
	 */
	void Execute(Operation *op) override;

	/** @brief Get argument format string
	 * @param opcode	operation Id
	 * @return format of arguments
	 */
	const char *ArgumentFormat(uint16_t opcode) const override;

	/** @brief Get format of response data
	 * @param opcode	operation Id
	 * @return format of response data
	 */
	const char *ResponseFormat(uint16_t opcode) const override;

	/** @brief Open the valve
	 */
	void Open();

	/** @brief Close the valve
	 */
	void Close();

	/** @brief Get the valve state
	 */
	bool IsOpen() const { return m_isOpen; }
};

#endif /* SHUTOFFVALVE_HPP_ */
