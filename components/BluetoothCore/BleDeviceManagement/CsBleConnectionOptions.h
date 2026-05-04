#pragma once
#include "CsBleUtilities.h"
#include <optional>
#include <string>

namespace SignatureBridge::BluetoothCore::BleDeviceManagement {

using namespace SignatureBridge::BluetoothCore::BleUtilities;

struct CsBleConnectionOptions
{
public:
    std::string AuthToken;
    std::optional<std::string> TargetMacAddress;

public:
    bool HasAuthToken() const
    {
        return !AuthToken.empty();
    }

    bool HasTargetMacAddress() const
    {
        return TargetMacAddress.has_value() && CsBleUtilities::Instance().IsMacAddressValid(*TargetMacAddress);
    }
};

}
