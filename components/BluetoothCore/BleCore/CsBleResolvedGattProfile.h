#pragma once
#include "CsBleGattService.h"
#include "CsBleGattCharacteristic.h"

namespace SignatureBridge::BluetoothCore::BleCore {

struct CsBleResolvedGattProfile
{
    /**
     * The resolved GATT service that matches the profile's service UUID. This will be nullptr if the service was not
     * found on the device.
     */
    const CsBleGattService* Service = nullptr;

    /**
     * The resolved GATT characteristic that matches the profile's read characteristic UUID. This will be nullptr if
     * the characteristic was not found on the device.
     */
    const CsBleGattCharacteristic* ReadCharacteristic = nullptr;

    /**
     * The resolved GATT characteristic that matches the profile's write characteristic UUID. This will be nullptr if
     * the characteristic was not found on the device.
     */
    const CsBleGattCharacteristic* WriteCharacteristic = nullptr;
};

}
