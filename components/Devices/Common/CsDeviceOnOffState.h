#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::Common {

enum class CsDeviceOnOffState : int
{
    Unknown = -1,
    Off = 0,
    On = 1
};

inline CsDeviceOnOffState CsDeviceOnOffStateTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsDeviceOnOffState::Off;
        case 1: return CsDeviceOnOffState::On;

        default:
            ESP_LOGW("CsDeviceOnOffState", "Unknown on/off state value: %u", value);
            return CsDeviceOnOffState::Unknown;
    }
}

}
