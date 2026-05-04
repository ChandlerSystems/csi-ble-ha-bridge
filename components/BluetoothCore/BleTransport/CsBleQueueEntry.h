#pragma once
#include "CsBleQueueCompletionType.h"
#include <vector>

namespace SignatureBridge::BluetoothCore::BleTransport {

struct CsBleQueueEntry
{
    std::vector<uint8_t> Payload;
    std::vector<std::vector<uint8_t>> Packets;
    size_t PacketIndex = 0;
    uint8_t RetryCount = 0;
    CsBleQueueCompletionType CompletionType = CsBleQueueCompletionType::Ack;
};

}
