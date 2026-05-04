#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::Common {

enum class CsDeviceAutoReserveMode : uint8_t
{
    Manual = 0U,
    Automatic = 1U
};

inline CsDeviceAutoReserveMode CsDeviceAutoReserveModeTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsDeviceAutoReserveMode::Manual;
        case 1: return CsDeviceAutoReserveMode::Automatic;

        default:
            ESP_LOGW("CsDeviceAutoReserveMode", "Unknown auto reserve mode value: %u", value);
            return CsDeviceAutoReserveMode::Manual;
    }
}

}
