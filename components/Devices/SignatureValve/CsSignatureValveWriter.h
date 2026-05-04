#pragma once
#include "CsDeviceActiveState.h"
#include "CsDeviceAutoReserveMode.h"
#include "CsSignatureValveCyclePosition.h"
#include "CsSignatureValveDevice.h"
#include "CsSignatureValveState.h"
#include "CsSignatureValveWriteBuilder.h"
#include <string>

namespace SignatureBridge::Devices::SignatureValve {

using namespace SignatureBridge::Devices::Common;
using namespace SignatureBridge::Devices::SignatureValve::SignatureCore;

class CsSignatureValveWriter
{public:
    explicit CsSignatureValveWriter(CsSignatureValveDevice& device): _device(device) { }

public: // Auth
    bool AuthInvalidateKey();
    bool AuthEnableApi();

public: // Dashboard
    bool SetTime(int hours, int minutes, int seconds);
    bool SetWaterHardness(int value);
    bool SetDayOverride(int value);
    bool SetRegenTime(int hours, int minutes);
    bool SetPrefillEnabled(bool value);
    bool SetPrefillDuration(int value);
    bool SetBrineTankRemainingSaltPounds(int value);

public: // Settings
    bool SetRegenDayOverride(int value);
    bool SetAutoReserveMode(CsDeviceAutoReserveMode value);
    bool SetReserveCapacity(int value);
    bool SetTotalResinGrainsCapacity(int value);
    bool SetAirRechargeFrequency(int value);
    bool SetChlorinePulses(int value);
    bool SetDisplayState(CsDeviceActiveState value);
    bool SetCyclePosition(CsSignatureValveCyclePosition position, int value);

public: // History
    bool ResetRegenCounter();
    bool ResetGallonsTotalizer();

public: // Dealer Information
    bool SetDealerInformation(
        const std::string& name,
        const std::string& phone,
        const std::string& address1,
        const std::string& address2,
        const std::string& email,
        const std::string& web
    );

public: // Global
    bool RegenNow();
    bool RegenNext();
    bool AerateNow();
    bool AerateNext();
    bool FindHome();

private:
    CsSignatureValveDevice& _device;
};

}
