#include "CsCoreUtils.h"
#include "CsSignatureValveHaStateValueResolver.h"
#include "CsSignatureValveRegenState.h"
#include "CsSignatureValveType.h"

namespace SignatureBridge::Devices::SignatureValve::HomeAssistant {

using namespace SignatureBridge::Core;
using namespace SignatureBridge::Devices::SignatureValve::SignatureCore;

bool CsSignatureValveHaStateValueResolver::TryResolve(
    const CsSignatureValveState& state,
    CsSignatureValvePacketKey key,
    std::string& value
)
{
    switch (key)
    {
        case CsSignatureValvePacketKey::GlRegenActive:
            value = state.GlRegenActive ? "ON" : "OFF";
            return true;

        case CsSignatureValvePacketKey::GlPresentFlow:
            value = std::to_string(state.GlPresentFlow);
            return true;

        case CsSignatureValvePacketKey::GlRegenState:
            value = CsSignatureValveTypeGetRegenStepString
            (
                state.GenDeviceType,
                state.GenFirmwareVersion,
                state.DbRegenCurrentPosition,
                state.DbRegenSoakTimer,
                state.DlIsTwinValve,
                state.DbPrefillSoakMode
            );
            return true;

        case CsSignatureValvePacketKey::GenFirmwareVersion:
            value = CsCoreUtils::Instance().FirmwareVersionToString(state.GenFirmwareVersion);
            return true;

        case CsSignatureValvePacketKey::GenDeviceType:
            value = CsSignatureValveTypeToString(state.GenDeviceType);
            return true;

        case CsSignatureValvePacketKey::DbBrineTankRemainingSaltPounds:
            value = std::to_string(state.DbBrineTankRemainingSaltPounds);
            return true;

        case CsSignatureValvePacketKey::AsAerationDays:
            value = std::to_string(state.AsAerationDays);
            return true;

        default:
            return false;
    }
}

std::string CsSignatureValveHaStateValueResolver::ResolveCurrentTime(const CsSignatureValveState& state)
{
    const int hours   = state.DbHours   % 24;
    const int minutes = state.DbMinutes % 60;
    const int seconds = state.DbSeconds % 60;
    char buffer[9];

    std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, minutes, seconds);

    return std::string(buffer);
}

}
