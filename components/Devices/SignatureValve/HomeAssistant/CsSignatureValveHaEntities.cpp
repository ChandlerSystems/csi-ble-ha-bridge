#include "CsSignatureValveHaEntities.h"
#include "CsSignatureValveHaStateValueResolver.h"

namespace SignatureBridge::Devices::SignatureValve::HomeAssistant {

using namespace SignatureBridge::Devices::SignatureValve::SignatureCore;

static bool ResolveCurrentTimeValue(const CsSignatureValveState& state, std::string& value)
{
    value = CsSignatureValveHaStateValueResolver::ResolveCurrentTime(state);
    return !value.empty();
}

static const CsSignatureValveHaEntityDefinition _signatureValveHaEntities[] =
{
    {
        "time",
        "Time",
        CsSignatureValveHaEntityType::Sensor,
        CsSignatureValveHaCapability::None,
        CsSignatureValvePacketKey::DbHours,
        ResolveCurrentTimeValue
    },
    {
        "regen_active",
        "Regenerating",
        CsSignatureValveHaEntityType::BinarySensor,
        CsSignatureValveHaCapability::None,
        CsSignatureValvePacketKey::GlRegenActive
    },
    {
        "regen_step",
        "Regen Step",
        CsSignatureValveHaEntityType::Sensor,
        CsSignatureValveHaCapability::None,
        CsSignatureValvePacketKey::GlRegenState
    },
    {
        "device_type",
        "Device Type",
        CsSignatureValveHaEntityType::Sensor,
        CsSignatureValveHaCapability::None,
        CsSignatureValvePacketKey::GenDeviceType
    },
    {
        "firmware_version",
        "Firmware Version",
        CsSignatureValveHaEntityType::Sensor,
        CsSignatureValveHaCapability::None,
        CsSignatureValvePacketKey::GenFirmwareVersion
    },
    {
        "present_flow",
        "Present Flow",
        CsSignatureValveHaEntityType::Sensor,
        CsSignatureValveHaCapability::IsSdMeteredSoftener,
        CsSignatureValvePacketKey::GlPresentFlow
    },
    {
        "salt_remaining",
        "Salt Remaining",
        CsSignatureValveHaEntityType::Sensor,
        CsSignatureValveHaCapability::None,
        CsSignatureValvePacketKey::DbBrineTankRemainingSaltPounds
    },
    {
        "aeration_days",
        "Aeration Days",
        CsSignatureValveHaEntityType::Sensor,
        CsSignatureValveHaCapability::HasAerationCycle,
        CsSignatureValvePacketKey::AsAerationDays
    }
};

const CsSignatureValveHaEntityDefinition* GetSignatureValveHaEntities(size_t& count)
{
    count = sizeof(_signatureValveHaEntities) / sizeof(_signatureValveHaEntities[0]);
    return _signatureValveHaEntities;
}

}
