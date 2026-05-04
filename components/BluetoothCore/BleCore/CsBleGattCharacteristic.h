#pragma once
#include <host/ble_uuid.h>

namespace SignatureBridge::BluetoothCore::BleCore {

struct CsBleGattCharacteristic
{
    /**
     * The UUID of the GATT service that this characteristic belongs to. This is used to identify the service and to
     * group characteristics together under a common service. The service UUID is typically a 128-bit UUID that is
     * defined by the Bluetooth SIG or by the device manufacturer for custom services.
     */
    ble_uuid_any_t ServiceUuid = {};

    /**
     * The UUID of the GATT characteristic. This is used to identify the characteristic and to perform read/write
     * operations on it. The characteristic UUID is typically a 128-bit UUID that is defined by the Bluetooth SIG or by
     * the device manufacturer for custom characteristics.
     */
    ble_uuid_any_t CharacteristicUuid = {};

    /**
     * The handle of the characteristic definition. This is a unique identifier for the characteristic within the GATT
     * server's attribute table. The definition handle is used by the client to reference the characteristic when
     * performing operations such as reading, writing, or subscribing to notifications.
     */
    uint16_t DefinitionHandle = 0;

    /**
     * The handle of the characteristic's value. This is a unique identifier for the characteristic's value attribute
     * within the GATT server's attribute table. The value handle is used by the client to read from or write to the
     * characteristic's value, and to subscribe to notifications or indications from the characteristic.
     */
    uint16_t ValueHandle = 0;

    /**
     * The properties of the characteristic. This is a bitmask that defines the behavior and capabilities of the
     * characteristic, such as whether it supports read, write, notify, or indicate operations. The properties are
     * defined by the Bluetooth SIG and are used by the client to determine how it can interact with the characteristic.
     */
    uint8_t Properties = 0;
};

}
