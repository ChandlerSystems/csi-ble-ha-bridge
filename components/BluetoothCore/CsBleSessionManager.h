#pragma once
#include "BleDeviceManagement/CsBleConnectionOptions.h"
#include "BleDeviceManagement/CsBleDevice.h"
#include "BleScanner/CsBleScanner.h"
#include <esp_err.h>
#include <string>

namespace SignatureBridge::BluetoothCore {

using namespace SignatureBridge::BluetoothCore::BleScanner;
using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;

class CsBleSessionManager
{
    CS_CLASS_TAG(CsBleSessionManager)

public:
    /**
     * Initializes the BLE session manager, setting up necessary resources and preparing it for scanning and managing
     * BLE devices.
     *
     * @return True if initialization was successful; otherwise false.
     */
    bool Initialize();

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

public:
    /**
     * Requests a connection to the specified BLE device. The session manager will handle the connection process,
     * including authentication using the provided token.
     *
     * @param[in] device The BLE device to connect to.
     * @param[in] options The CsBleConnectionOptions instance containing the connection options to use for the
     *                    connection.
     */
    void RequestConnection(CsBleDevice* device, const CsBleConnectionOptions& options);

private:
    bool _initialized = false;
    CsBleScanner _scanner;
};

}
