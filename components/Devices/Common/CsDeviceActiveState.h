#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::Common {

enum class CsDeviceActiveState : uint8_t
{
    Inactive = 0U,
    Active = 1U
};

inline CsDeviceActiveState CsDeviceActiveStateTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsDeviceActiveState::Inactive;
        case 1: return CsDeviceActiveState::Active;

        default:
            ESP_LOGW("CsDeviceActiveState", "Unknown active state value: %u", value);
            return CsDeviceActiveState::Inactive;
    }
}

}
