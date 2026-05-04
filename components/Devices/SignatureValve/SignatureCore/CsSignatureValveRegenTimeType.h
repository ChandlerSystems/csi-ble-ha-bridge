#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValveRegenTimeType : uint8_t
{
    Seconds = 0U,
    Minutes = 1U,
    SaltPounds = 2U
};

inline CsSignatureValveRegenTimeType CsSignatureValveRegenTimeTypeTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsSignatureValveRegenTimeType::Seconds;
        case 1: return CsSignatureValveRegenTimeType::Minutes;
        case 2: return CsSignatureValveRegenTimeType::SaltPounds;

        default:
            ESP_LOGW("CsSignatureValveRegenTimeType", "Unknown regen time type value: %u", value);
            return CsSignatureValveRegenTimeType::Seconds;
    }
}

}
