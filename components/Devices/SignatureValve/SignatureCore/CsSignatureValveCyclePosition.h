#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValveCyclePosition : uint8_t
{
    Unknown = 0U,
    Position1 = 1U,
    Position2 = 2U,
    Position3 = 3U,
    Position4 = 4U,
    Position5 = 5U,
    Position6 = 6U,
    Position7 = 7U,
    Position8 = 8U
};

inline CsSignatureValveCyclePosition CsSignatureValveCyclePositionTryFromInt(int32_t value)
{
    switch (value)
    {
        case 1: return CsSignatureValveCyclePosition::Position1;
        case 2: return CsSignatureValveCyclePosition::Position2;
        case 3: return CsSignatureValveCyclePosition::Position3;
        case 4: return CsSignatureValveCyclePosition::Position4;
        case 5: return CsSignatureValveCyclePosition::Position5;
        case 6: return CsSignatureValveCyclePosition::Position6;
        case 7: return CsSignatureValveCyclePosition::Position7;
        case 8: return CsSignatureValveCyclePosition::Position8;

        default:
            ESP_LOGW("CsSignatureValveCyclePosition", "Unknown cycle position value: %u", value);
            return CsSignatureValveCyclePosition::Unknown;
    }
}

}
