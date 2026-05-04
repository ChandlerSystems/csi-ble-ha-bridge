#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValveSeries : uint8_t
{
    Unknown = 0U,
    Series2 = 2U,
    Series3 = 3U,
    Series4 = 4U,
    Series5 = 5U,
    Series6 = 6U
};

inline CsSignatureValveSeries CsSignatureValveSeriesTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsSignatureValveSeries::Unknown;
        case 2: return CsSignatureValveSeries::Series2;
        case 3: return CsSignatureValveSeries::Series3;
        case 4: return CsSignatureValveSeries::Series4;
        case 5: return CsSignatureValveSeries::Series5;
        case 6: return CsSignatureValveSeries::Series6;

        default:
            ESP_LOGW("CsSignatureValveSeries", "Unknown series value: %u", value);
            return CsSignatureValveSeries::Unknown;
    }
}

}
