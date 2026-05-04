#include "CsDeviceBootstrap.h"
#include "SignatureValve/CsSignatureValveDevice.h"

using namespace SignatureBridge::Devices;
using namespace SignatureBridge::Devices::SignatureValve;

void CsDeviceBootstrap::BootstrapDevices()
{
    CsSignatureValveDevice::Register();

    // Register other devices here as needed in the future
}
