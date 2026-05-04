#pragma once
#include "CsBleDevice.h"

namespace SignatureBridge::Devices {

using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;

class CsDeviceBootstrap : public CsBleDevice
{
public:
    static void BootstrapDevices();
};

}
