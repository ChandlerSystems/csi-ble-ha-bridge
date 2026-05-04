#pragma once

namespace SignatureBridge::BluetoothCore::BleTransport {

enum class CsBlePacketTransportEventType
{
    None,
    Ack,
    Nak,
    KeepAlive,
    TimeoutAck,
    TimeoutQuery,
    PayloadComplete,
    CrcError,
    ProtocolError,
    PacketReceived,
    PacketDecoded
};

}
