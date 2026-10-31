/*
 * Device.cpp
 *
 *  Created on: 14 авг. 2026 г.
 *      Author: dysgn
 */

#include "Device.hpp"
#include "Operation.hpp"

Device::Device(uint16_t id)
{
	m_id = id;
	GetDeviceMap().emplace(std::make_pair(m_id, this));
}

std::map<uint16_t, Device *>& Device::GetDeviceMap()
{
	static std::map<uint16_t, Device *> s_deviceMap;
	return s_deviceMap;
}

void Device::InitAll()
{
	for (auto it = GetDeviceMap().begin(); it != GetDeviceMap().end(); ++it)
	{
		it->second->Initialize();
	}
}

bool Device::IsBusy(Operation *requestedOp) const
{
	// Default implementation doesn't care of specific operations,
	// but devices may allow some operations to happen in parallel
	(void)requestedOp;
	return (m_operation != nullptr);
}

Device* Device::FindDevice(uint16_t deviceId)
{
	auto it = GetDeviceMap().find(deviceId);
	if (it != GetDeviceMap().end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}
