#include "CsBlePacketTransportHeader.h"

using namespace SignatureBridge::BluetoothCore::BleTransport;

CsBlePacketTransportHeader::CsBlePacketTransportHeader(uint8_t header)
{
    _rawHeader = header;
    _isNoop = (header == Nop);
    _isIdPacket = (header == IdPacket);
    _isFirstPacket = ((header & FirstPacket) != 0);
    _isLastPacket = ((header & LastPacket) != 0);
    _isSinglePacket = ((header & SinglePacket) == SinglePacket);
    _isStatus = ((header & (KeepAlive | AckNak | Timeout)) != 0);
    _isStatusKeepAlive = ((header & KeepAlive) != 0);
    _isStatusKeepAlivePolo = _isStatusKeepAlive && ((header & KeepAliveType) != 0);
    _isStatusKeepAliveMarco = _isStatusKeepAlive && !_isStatusKeepAlivePolo;
    _istatusAckNak = ((header & AckNak) != 0);
    _istatusAck = _istatusAckNak && ((header & AckNakType) != 0);
    _istatusNak = _istatusAckNak && !_istatusAck;
    _isStatusTimeout = ((header & Timeout) != 0);
    _isStatusTimeoutAck = _isStatusTimeout && ((header & TimeoutType) != 0);
    _isStatusTimeoutQuery = _isStatusTimeout && !_isStatusTimeoutAck;
}
