#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::Common {

enum class CsDeviceSaltSensorStatus : int
{
    Unknown = -1,
    SaltOkay = 0,
    SaltLow = 1
};

inline CsDeviceSaltSensorStatus CsDeviceSaltSensorStatusTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsDeviceSaltSensorStatus::SaltOkay;
        case 1: return CsDeviceSaltSensorStatus::SaltLow;

        default:
            ESP_LOGW("CsDeviceSaltSensorStatus", "Unknown salt sensor status value: %u", value);
            return CsDeviceSaltSensorStatus::Unknown;
    }
}

}
