#pragma once
#include "CsBleDevice.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleDeviceManagement {

class CsBleDeviceRegistry
{
public:
    /**
     * Type definition for a factory function that creates instances of CsBleDevice based on a given Bluetooth Low
     * Energy advertisement.
     */
    using DeviceFactory = std::function<std::unique_ptr<CsBleDevice>(const CsBleAdvertisement&)>;

public:
    /**
     * Get the instance of the CsBleDeviceRegistry singleton.
     *
     * @return A reference to the CsBleDeviceRegistry instance.
     */
    static CsBleDeviceRegistry& Instance();

public:
    /**
     * Register a device ID prefix that can be used to identify devices. This is typically used to register the prefixes
     * associated with different types of devices that the system can recognize. When a Bluetooth Low Energy
     * advertisement is received, the system can check the advertising name against the registered prefixes to determine
     * if the device is of a known type.
     *
     * @param[in] prefix The device ID prefix to register. This is typically a string that appears at the beginning of
     * the advertising name of devices of a certain type. For example, "C2_" might  be used as a prefix to identify
     * Signature Valve devices.
     * @param[in] factory A factory function that can be used to create instances of CsBleDevice for devices that match
     * the registered prefix. This allows the registry to not only recognize device types based on prefixes but also to
     * create appropriate device instances when needed.
     */
    void Register(std::string prefix, DeviceFactory factory);

    /**
     * Try to create a CsBleDevice instance based on the given Bluetooth Low Energy advertisement. This method checks
     * if the advertisement's name matches any of the registered device ID prefixes and, if so, uses the associated
     * factory function to create and return a CsBleDevice instance. If no matching prefix is found, this method returns
     * nullptr.
     *
     * @param[in] advertisement The Bluetooth Low Energy advertisement to use for creating a CsBleDevice instance.
     *
     * @return A unique pointer to the created CsBleDevice instance, or nullptr if no matching prefix is found.
     */
    std::unique_ptr<CsBleDevice> TryCreate(const CsBleAdvertisement& advertisement) const;

    /**
     * Check if the given name matches any of the registered device ID prefixes. This is typically used to determine if
     * a devices name matches a known device type based on the prefixes that have been registered in the registry.
     *
     * @param[in] name The name of the device to check.
     *
     * @return True if the name matches a registered prefix, false otherwise.
     */
    bool MatchesRegisteredPrefix(const std::string& name) const;

private:
    struct CsDeviceEntry
    {
        std::string Prefix;
        DeviceFactory Factory;
    };

    std::vector<CsDeviceEntry> _entries;
};

}
