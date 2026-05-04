#include "CsSignatureValveWriter.h"

using namespace SignatureBridge::Devices::SignatureValve;

bool CsSignatureValveWriter::AuthInvalidateKey()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::AuthInvalidateKey(),
        "Invalid auth invalidate key write requested"
    );
}

bool CsSignatureValveWriter::AuthEnableApi()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::AuthEnableApi(),
        "Invalid auth enable API write requested"
    );
}

bool CsSignatureValveWriter::SetTime(int hours, int minutes, int seconds)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetTime(hours, minutes, seconds),
        "Invalid time write requested"
    );
}

bool CsSignatureValveWriter::SetWaterHardness(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetWaterHardness(value),
        "Invalid water hardness write requested"
    );
}

bool CsSignatureValveWriter::SetDayOverride(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetDayOverride(value),
        "Invalid day override write requested"
    );
}

bool CsSignatureValveWriter::SetRegenTime(int hours, int minutes)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetRegenTime(hours, minutes),
        "Invalid regen time write requested"
    );
}

bool CsSignatureValveWriter::SetPrefillEnabled(bool value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetPrefillEnabled(value),
        "Invalid prefill enabled write requested"
    );
}

bool CsSignatureValveWriter::SetPrefillDuration(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetPrefillDuration(value),
        "Invalid prefill duration write requested"
    );
}

bool CsSignatureValveWriter::SetBrineTankRemainingSaltPounds(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetBrineTankRemainingSaltPounds(value),
        "Invalid brine tank remaining salt pounds write requested"
    );
}

bool CsSignatureValveWriter::SetRegenDayOverride(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetRegenDayOverride(value),
        "Invalid regen day override write requested"
    );
}

bool CsSignatureValveWriter::SetAutoReserveMode(CsDeviceAutoReserveMode value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetAutoReserveMode(value),
        "Invalid auto reserve mode write requested"
    );
}

bool CsSignatureValveWriter::SetReserveCapacity(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetReserveCapacity(value),
        "Invalid reserve capacity write requested"
    );
}

bool CsSignatureValveWriter::SetTotalResinGrainsCapacity(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetTotalResinGrainsCapacity(value),
        "Invalid total resin grains capacity write requested"
    );
}

bool CsSignatureValveWriter::SetAirRechargeFrequency(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetAirRechargeFrequency(value),
        "Invalid air recharge frequency write requested"
    );
}

bool CsSignatureValveWriter::SetChlorinePulses(int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetChlorinePulses(value),
        "Invalid chlorine pulses write requested"
    );
}

bool CsSignatureValveWriter::SetDisplayState(CsDeviceActiveState value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetDisplayState(value),
        "Invalid display state write requested"
    );
}

bool CsSignatureValveWriter::SetCyclePosition(CsSignatureValveCyclePosition position, int value)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetCyclePosition(_device.State(), position, value),
        "Invalid cycle position write requested"
    );
}

bool CsSignatureValveWriter::ResetRegenCounter()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::ResetRegenCounter(),
        "Invalid regen counter reset write requested"
    );
}

bool CsSignatureValveWriter::ResetGallonsTotalizer()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::ResetGallonsTotalizer(),
        "Invalid gallons totalizer reset write requested"
    );
}

bool CsSignatureValveWriter::SetDealerInformation(
    const std::string& name,
    const std::string& phone,
    const std::string& address1,
    const std::string& address2,
    const std::string& email,
    const std::string& web
)
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::SetDealerInformation(
            name,
            phone,
            address1,
            address2,
            email,
            web
        ),
        "Invalid dealer information write requested"
    );
}

bool CsSignatureValveWriter::RegenNow()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::RegenNow(),
        "Invalid regen now write requested"
    );
}

bool CsSignatureValveWriter::RegenNext()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::RegenNext(),
        "Invalid regen next write requested"
    );
}

bool CsSignatureValveWriter::AerateNow()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::AerateNow(),
        "Invalid aerate now write requested"
    );
}

bool CsSignatureValveWriter::AerateNext()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::AerateNext(),
        "Invalid aerate next write requested"
    );
}

bool CsSignatureValveWriter::FindHome()
{
    return _device.TrySendWriteCommand(
        CsSignatureValveWriteBuilder::FindHome(),
        "Invalid find home write requested"
    );
}
