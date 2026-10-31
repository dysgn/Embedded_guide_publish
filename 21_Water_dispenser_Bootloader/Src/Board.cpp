/*
 * Board.cpp
 *
 *  Created on: 8 окт. 2026 г.
 *      Author: dysgn
 */
#include <cstring>
#include "stm32f7xx_hal.h"
#include "Board.hpp"
#include "BootInfo.h"
#include "Operation.hpp"
#include "BootMain.hpp"

__attribute__ ((section(".boot_version"))) char VersionString[] = "Bootloader v1.0.0";
__attribute__ ((section(".shared_boot_info"))) BootInfo bootInfo;

extern "C" const uint32_t AppAreaStart = 0x08010000;
const uint32_t AppAreaEnd = 0x08100000;

const char *applicationVersionAddr = (const char *)0x08010200;
const char blankAppVer[] = "<blank>";

void Board::Initialize()
{
	AddPeriodicHandler(this);
}

void Board::Execute(Operation *op)
{
	switch (op->GetOpId())
	{
	case GetAppCmd:
		op->Complete(Operation::ErrorStatus::Success, 1, (Value32[]){{ .Int = 1 }});
		break;

	case GetAppVersionCmd:
		if (*(uint32_t *)applicationVersionAddr == 0xFFFFFFFF)
		{
			op->Complete(Operation::ErrorStatus::Success, strlen(blankAppVer) + 1, blankAppVer);
		}
		else
		{
			op->Complete(Operation::ErrorStatus::Success, strlen(applicationVersionAddr) + 1, applicationVersionAddr);
		}
		break;

	case GetBootVersionCmd:
		op->Complete(Operation::ErrorStatus::Success, strlen(VersionString) + 1, VersionString);
		break;

	case JumpToAppCmd:
		if (IsAppPresent())
		{
			op->Complete(Operation::ErrorStatus::Success);
			// give time for command response to be sent out
			m_actionAfterDelay = DelayPurpose::prpJumpToApp;
			m_counter = 0;
		}
		else
		{
			op->Complete(Operation::ErrorStatus::NoAppPresent);
		}
		break;

	case GetResetReasonCmd:
		if (bootInfo.m_magicNumber != MAGIC_NUMBER)
		{
			// Neither intended jump to bootloader, nor exception handling, just hardware/power-on reset
			op->Complete(Operation::ErrorStatus::Success, 2,
					(Value32[]){ { .Uint = (uint32_t)ResetReason::POR_Hdw }, { .Uint = 0 } });
		}
		else
		{
			op->Complete(Operation::ErrorStatus::Success, 2,
					(Value32[]){ { .Uint = (uint32_t)bootInfo.m_reason }, { .Uint = (uint32_t)bootInfo.m_faultAddress } });
		}
		break;

	case EraseAppCmd:
		m_operation = op;
		m_counter = 0;
		m_actionAfterDelay = DelayPurpose::prpEraseFlash;
		break;

	case WriteCodeCmd:
		if (op->GetUintArgument(0) < AppAreaStart || op->GetUintArgument(0) >= AppAreaEnd)
		{
			op->Complete(Operation::ErrorStatus::InvalidArgument);
			return;
		}
		if (op->GetUintArgument(1) > 240)
		{
			op->Complete(Operation::ErrorStatus::InvalidArgument);
			return;
		}
		m_operation = op;
		m_counter = 0;
		m_actionAfterDelay = DelayPurpose::prpWriteFlash;
		break;

	default:
		op->Complete(Operation::ErrorStatus::InvalidCommand);
		break;
	}
}

const char* Board::ArgumentFormat(uint16_t opcode) const
{
	switch (opcode)
	{
	case GetAppCmd:
	case GetAppVersionCmd:
	case GetBootVersionCmd:
	case JumpToAppCmd:
	case GetResetReasonCmd:
	case EraseAppCmd:
		return "";

	case WriteCodeCmd:
		return "uup";

	default:
		return nullptr;
	}
}

const char* Board::ResponseFormat(uint16_t opcode) const
{
	switch (opcode)
	{
	case GetAppCmd:
		return "i";

	case GetAppVersionCmd:
	case GetBootVersionCmd:
		return "p";

	case JumpToAppCmd:
		return "";

	case GetResetReasonCmd:
		return "uu";

	case EraseAppCmd:
	case WriteCodeCmd:
		return "";

	default:
		return nullptr;
	}
}

void Board::PeriodicHandler()
{
	if (m_actionAfterDelay != DelayPurpose::prpNone)
	{
		m_counter++;
		switch (m_actionAfterDelay)
		{
		case DelayPurpose::prpJumpToApp:
			if (m_counter >= 100)
			{
				m_actionAfterDelay = DelayPurpose::prpNone;
				bootInfo.m_magicNumber = MAGIC_NUMBER;
				bootInfo.m_reason = ResetReason::POR_Hdw;
				bootInfo.m_stayInBoot = false;
				bootInfo.m_faultAddress = nullptr;
				NVIC_SystemReset();
			}
			break;

		case DelayPurpose::prpWriteFlash:
			if (m_counter >= 3)
			{
				m_actionAfterDelay = DelayPurpose::prpNone;
				WriteApplicationFlash(m_operation->GetUintArgument(0), m_operation->GetUintArgument(1), m_operation->GetPtrArgument(2));
			}
			break;

		case DelayPurpose::prpEraseFlash:
			if (m_counter >= 3)
			{
				m_actionAfterDelay = DelayPurpose::prpNone;
				EraseAppFlash();
			}
			break;
		}
	}
}

void Board::JumpToApp()
{
    typedef void (*pFunction)(void);

    // prevent any interrupts
    __disable_irq();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    // clear all interrupts in NVIC
    for (uint8_t i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFFUL; // disable IRQs
        NVIC->ICPR[i] = 0xFFFFFFFFUL; // clear IRQ pending flags
    }

    volatile uint32_t* flashVectors = reinterpret_cast<volatile uint32_t*>(AppAreaStart);

    uint32_t appMsp          = flashVectors[0]; // initial SP
    uint32_t appResetHandler = flashVectors[1]; // Reset_Handler of the app

    // make SP aligned by 8-byte boundary
    appMsp &= ~0x7U;

    // set new SP value
    __set_MSP(appMsp);

    pFunction JumpFunc = reinterpret_cast<pFunction>(appResetHandler);
    JumpFunc();
}

bool Board::IsAppPresent()
{
	uint32_t *appResetAddr = (uint32_t *)AppAreaStart;

	if (*appResetAddr == 0xFFFFFFFF)
	{
		return false;
	}
	return true;
}

void Board::EraseAppFlash()
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError = 0;

	// Unlock Flash operations
	HAL_FLASH_Unlock();

	// Set up erase parameters
	EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Sector       = FLASH_SECTOR_2;
	EraseInitStruct.NbSectors    = 6;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

	// Disable interrupts. The system becomes unresponsive while erasing proceeds
	__disable_irq();
	HAL_StatusTypeDef erase_status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	__enable_irq();

	// Clear cash to synchronize with new state of memory
	SCB_InvalidateDCache();
	SCB_InvalidateICache();

	// Lock Flash back
	HAL_FLASH_Lock();

	if (m_operation != nullptr)
	{
		if (erase_status == HAL_OK)
		{
			m_operation->Complete(Operation::ErrorStatus::Success);
		}
		else
		{
			m_operation->Complete(Operation::ErrorStatus::FlashEraseError);
		}
		m_operation = nullptr;
	}
}

void Board::WriteApplicationFlash(uint32_t flashAddress, uint32_t byteCount, const char *data)
{
    uint64_t tmpBuffer;
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t bytesWritten = 0;

    // Unlock Flash
    HAL_FLASH_Unlock();

    // Clear possible stale error flags
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_ERSERR);

    // Disable interrupts while writing
    __disable_irq();
    // disable cache
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
    {
        SCB->CCR &= ~(uint32_t)SCB_CCR_DC_Msk;

        __DSB();
        __ISB();
    }
    __HAL_FLASH_PREFETCH_BUFFER_DISABLE();

    // All Flash writes in
    while (bytesWritten < byteCount)
    {
        uint32_t remaining = byteCount - bytesWritten;

        if (remaining >= 8)
        {
            memcpy(&tmpBuffer, &data[bytesWritten], 8);
        }
        else
        {
            tmpBuffer = 0xFFFFFFFFFFFFFFFFull;
            memcpy(&tmpBuffer, &data[bytesWritten], remaining);
        }

//        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flashAddress, tmpBuffer);
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddress,     (uint32_t)(tmpBuffer & 0xFFFFFFFF));
        if(status == HAL_OK)
        {
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddress + 4, (uint32_t)(tmpBuffer >> 32));
        }
        if (status != HAL_OK)
        {
            break;
        }
        flashAddress += 8;
        bytesWritten += 8;
    }

    SCB_EnableICache();
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    __enable_irq();

    // Lock Flash back
    HAL_FLASH_Lock();

	if (m_operation != nullptr)
	{
		if (status == HAL_OK)
		{
			m_operation->Complete(Operation::ErrorStatus::Success);
		}
		else
		{
			m_operation->Complete(Operation::ErrorStatus::FlashWriteError);
		}
		m_operation = nullptr;
	}
}
