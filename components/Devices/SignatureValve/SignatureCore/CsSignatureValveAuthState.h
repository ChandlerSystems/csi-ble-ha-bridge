#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValveAuthState : uint8_t
{
    Unknown,
    Unauthenticated,
    Authenticated,
    RateLimited,
    AuthRequestTimedOut
};

inline CsSignatureValveAuthState CsSignatureValveAuthStateTryFromInt(int32_t value)
{
    switch (value)
    {
        case 1: return CsSignatureValveAuthState::Unauthenticated;
        case 2: return CsSignatureValveAuthState::Authenticated;
        case 3: return CsSignatureValveAuthState::RateLimited;
        case 4: return CsSignatureValveAuthState::AuthRequestTimedOut;

        default:
            ESP_LOGW("CsSignatureValveAuthState", "Unknown auth state value: %u", value);
            return CsSignatureValveAuthState::Unknown;
    }
}

}
