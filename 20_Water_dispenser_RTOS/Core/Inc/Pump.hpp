/*
 * Pump.hpp
 *
 *  Created on: 17 авг. 2026 г.
 *      Author: dysgn
 */

#ifndef PUMP_HPP_
#define PUMP_HPP_

#include "Device.hpp"
#include "PID.hpp"
#include "stm32f7xx_hal.h"

/**
 * Pump driver class
 */
class Pump : public Device
{
private:
	/// Frequency of PWM driving the pump
	const float DCMotorPWMFrequency = 25.0f;

	/// Tolerance to consider current flow matching target
	const float FlowTolerance = 2.0f;

	/// Time in ms to decide whether flow reached the target
	const uint32_t FlowSetTimeout = 10000;

	/// Prescaler from SysTick clock to perform flow regulation
	const uint32_t FlowPIDPrescaler = 100;

	/// HAL Timer object to control pump PWM
	TIM_HandleTypeDef *m_pwmTimer;
	/// Timer channel
	uint32_t m_pwmChannel;

	/// Deadline for timed activities
	uint32_t m_endTime;

	/// Setpoint for flow regulation
	float m_flowSetpoint;
	/// True if flow regulation is active
	bool m_flowPIDRunning = false;

	/// Target volume for fluid dispenses
	uint32_t m_volumeTarget;

	/// Current tick counter value for regulation prescaler
	uint32_t m_flowPIDPrescalerCnt = 0;

	/// Regulator object
	PID m_regulator;

	/// Period for periodic reports in ms
	uint32_t m_reportPeriod = 0;

	/// Current counter of report period
	uint32_t m_reportCounter = 0;

public:
	/** @brief Constructor
	 * @param id		device Id
	 * @param pwmTimer	HAL Timer object to control pump PWM
	 * @param channel	Timer channel
	 */
	Pump(uint16_t id, TIM_HandleTypeDef *pwmTimer, uint32_t channel) :
			Device(id),
			m_pwmTimer(pwmTimer),
			m_pwmChannel(channel)
	{

	}

	virtual ~Pump()
	{

	}

	/// @brief Initialize pump
	void Initialize() override;

	/** @brief Command dispatcher
	 *  @param op	Operation object
	 */
	void Execute(Operation *op) override;

	/** @brief Override regular busy checks for some operations
	 * @param requestedOp	operation that needs to be performed
	 * @return true if operation is NOT allowed
	 */
	bool IsBusy(Operation *requestedOp) const override;

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

	/** @brief Perform multiple periodic tasks at low interrupt priority
	 */
	void PeriodicHandler() override;
};

#endif /* PUMP_HPP_ */
