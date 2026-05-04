#pragma once
#include <string>

namespace SignatureBridge::BluetoothCore::BleCore {

struct CsBleGattProfile
{
    /**
     * The UUID of the GATT service that this profile represents. This is used to identify the service and to group
     * characteristics together under a common service. The service UUID is typically a 128-bit UUID that is defined by
     * the Bluetooth SIG or by the device manufacturer for custom services.
     */
    std::string ServiceUuid;

    /**
     * The UUID of the GATT characteristic that is used for reading data. This is used to identify the characteristic
     * and to perform read operations on it. The characteristic UUID is typically a 128-bit UUID that is defined by
     * the Bluetooth SIG or by the device manufacturer for custom characteristics.
     */
    std::string ReadCharacteristicUuid;

    /**
     * The UUID of the GATT characteristic that is used for writing data. This is used to identify the characteristic
     * and to perform write operations on it. The characteristic UUID is typically a 128-bit UUID that is defined by
     * the Bluetooth SIG or by the device manufacturer for custom characteristics.
     */
    std::string WriteCharacteristicUuid;
};

}
