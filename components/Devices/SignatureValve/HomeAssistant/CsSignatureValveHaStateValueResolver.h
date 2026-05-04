#pragma once
#include "CsSignatureValveState.h"
#include "CsSignatureValvePacketKey.h"
#include <string>

namespace SignatureBridge::Devices::SignatureValve::HomeAssistant {

class CsSignatureValveHaStateValueResolver
{
public:
    static bool TryResolve(
        const SignatureCore::CsSignatureValveState& state,
        SignatureCore::CsSignatureValvePacketKey key,
        std::string& value
    );

    static std::string ResolveCurrentTime(const SignatureCore::CsSignatureValveState& state);
};

}
