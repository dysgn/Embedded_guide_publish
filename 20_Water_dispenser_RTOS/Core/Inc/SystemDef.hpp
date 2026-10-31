/*
 * SystemDef.hpp
 *
 *  Created on: 26 сент. 2026 г.
 *      Author: dysgn
 */

#ifndef INC_SYSTEMDEF_HPP_
#define INC_SYSTEMDEF_HPP_

#include <stdint.h>
#include "ShutoffValve.hpp"
#include "Pump.hpp"
#include "FlowMeter.hpp"

#define BOARD_IP		{ 192, 68, 1, 1 }
#define BOARD_PORT		5000

const uint16_t ShutoffValveId	= SWAP('SV');
const uint16_t PumpId			= SWAP('Pm');
const uint16_t FlowMeterId		= SWAP('FM');

const uint16_t OpenCmd			= SWAP('Op');
const uint16_t CloseCmd			= SWAP('Cl');
const uint16_t QueryStateCmd	= SWAP('QS');

const uint16_t SetPowerCmd		= SWAP('Pc');
const uint16_t SetTimedPowerCmd	= SWAP('Pt');
const uint16_t SetFlowRateCmd	= SWAP('FR');
const uint16_t DispenseVolumeCmd = SWAP('Vm');
const uint16_t StopCmd			= SWAP('St');
const uint16_t QueryPowerCmd	= SWAP('QP');

const uint16_t ReadFlowCmd		= SWAP('RF');
const uint16_t ResetVolumeCmd	= SWAP('RV');

const uint16_t ReportEnableCmd	= SWAP('RE');
const uint16_t ReportDisableCmd	= SWAP('RD');

const uint16_t PeriodicReportMsg = SWAP('PR');

extern ShutoffValve s_shutoffValve;
extern Pump s_pump;
extern FlowMeter s_flowMeter;

#endif /* INC_SYSTEMDEF_HPP_ */
