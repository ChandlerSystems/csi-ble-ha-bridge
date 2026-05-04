#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValveBrineTank : uint8_t
{
    Unknown = 0U,
    Tank16X33 = 16U,
    Tank18X40 = 18U,
    Tank24X50 = 24U,
    Tank30X50 = 30U
};

inline CsSignatureValveBrineTank CsSignatureValveBrineTankTryFromInt(int32_t value)
{
    switch (value)
    {
        case 16: return CsSignatureValveBrineTank::Tank16X33;
        case 18: return CsSignatureValveBrineTank::Tank18X40;
        case 24: return CsSignatureValveBrineTank::Tank24X50;
        case 30: return CsSignatureValveBrineTank::Tank30X50;

        default:
            ESP_LOGW("CsSignatureValveBrineTank", "Unknown brine tank value: %u", value);
            return CsSignatureValveBrineTank::Unknown;
    }
}

}
