/*
 * AppMain.hpp
 *
 *  Created on: 12 авг. 2026 г.
 *      Author: dysgn
 */

#ifndef BOOTMAIN_HPP_
#define BOOTMAIN_HPP_

#include "Board.hpp"

const uint16_t BoardDeviceId		= SWAP('Bd');

const uint16_t GetAppCmd			= SWAP('Ap');
const uint16_t GetAppVersionCmd		= SWAP('VA');
const uint16_t GetBootVersionCmd	= SWAP('VB');
const uint16_t JumpToAppCmd			= SWAP('JA');
const uint16_t GetResetReasonCmd	= SWAP('BR');
const uint16_t EraseAppCmd			= SWAP('Er');
const uint16_t WriteCodeCmd			= SWAP('WX');

extern Board s_board;

class Device;

extern "C" void BootMain();
void AddPeriodicHandler(Device *);

#endif /* BOOTMAIN_HPP_ */
