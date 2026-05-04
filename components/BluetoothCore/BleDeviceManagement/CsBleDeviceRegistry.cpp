#include "CsBleDeviceRegistry.h"
#include <esp_log.h>

using namespace SignatureBridge::BluetoothCore::BleCore;
using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;

CsBleDeviceRegistry& CsBleDeviceRegistry::Instance()
{
    static CsBleDeviceRegistry instance;
    return instance;
}

void CsBleDeviceRegistry::Register(std::string prefix, DeviceFactory factory)
{
    _entries.emplace_back(
        std::move(prefix),
        std::move(factory)
    );
}

std::unique_ptr<CsBleDevice> CsBleDeviceRegistry::TryCreate(const CsBleAdvertisement& advertisement) const
{
    if (advertisement.Name.empty())
    {
        return nullptr;
    }

    for (const auto& entry : _entries)
    {
        if (advertisement.Name.starts_with(entry.Prefix))
        {
            return entry.Factory(advertisement);
        }
    }

    return nullptr;
}

bool CsBleDeviceRegistry::MatchesRegisteredPrefix(const std::string& name) const
{
    if (name.empty())
    {
        return false;
    }

    for (const auto& entry : _entries)
    {
        if (name.starts_with(entry.Prefix))
        {
            return true;
        }
    }

    return false;
}
