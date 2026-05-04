#pragma once
#include "CsSignatureValveError.h"
#include <cstdint>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

struct CsSignatureValveErrorLogEntry
{
    int32_t Day = 0;
    int32_t Hour = 0;
    int32_t Minute = 0;
    int32_t Second = 0;
    CsSignatureValveError Error = CsSignatureValveError::NoError;

    [[nodiscard]] bool HasValidData() const
    {
        return Error != CsSignatureValveError::NoError;
    }
};

}
