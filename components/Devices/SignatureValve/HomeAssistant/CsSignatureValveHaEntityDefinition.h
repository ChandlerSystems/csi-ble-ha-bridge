#pragma once

#include "CsSignatureValveHaCapability.h"
#include "CsSignatureValveHaEntityType.h"
#include "CsSignatureValvePacketKey.h"
#include "CsSignatureValveState.h"

#include <string>

namespace SignatureBridge::Devices::SignatureValve::HomeAssistant
{
    using CsSignatureValveHaCustomValueResolver =
        bool (*)(const SignatureCore::CsSignatureValveState& state, std::string& value);

    struct CsSignatureValveHaEntityDefinition
    {
        const char* ObjectId;
        const char* Name;
        CsSignatureValveHaEntityType Type;
        CsSignatureValveHaCapability RequiredCapabilities;
        SignatureCore::CsSignatureValvePacketKey PacketKey;
        CsSignatureValveHaCustomValueResolver CustomResolver = nullptr;
    };
}
