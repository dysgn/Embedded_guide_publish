/*
 * BootInfo.h
 *
 *  Created on: 8 окт. 2026 г.
 *      Author: dysgn
 */

#ifndef BOOTINFO_H_
#define BOOTINFO_H_

#define MAGIC_NUMBER	0xDA3045C1

/// Reasons to execute bootloader
enum ResetReason
{
	/// Power On or hardware reset
	POR_Hdw,

	/// Command from host
	Command,

	/// No valid application
	NoApp,

	/// NMI exception
	NMI,

	/// HardFault exception
	HardFault,

	/// MemManage exception
	MemManage,

	/// BusFault exception
	BusFault,

	/// UsageFault exception
	UsageFault
};

/// Information to pass to bootloader
struct BootInfo
{
	/// Specific number to identify valid information
	uint32_t m_magicNumber;

	/// true if prevent jump to application
	uint32_t m_stayInBoot;

	/// Reason to execute bootloader
	enum ResetReason m_reason;

	/// address of the instruction causing fault. If not fault, nullptr
	void *m_faultAddress;
};

#endif /* BOOTINFO_H_ */
