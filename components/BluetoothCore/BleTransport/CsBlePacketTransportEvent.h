#pragma once
#include "CsBlePacketChange.h"
#include "CsBlePacketTransportEventType.h"
#include "CsBlePacketTransportHeader.h"
#include <vector>

namespace SignatureBridge::BluetoothCore::BleTransport {

using namespace SignatureBridge::BluetoothCore::BleCore;

struct CsBlePacketTransportEvent
{
    CsBlePacketTransportEventType Type = CsBlePacketTransportEventType::None;
    CsBlePacketTransportHeader Header = CsBlePacketTransportHeader(CsBlePacketTransportHeader::Nop);
    std::vector<uint8_t> Payload;
    std::vector<CsBlePacketChange> Changes;
};

}
