#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::Common {

enum class CsDeviceAmPm : int
{
    Unknown = -1,
    Am = 0,
    Pm = 1
};

inline CsDeviceAmPm CsDeviceAmPmTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsDeviceAmPm::Am;
        case 1: return CsDeviceAmPm::Pm;

        default:
            ESP_LOGW("CsDeviceAmPm", "Unknown AM/PM value: %u", value);
            return CsDeviceAmPm::Unknown;
    }
}

}
