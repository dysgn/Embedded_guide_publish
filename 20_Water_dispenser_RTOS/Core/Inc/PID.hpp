/*
 * PID.hpp
 *
 *  Created on: 20 авг. 2026 г.
 *      Author: dysgn
 */

#ifndef PID_HPP_
#define PID_HPP_

/*
 *
 */
class PID final
{
private:
    float m_kp, m_ki, m_kd;
    float m_minOutput, m_maxOutput;
    float m_integral = 0.0f;
	float m_previousError = 0.0f;

public:
    PID( ) { }

    void SetRange(float min_out, float max_out)
    {
		m_minOutput = min_out;
		m_maxOutput = max_out;
    }

    void SetGains(float p, float i, float d)
    {
		m_kp = p;
		m_ki = i;
		m_kd = d;
    }

	float calculate(float setpoint, float pv);
};

#endif /* PID_HPP_ */
