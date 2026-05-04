#pragma once
#include <cstdint>
#include <esp_log.h>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValveRegenState : uint8_t
{
    Idle = 0U,
    MovingToNextPosition = 1U,
    MovingToFinalPosition = 2U,
    TwedoWaitingForMotorState = 3U,
    WaitingForTwedoState = 4U,
    WaitingInPosition = 5U,
    MovingToService = 6U,
    MovingToBypass = 7U,
    InBypass = 8U,
    MovingToBrineSoak = 9U,
    WaitingInBrineSoak = 10U,
    MovingToCreepPosition = 11U,
    CreepingToPosition = 12U
};

inline CsSignatureValveRegenState CsSignatureValveRegenStateTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsSignatureValveRegenState::Idle;
        case 1: return CsSignatureValveRegenState::MovingToNextPosition;
        case 2: return CsSignatureValveRegenState::MovingToFinalPosition;
        case 3: return CsSignatureValveRegenState::TwedoWaitingForMotorState;
        case 4: return CsSignatureValveRegenState::WaitingForTwedoState;
        case 5: return CsSignatureValveRegenState::WaitingInPosition;
        case 6: return CsSignatureValveRegenState::MovingToService;
        case 7: return CsSignatureValveRegenState::MovingToBypass;
        case 8: return CsSignatureValveRegenState::InBypass;
        case 9: return CsSignatureValveRegenState::MovingToBrineSoak;
        case 10: return CsSignatureValveRegenState::WaitingInBrineSoak;
        case 11: return CsSignatureValveRegenState::MovingToCreepPosition;
        case 12: return CsSignatureValveRegenState::CreepingToPosition;

        default:
            ESP_LOGW("CsSignatureValveRegenState", "Unknown regen state value: %u", value);
            return CsSignatureValveRegenState::Idle;
    }
}

}
