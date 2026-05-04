#pragma once
#include <host/ble_uuid.h>

namespace SignatureBridge::BluetoothCore::BleCore {

struct CsBleGattService
{
    /**
     * The UUID of the GATT service. This is used to identify the service and to group characteristics together under a
     * common service. The service UUID is typically a 128-bit UUID that is defined by the Bluetooth SIG or by the
     * device manufacturer for custom services.
     */
    ble_uuid_any_t Uuid = {};

    /**
     * The handle of the first attribute in the service. This is used to define the range of attributes that belong to
     * the service.
     */
    uint16_t StartHandle = 0;

    /**
     * The handle of the last attribute in the service. This is used to define the range of attributes that belong to
     * the service.
     */
    uint16_t EndHandle = 0;
};

}
