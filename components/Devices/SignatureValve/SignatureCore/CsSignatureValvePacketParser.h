#pragma once
#include "CsBlePacketChange.h"
#include "CsCore.h"
#include "CsSignatureValvePacketKey.h"
#include "CsSignatureValveState.h"
#include <vector>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

using namespace SignatureBridge::BluetoothCore::BleCore;

class CsSignatureValvePacketParser
{
    CS_CLASS_TAG(CsSignatureValvePacketParser)

public:
    [[nodiscard]] bool ApplyChanges(
        CsSignatureValveState& state,
        const std::vector<CsBlePacketChange>& changes
    ) const;

private:
    [[nodiscard]] bool ApplyChange(
        CsSignatureValveState& state,
        const CsBlePacketChange& change
    ) const;

    [[nodiscard]] bool ApplyScalarChange(
        CsSignatureValveState& state,
        CsSignatureValvePacketKey key,
        const CsBlePacketValue& value
    ) const;

    [[nodiscard]] bool ApplyArrayChange(
        CsSignatureValveState& state,
        CsSignatureValvePacketKey key,
        const CsBlePacketValue& value
    ) const;

private:
    [[nodiscard]] static uint8_t GetBatteryCapacity(float volts);

private:
    [[nodiscard]] static std::vector<float> ConvertPeakFlowGraph(const std::vector<int32_t>& values);
    [[nodiscard]] static std::vector<float> ConvertGallonsGraph(
        const CsSignatureValveState& state,
        const std::vector<int32_t>& values
    );

private:
    static bool TryApplyDealerInfoString(const CsBlePacketValue& value, std::string& target);
};

}
