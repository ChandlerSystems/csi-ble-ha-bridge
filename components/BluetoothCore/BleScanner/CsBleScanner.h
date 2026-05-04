#pragma once
#include "CsCore.h"
#include <host/ble_gap.h>
#include <string>

namespace SignatureBridge::BluetoothCore::BleScanner {

class CsBleScanner
{
    CS_CLASS_TAG(CsBleScanner)

public:
    /**
     * @brief Starts the BLE scan.
     *
     * @return True if the scan was started successfully; otherwise false.
     */
    bool StartScan();

    /**
     * @brief Stops an active BLE scan.
     *
     * @return True if the scan was stopped successfully; otherwise false.
     */
    bool StopScan();

private:
    bool _isScanning = false;
};

}
