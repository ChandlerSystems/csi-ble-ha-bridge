#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValveError : uint8_t
{
    NoError = 0U,
    LostHome = 2U,
    NotSeeingSlotsNormalMotorCurrent = 3U,
    UnableToFindHomingSlot = 4U,
    NotSeeingSlotsHighMotorCurrent = 5U,
    NotSeeingSlotsNoMotorCurrent = 6U,
    TwedoMotorTimeout = 7U,
    RegenAbortedOnBattery = 0xC0U,
    PowerDebounced = 0xC1U
};

inline CsSignatureValveError CsSignatureValveErrorTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsSignatureValveError::NoError;
        case 2: return CsSignatureValveError::LostHome;
        case 3: return CsSignatureValveError::NotSeeingSlotsNormalMotorCurrent;
        case 4: return CsSignatureValveError::UnableToFindHomingSlot;
        case 5: return CsSignatureValveError::NotSeeingSlotsHighMotorCurrent;
        case 6: return CsSignatureValveError::NotSeeingSlotsNoMotorCurrent;
        case 7: return CsSignatureValveError::TwedoMotorTimeout;
        case 0xC0: return CsSignatureValveError::RegenAbortedOnBattery;
        case 0xC1: return CsSignatureValveError::PowerDebounced;

        default:
            ESP_LOGW("CsSignatureValveError", "Unknown error value: %u", value);
            return CsSignatureValveError::NoError;
    }
}

}

