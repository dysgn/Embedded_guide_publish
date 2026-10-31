/*
 * Board.hpp
 *
 *  Created on: 8 окт. 2026 г.
 *      Author: dysgn
 */

#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <Device.hpp>

class Board: public Device
{
private:
	enum DelayPurpose
	{
		prpNone,

		prpJumpToApp,

		prpWriteFlash,

		prpEraseFlash
	};

	DelayPurpose m_actionAfterDelay;

	/// tick counter for various delays
	uint32_t m_counter;

	/** @brief Erase application Flash
	 * Erases all flash above bootloader
	 */
	void EraseAppFlash();

	/** @breif Write data to flash
	 * @param flashAddress		address to write to
	 * @param bytecount			number of bytes to write
	 * @param data				buffer with data to write
	 */
	void WriteApplicationFlash(uint32_t flashAddress, uint32_t byteCount, const char *data);

public:
	/** @brief Constructor
	 * @param id		device Id
	 */
	Board(uint16_t id) : Device(id)
	{
	}

	virtual ~Board() {}

	/** @brief Initialization
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

	/** @brief Counts delay before jump to bootloader
	 */
	void PeriodicHandler() override;

	void JumpToApp();

	bool IsAppPresent();
};

#endif /* BOARD_HPP_ */
