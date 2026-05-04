#pragma once
#include <cstdint>

namespace SignatureBridge::Devices::SignatureValve::HomeAssistant {

enum class CsSignatureValveHaCapability : uint32_t
{
    None = 0,
    IsSoftener = 1 << 0,
    IsAerationFilter = 1 << 1,
    IsCommercial = 1 << 2,
    IsSd = 1 << 3,
    IsSdMeteredSoftener = 1 << 4,
    HasAerationCycle = 1 << 5,
    HasSaltDose = 1 << 6,
};

inline CsSignatureValveHaCapability operator|(CsSignatureValveHaCapability a, CsSignatureValveHaCapability b)
{
    return static_cast<CsSignatureValveHaCapability>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

inline bool HasCapability(CsSignatureValveHaCapability value, CsSignatureValveHaCapability flag)
{
    return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0U;
}

}
