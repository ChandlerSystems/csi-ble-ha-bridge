#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValveCyclePositionOption : uint16_t
{
    None                           = 0x0000U,
    ForwardOnly                    = 0x0001U,
    AerationPosition               = 0x0002U,
    OpenTwedoValve                 = 0x0004U,
    TimeIsSeconds                  = 0x0008U,
    TimeNotAdjustable              = 0x0010U,
    RecoveryPosition               = 0x0020U,
    CreepIntoPosition              = 0x0040U,
    EdgeCreepIntoPosition          = 0x0080U,
    TimeIsSaltPounds               = 0x0100U,
    BrineFillPosition              = 0x0200U,
    AirCycle                       = 0x0400U,
    CommercialBrineLineFlowControl = 0x0800U,
    TurnOnPowerOutput1             = 0x1000U,
    TurnOnPowerOutput2             = 0x2000U,
    SkipForwardOnly                = 0x4000U
};

inline CsSignatureValveCyclePositionOption CsSignatureValveCyclePositionOptionTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0x0000U: return CsSignatureValveCyclePositionOption::None;
        case 0x0001U: return CsSignatureValveCyclePositionOption::ForwardOnly;
        case 0x0002U: return CsSignatureValveCyclePositionOption::AerationPosition;
        case 0x0004U: return CsSignatureValveCyclePositionOption::OpenTwedoValve;
        case 0x0008U: return CsSignatureValveCyclePositionOption::TimeIsSeconds;
        case 0x0010U: return CsSignatureValveCyclePositionOption::TimeNotAdjustable;
        case 0x0020U: return CsSignatureValveCyclePositionOption::RecoveryPosition;
        case 0x0040U: return CsSignatureValveCyclePositionOption::CreepIntoPosition;
        case 0x0080U: return CsSignatureValveCyclePositionOption::EdgeCreepIntoPosition;
        case 0x0100U: return CsSignatureValveCyclePositionOption::TimeIsSaltPounds;
        case 0x0200U: return CsSignatureValveCyclePositionOption::BrineFillPosition;
        case 0x0400U: return CsSignatureValveCyclePositionOption::AirCycle;
        case 0x0800U: return CsSignatureValveCyclePositionOption::CommercialBrineLineFlowControl;
        case 0x1000U: return CsSignatureValveCyclePositionOption::TurnOnPowerOutput1;
        case 0x2000U: return CsSignatureValveCyclePositionOption::TurnOnPowerOutput2;
        case 0x4000U: return CsSignatureValveCyclePositionOption::SkipForwardOnly;

        default:
            ESP_LOGW("CsSignatureValveCyclePositionOption", "Unknown cycle position option value: %u", value);
            return CsSignatureValveCyclePositionOption::None;
    }
}

}
