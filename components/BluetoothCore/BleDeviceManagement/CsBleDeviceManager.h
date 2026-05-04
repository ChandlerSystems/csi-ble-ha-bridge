#pragma once
#include "CsBleConnectionOptions.h"
#include "CsBleDevice.h"
#include "CsBleScanner.h"
#include "CsCore.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace SignatureBridge::BluetoothCore {
    class CsBleSessionManager;
}

namespace SignatureBridge::BluetoothCore::BleDeviceManagement {

using namespace SignatureBridge::BluetoothCore::BleScanner;

class CsBleDeviceManager
{
    CS_CLASS_TAG(CsBleDeviceManager)

public:
    /**
     * Get the singleton instance of CsBleDeviceManager.
     *
     * @return Reference to the singleton instance of CsBleDeviceManager.
     */
    static CsBleDeviceManager& Instance();

public:
    /**
     * Set the session manager that will be used to manage BLE sessions and connections. This is necessary for the
     * device manager to be able to request connections to devices when they are discovered. The session manager is
     * responsible for handling the actual connection process and managing the BLE sessions.
     *
     * @param[in] session Pointer to the session manager instance.
     */
    void SetSessionManager(CsBleSessionManager* session);

public:
    /**
     * Set the connection options that will be used when connecting to devices. This includes options such as the
     * authentication token and the optional target MAC address.
     *
     * @param[in] options The CsBleConnectionOptions instance containing the connection options to set.
     */
    void SetConnectionOptions(const CsBleConnectionOptions& options);

    /**
     * Get the current connection options that are being used for connecting to devices.
     *
     * @return A reference to the current CsBleConnectionOptions instance containing the connection options.
     */
    const CsBleConnectionOptions& GetConnectionOptions() const;

public:
    /**
     * Determines whether the device should attempt to connect to a given advertisement based on the target MAC address.
     *
     * @param[in] discovery Pointer to the BLE GAP discovery descriptor.
     *
     * @return True if the device should attempt to connect, false otherwise.
     */
    bool ShouldConnectToAdvertisement(const ble_gap_disc_desc* discovery) const;

public:
    /**
     * Handle a new Bluetooth Low Energy advertisement. This method is called when a new advertisement is received, and
     * it is responsible for updating the device registry with the information from the advertisement. If the device
     * already exists in the registry, it will update the existing device instance with the new advertisement
     * information. If the device does not exist in the registry, it will create a new device instance based on the
     * advertisement and add it to the registry.
     *
     * @param[in] advertisement The Bluetooth Low Energy advertisement to process.
     * @param[in] discovery Pointer to the BLE GAP discovery descriptor.
     */
    void OnAdvertisement(const CsBleAdvertisement& advertisement, const ble_gap_disc_desc* discovery);

private:
    bool DoesAdvertisedDeviceMatchTarget(const ble_gap_disc_desc* discovery) const;
    void TryConnectToTarget(CsBleDevice* device, const ble_gap_disc_desc* discovery);
    void TryConnectToFirstMatch();

private:
    CsBleConnectionOptions _connectionOptions;
    CsBleSessionManager* _sessionManager = nullptr;
    std::unordered_map<std::string, std::unique_ptr<CsBleDevice>> _devices;
};

}
