#pragma once
#include "CsSignatureValveCyclePositionOption.h"
#include <vector>
#include <cstdint>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

class CsSignatureValveCyclePositionOptionsSet
{
public:
    void Add(CsSignatureValveCyclePositionOption option)
    {
        _flags |= static_cast<uint16_t>(option);
    }

    void Remove(CsSignatureValveCyclePositionOption option)
    {
        _flags &= static_cast<uint16_t>(~static_cast<uint16_t>(option));
    }

    void Clear()
    {
        _flags = 0U;
    }

    [[nodiscard]] bool Contains(CsSignatureValveCyclePositionOption option) const
    {
        return (_flags & static_cast<uint16_t>(option)) != 0U;
    }

    [[nodiscard]] std::vector<CsSignatureValveCyclePositionOption> ToList() const
    {
        std::vector<CsSignatureValveCyclePositionOption> result;

        for (const auto option : AllOptions())
        {
            if (Contains(option))
            {
                result.push_back(option);
            }
        }

        return result;
    }

    [[nodiscard]] uint16_t Raw() const
    {
        return _flags;
    }

    void SetRaw(uint16_t flags)
    {
        _flags = flags;
    }

private:
    static const std::vector<CsSignatureValveCyclePositionOption>& AllOptions()
    {
        static const std::vector<CsSignatureValveCyclePositionOption> options =
        {
            CsSignatureValveCyclePositionOption::ForwardOnly,
            CsSignatureValveCyclePositionOption::AerationPosition,
            CsSignatureValveCyclePositionOption::OpenTwedoValve,
            CsSignatureValveCyclePositionOption::TimeIsSeconds,
            CsSignatureValveCyclePositionOption::TimeNotAdjustable,
            CsSignatureValveCyclePositionOption::RecoveryPosition,
            CsSignatureValveCyclePositionOption::CreepIntoPosition,
            CsSignatureValveCyclePositionOption::EdgeCreepIntoPosition,
            CsSignatureValveCyclePositionOption::TimeIsSaltPounds,
            CsSignatureValveCyclePositionOption::BrineFillPosition,
            CsSignatureValveCyclePositionOption::AirCycle,
            CsSignatureValveCyclePositionOption::CommercialBrineLineFlowControl,
            CsSignatureValveCyclePositionOption::TurnOnPowerOutput1,
            CsSignatureValveCyclePositionOption::TurnOnPowerOutput2,
            CsSignatureValveCyclePositionOption::SkipForwardOnly
        };

        return options;
    }

private:
    uint16_t _flags = 0U;
};

}
