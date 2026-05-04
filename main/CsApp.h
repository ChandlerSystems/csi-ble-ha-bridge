#pragma once
#include "BleDeviceManagement/CsBleConnectionOptions.h"
#include "CsBleSessionManager.h"
#include "CsCore.h"

namespace SignatureBridge {

using namespace BluetoothCore;
using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;

class CsApp
{
    CS_CLASS_TAG(CsApp)

public:
    void Initialize();
    void Run();

public:
    void SetBleConnectionOptions(const CsBleConnectionOptions& options);

private:
    bool StartWifi();
    bool StartMqtt();

private:
    bool _initialized = false;
    CsBleSessionManager _bleSessionManager;
};

}
