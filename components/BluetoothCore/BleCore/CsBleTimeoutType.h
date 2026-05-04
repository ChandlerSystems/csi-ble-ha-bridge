#pragma once

namespace SignatureBridge::BluetoothCore::BleCore {

/**
 * The type of timeout that occurred when processing BLE packets. If the device stops receiving responses from the app,
 * then it will send a query timeout type. If the app does not respond with an ack, then the device will consider the
 * connection with the app dead and disconnect.
 */
enum class CsBleTimeoutType
{
    /**
     * The timeout type for when the device stops receiving responses from the app.
     */
    Query,

    /**
     * The response to the query timeout type.
     */
    Ack,
};

}
