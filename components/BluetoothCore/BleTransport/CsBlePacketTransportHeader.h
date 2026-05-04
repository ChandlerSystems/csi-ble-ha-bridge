#pragma once
#include <cstdint>

namespace SignatureBridge::BluetoothCore::BleTransport {

class CsBlePacketTransportHeader
{
public:
    CsBlePacketTransportHeader(uint8_t header);

public:
    uint8_t GetRawHeader() const { return _rawHeader; }
    bool IsNoop() const { return _isNoop; }
    bool IsIdPacket() const { return _isIdPacket; }
    bool IsFirstPacket() const { return _isFirstPacket; }
    bool IsLastPacket() const { return _isLastPacket; }
    bool IsSinglePacket() const { return _isSinglePacket; }
    bool IsStatus() const { return _isStatus; }
    bool IsStatusKeepAlive() const { return _isStatusKeepAlive; }
    bool IsStatusKeepAliveMarco() const { return _isStatusKeepAliveMarco; }
    bool IsStatusKeepAlivePolo() const { return _isStatusKeepAlivePolo; }
    bool IsStatusAckNak() const { return _istatusAckNak; }
    bool IsStatusAck() const { return _istatusAck; }
    bool IsStatusNak() const { return _istatusNak; }
    bool IsStatusTimeout() const { return _isStatusTimeout; }
    bool IsStatusTimeoutAck() const { return _isStatusTimeoutAck; }
    bool IsStatusTimeoutQuery() const { return _isStatusTimeoutQuery; }

public:
    // A NOP (no operation) response.
    static constexpr uint8_t Nop = 0x00;

    // Bit to indicate the type of timeout response.
    static constexpr uint8_t TimeoutType = 0x01;

    // Bit to indicate that this is a timeout response.
    static constexpr uint8_t Timeout = 0x02;

    // Bit to indicate the type of ACK/NAK response.
    static constexpr uint8_t AckNakType = 0x04;

    // Bit to indicate that this is an ACK/NAK response.
    static constexpr uint8_t AckNak = 0x08;

    // Bit to indicate the type of keep-alive response.
    static constexpr uint8_t KeepAliveType = 0x10;

    // Bit to indicate that this is a keep-alive response.
    static constexpr uint8_t KeepAlive = 0x20;

    // Bit to indicate that this is the last packet in a series of packets.
    static constexpr uint8_t LastPacket = 0x40;

    // Bit to indicate that this is the first packet in a series of packets.
    static constexpr uint8_t FirstPacket = 0x80;

    // Bits to indicate that there is only a single packet in the response.
    static constexpr uint8_t SinglePacket = FirstPacket | LastPacket;

    // Bits to indicate that this is an ID packet.
    static constexpr uint8_t IdPacket = Timeout | AckNak | KeepAlive | SinglePacket;

private:
    uint8_t _rawHeader = 0x00U;
    bool _isNoop = false;
    bool _isIdPacket = false;
    bool _isFirstPacket = false;
    bool _isLastPacket = false;
    bool _isSinglePacket = false;
    bool _isStatus = false;
    bool _isStatusKeepAlive = false;
    bool _isStatusKeepAliveMarco = false;
    bool _isStatusKeepAlivePolo = false;
    bool _istatusAckNak = false;
    bool _istatusAck = false;
    bool _istatusNak = false;
    bool _isStatusTimeout = false;
    bool _isStatusTimeoutAck = false;
    bool _isStatusTimeoutQuery = false;
};

}
