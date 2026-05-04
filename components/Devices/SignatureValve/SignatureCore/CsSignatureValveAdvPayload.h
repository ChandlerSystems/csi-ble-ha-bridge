#pragma once
#include "Common/CsDeviceOnOffState.h"
#include "Common/CsDeviceSaltSensorStatus.h"
#include "CsSignatureValveType.h"
#include "CsSignatureValveSeries.h"
#include <cstdint>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

using namespace SignatureBridge::Devices::Common;

struct CsSignatureValveAdvPayload
{
    uint8_t StatusBits = 0;
    uint8_t ErrorState = 0;
    uint8_t TimeHours = 0;
    uint8_t TimeMinutes = 0;
    CsSignatureValveType ValveType = CsSignatureValveType::Unknown;
    CsSignatureValveSeries Series = CsSignatureValveSeries::Unknown;
    uint8_t FirmwareMajor = 0;
    uint8_t FirmwareMinor = 0;
    CsDeviceSaltSensorStatus saltSensorStatus = CsDeviceSaltSensorStatus::Unknown;
    CsDeviceOnOffState waterState = CsDeviceOnOffState::Unknown;
    CsDeviceOnOffState bypassState = CsDeviceOnOffState::Unknown;
    bool IsValid = false;
};

}
