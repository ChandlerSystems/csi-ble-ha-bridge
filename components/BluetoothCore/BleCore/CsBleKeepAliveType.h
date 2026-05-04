#pragma once

namespace SignatureBridge::BluetoothCore::BleCore {

enum class CsBleKeepAliveType
{
    /**
     * Marco type when either the device or the app is sending a keep-alive packet
     */
    Marco,

    /**
     * Type when either the device or the app is responding to a keep-alive marco packet
     */
    Polo,
};

}
