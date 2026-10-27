#pragma once
#include "CsBlePacketTransport.h"
#include "CsBleQueueEntry.h"
#include "CsBleQueueCompletionType.h"
#include "BleUtilities/CsBleHostTimer.h"
#include <cstdint>
#include <deque>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleDeviceManagement {
    class CsBleDevice;
}

namespace SignatureBridge::BluetoothCore::BleTransport {

using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;

class CsBleQueueProcessor : private CsBlePacketTransport
{
    CS_CLASS_TAG(CsBleQueueProcessor)

public:
    explicit CsBleQueueProcessor(CsBleDevice& device);
    ~CsBleQueueProcessor() override;

public:
    bool EnqueueDataPayload(const std::vector<uint8_t>& payload, CsBleQueueCompletionType completionType);
    bool EnqueueRawPacket(const std::vector<uint8_t>& payload, CsBleQueueCompletionType completionType);

public:
    void Reset();
    bool IsBusy() const;
    size_t GetQueuedCount() const;
    void ProcessIncomingNotification(const uint8_t* buffer, size_t length);

protected:
    void OnPayloadReceived(const std::vector<uint8_t>& payload) override;
    void OnAckReceived() override;
    void OnNakReceived() override;
    void OnKeepAliveReceived(const CsBlePacketTransportHeader& header) override;
    void OnTimeoutAckReceived(const CsBlePacketTransportHeader& header) override;
    void OnTimeoutQueryReceived(const CsBlePacketTransportHeader& header) override;
    void OnDataPacketAccepted(const CsBlePacketTransportHeader& header) override;
    void OnCrcError(const CsBlePacketTransportHeader& header) override;
    void OnProtocolError(const CsBlePacketTransportHeader& header) override;
    void OnPacketDecoded(const std::vector<CsBlePacketChange>& changes) override;

private:
    bool EnqueuePackets(
        const std::vector<uint8_t>& payload,
        std::vector<std::vector<uint8_t>> packets,
        CsBleQueueCompletionType completionType
    );

private:
    void TryStartNextEntry();
    bool SendCurrentPacket();
    void CompleteCurrentEntry();
    void FailCurrentEntry(const char* reason);
    void RetryCurrentEntry(const char* reason);

private:
    void StartTimeoutTimer(uint32_t timeoutMs);
    void StopTimeoutTimer();
    void HandleTimeout();

private:
    static void TimeoutTimerCallback(void* arg);

private:
    static constexpr uint32_t AckTimeoutMs = 1500U;
    static constexpr uint32_t ResponseTimeoutMs = 3000U;
    static constexpr uint8_t MaxRetries = 4U;

private:
    CsBleDevice& _device;
    std::deque<CsBleQueueEntry> _queue;
    bool _hasActiveEntry = false;
    CsBleQueueEntry _activeEntry = {};
    BleUtilities::CsBleHostTimer _timeoutTimer{&CsBleQueueProcessor::TimeoutTimerCallback, this};
};

}
