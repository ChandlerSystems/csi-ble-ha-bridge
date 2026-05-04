#pragma once

namespace SignatureBridge::BluetoothCore::BleCore {

enum class CsBleAckNakType
{
    /**
     * Negative acknowledgement type for when the packet was not received correctly and it needs resent.
     */
    Nak,

    /**
     * Acknowledgement type for when the packet was received correctly.
     */
    Ack,
};

}
