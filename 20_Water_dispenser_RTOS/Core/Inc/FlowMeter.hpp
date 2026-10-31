/*
 * FlowMeter.hpp
 *
 *  Created on: 19 авг. 2026 г.
 *      Author: dysgn
 */

#ifndef FLOWMETER_HPP_
#define FLOWMETER_HPP_

#include "Device.hpp"
#include "stm32f7xx_hal.h"

/**
 * FlowMeter device driver class
 */
class FlowMeter final : public Device
{
private:
	/// HAL Timer object for interval measuring
	TIM_HandleTypeDef *m_counter;
	/// Timer channel
	uint32_t m_channel;

	/// Measured value below which the flow is considered 0
	const float MinDetectableFlow = 0.1f;

	/// Conversion factor of pulses to passed volume pulse/mL
	const float PulseToVolume = 5.88f * 2;

	/// Time interval between signal edges in timer ticks
	uint32_t m_flowTicks = 0;

	/// Flow value in mL/s as result of conversion of last measurement
	float m_flow_mL_s;

	/// Period of async reports sent to host. If 0, periodic reports are disabled
	uint32_t m_reportPeriod = 0;

	/// Time counter to next report
	uint32_t m_reportCounter = 0;

	/// Cumulative volume pulses since last reset
	uint32_t m_volumePulses = 0;

public:
	/** @brief Constructor
	 * @param id		devcie Id
	 * @param counter	HAL Timer object for interval measuring
	 * @param channel	Timer channel
	 */
	FlowMeter(uint16_t id, TIM_HandleTypeDef *counter, uint32_t channel) :
					Device(id),
					m_counter(counter),
					m_channel(channel)
	{
	}

	virtual ~FlowMeter()
	{
	}

	/** @brief Device initialization
	 */
	virtual void Initialize();

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

	/** @brief Perform periodic reporting if enabled
	 */
	void PeriodicHandler() override;

	/** @brief Get last flow measurement
	 */
	float GetFlow() const { return m_flow_mL_s; }

	/** @brief Reset cumulative volume counter
	 */
	void ResetVolume() { m_volumePulses = 0; }

	/** @brief Get cumulative volume since last reset
	 */
	float GetVolume() const { return m_volumePulses / PulseToVolume; }

	/** @brief Class member function performing ISR task for HAL_TIM_IC_CaptureCallback
	 * Captures the value to  be used in flow rate calculations and increments cumulative volume counter
	 */
	void PulseCallback();

	/** @brief Class member function performing ISR task for HAL_TIM_PeriodElapsedCallback
	 * If invoked, it means that intervals become too long, i.e. flow to low, so it may be considered 0
	 */
	void ThresholdCallback();
};

#endif /* FLOWMETER_HPP_ */
