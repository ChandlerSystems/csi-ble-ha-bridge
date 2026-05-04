#include "CsBleQueueProcessor.h"
#include "BleCore/CsBleAckNakType.h"
#include "BleCore/CsBleTimeoutType.h"
#include "BleDeviceManagement/CsBleDevice.h"
#include <esp_log.h>

using namespace SignatureBridge::BluetoothCore::BleCore;
using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;
using namespace SignatureBridge::BluetoothCore::BleTransport;

CsBleQueueProcessor::CsBleQueueProcessor(CsBleDevice& device)
    : _device(device)
{
    const esp_timer_create_args_t timerArgs = {
        .callback = &CsBleQueueProcessor::TimeoutTimerCallback,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ble_queue_timeout",
        .skip_unhandled_events = true
    };

    const auto result = esp_timer_create(&timerArgs, &_timeoutTimer);

    if (result != ESP_OK)
    {
        ESP_LOGE(CsBleQueueProcessor::Tag(), "Failed to create timeout timer: %s", esp_err_to_name(result));
        _timeoutTimer = nullptr;
    }
}

CsBleQueueProcessor::~CsBleQueueProcessor()
{
    StopTimeoutTimer();

    if (_timeoutTimer != nullptr)
    {
        esp_timer_delete(_timeoutTimer);
        _timeoutTimer = nullptr;
    }
}

bool CsBleQueueProcessor::EnqueueDataPayload(
    const std::vector<uint8_t>& payload,
    CsBleQueueCompletionType completionType
)
{
    const auto packets = BuildDataPackets(payload, _device.GetNegotiatedMtu());
    return EnqueuePackets(payload, packets, completionType);
}

bool CsBleQueueProcessor::EnqueueRawPacket(
    const std::vector<uint8_t>& payload,
    CsBleQueueCompletionType completionType
)
{
    std::vector<std::vector<uint8_t>> packets;
    packets.push_back(BuildRawPacket(payload));
    return EnqueuePackets(payload, std::move(packets), completionType);
}

bool CsBleQueueProcessor::EnqueuePackets(
    const std::vector<uint8_t>& payload,
    std::vector<std::vector<uint8_t>> packets,
    CsBleQueueCompletionType completionType
)
{
    if (payload.empty())
    {
        ESP_LOGE(CsBleQueueProcessor::Tag(), "Cannot enqueue empty payload");
        return false;
    }

    if (packets.empty())
    {
        ESP_LOGE(CsBleQueueProcessor::Tag(), "Cannot enqueue empty packet list");
        return false;
    }

    CsBleQueueEntry entry;
    entry.Payload = payload;
    entry.Packets = std::move(packets);
    entry.CompletionType = completionType;

    _queue.push_back(std::move(entry));

    ESP_LOGI(
        Tag(),
        "Queued entry [Queued: %u] [Packets: %u]",
        static_cast<unsigned>(_queue.size()),
        static_cast<unsigned>(_queue.back().Packets.size())
    );

    TryStartNextEntry();
    return true;
}

void CsBleQueueProcessor::Reset()
{
    StopTimeoutTimer();
    CsBlePacketTransport::Reset();

    _queue.clear();
    _hasActiveEntry = false;
    _activeEntry = {};

    ESP_LOGI(CsBleQueueProcessor::Tag(), "Queue processor reset");
}

bool CsBleQueueProcessor::IsBusy() const
{
    return _hasActiveEntry || !_queue.empty();
}

void CsBleQueueProcessor::ProcessIncomingNotification(const uint8_t* buffer, size_t length)
{
    if (buffer == nullptr || length == 0U)
    {
        return;
    }

    ProcessPayload(buffer, length);
}

size_t CsBleQueueProcessor::GetQueuedCount() const
{
    return _queue.size() + (_hasActiveEntry ? 1U : 0U);
}

void CsBleQueueProcessor::TryStartNextEntry()
{
    if (_hasActiveEntry || _queue.empty())
    {
        return;
    }

    if (!_device.IsReadyForQueueProcessing())
    {
        return;
    }

    _activeEntry = std::move(_queue.front());
    _queue.pop_front();
    _hasActiveEntry = true;

    ESP_LOGI(
        Tag(),
        "Starting queue entry [Packets: %u]",
        static_cast<unsigned>(_activeEntry.Packets.size())
    );

    if (!SendCurrentPacket())
    {
        FailCurrentEntry("Failed to send packet");
    }
}

bool CsBleQueueProcessor::SendCurrentPacket()
{
    if (!_hasActiveEntry)
    {
        return false;
    }

    if (_activeEntry.PacketIndex >= _activeEntry.Packets.size())
    {
        return false;
    }

    const auto& packet = _activeEntry.Packets[_activeEntry.PacketIndex];

    if (!_device.WriteRaw(packet.data(), packet.size()))
    {
        return false;
    }

    const bool isLastPacket = (_activeEntry.PacketIndex + 1U) >= _activeEntry.Packets.size();

    switch (_activeEntry.CompletionType)
    {
        case CsBleQueueCompletionType::Immediate:
        {
            if (isLastPacket)
            {
                CompleteCurrentEntry();
            }
            else
            {
                ++_activeEntry.PacketIndex;
                if (!SendCurrentPacket())
                {
                    FailCurrentEntry("Failed to send next packet");
                }
            }

            break;
        }

        case CsBleQueueCompletionType::Ack:
            StartTimeoutTimer(AckTimeoutMs);
            break;

        case CsBleQueueCompletionType::Response:
            StartTimeoutTimer(isLastPacket ? ResponseTimeoutMs : AckTimeoutMs);
            break;
    }

    return true;
}

void CsBleQueueProcessor::CompleteCurrentEntry()
{
    StopTimeoutTimer();

    if (_hasActiveEntry)
    {
        _device.OnQueueEntryCompleted(_activeEntry.Payload);
    }

    _activeEntry = {};
    _hasActiveEntry = false;

    _device.OnQueueProcessorIdle();
    TryStartNextEntry();
}

void CsBleQueueProcessor::FailCurrentEntry(const char* reason)
{
    StopTimeoutTimer();

    if (_hasActiveEntry)
    {
        _device.OnQueueEntryFailed(_activeEntry.Payload, reason);
    }

    ESP_LOGE(CsBleQueueProcessor::Tag(), "Queue entry failed: %s", reason);

    _activeEntry = {};
    _hasActiveEntry = false;

    _device.OnQueueProcessorIdle();
    TryStartNextEntry();
}

void CsBleQueueProcessor::RetryCurrentEntry(const char* reason)
{
    if (!_hasActiveEntry)
    {
        return;
    }

    StopTimeoutTimer();

    if (_activeEntry.RetryCount >= MaxRetries)
    {
        FailCurrentEntry(reason);
        return;
    }

    ++_activeEntry.RetryCount;

    ESP_LOGW(
        Tag(),
        "Retrying queue entry [Retry: %u/%u] [%s]",
        static_cast<unsigned>(_activeEntry.RetryCount),
        static_cast<unsigned>(MaxRetries),
        reason
    );

    if (!SendCurrentPacket())
    {
        FailCurrentEntry("Retry send failed");
    }
}

void CsBleQueueProcessor::StartTimeoutTimer(uint32_t timeoutMs)
{
    if (_timeoutTimer == nullptr)
    {
        return;
    }

    esp_timer_stop(_timeoutTimer);

    const auto result = esp_timer_start_once(_timeoutTimer, static_cast<uint64_t>(timeoutMs) * 1000ULL);

    if (result != ESP_OK)
    {
        ESP_LOGE(CsBleQueueProcessor::Tag(), "Failed to start timeout timer: %s", esp_err_to_name(result));
    }
}

void CsBleQueueProcessor::StopTimeoutTimer()
{
    if (_timeoutTimer != nullptr)
    {
        esp_timer_stop(_timeoutTimer);
    }
}

void CsBleQueueProcessor::HandleTimeout()
{
    if (!_hasActiveEntry)
    {
        return;
    }

    RetryCurrentEntry("Timeout waiting for packet completion");
}

void CsBleQueueProcessor::TimeoutTimerCallback(void* arg)
{
    auto* processor = static_cast<CsBleQueueProcessor*>(arg);

    if (processor == nullptr)
    {
        return;
    }

    processor->HandleTimeout();
}

void CsBleQueueProcessor::OnPayloadReceived(const std::vector<uint8_t>& payload)
{
    _device.OnPayloadReceived(payload);

    if (_hasActiveEntry && _activeEntry.CompletionType == CsBleQueueCompletionType::Response)
    {
        CompleteCurrentEntry();
    }
}

void CsBleQueueProcessor::OnAckReceived()
{
    if (!_hasActiveEntry)
    {
        ESP_LOGW(CsBleQueueProcessor::Tag(), "Ignoring ACK with no active entry");
        return;
    }

    StopTimeoutTimer();

    const bool isLastPacket = (_activeEntry.PacketIndex + 1U) >= _activeEntry.Packets.size();

    if (!isLastPacket)
    {
        ++_activeEntry.PacketIndex;

        if (!SendCurrentPacket())
        {
            FailCurrentEntry("Failed to send next packet after ACK");
        }

        return;
    }

    if (_activeEntry.CompletionType == CsBleQueueCompletionType::Response)
    {
        StartTimeoutTimer(ResponseTimeoutMs);
        return;
    }

    CompleteCurrentEntry();
}

void CsBleQueueProcessor::OnNakReceived()
{
    RetryCurrentEntry("Received NAK");
}

void CsBleQueueProcessor::OnKeepAliveReceived(const CsBlePacketTransportHeader& header)
{
    if (header.IsStatusKeepAliveMarco())
    {
        ESP_LOGI(CsBleQueueProcessor::Tag(), "Keep-alive Marco received; responding with Polo");

        const auto polo = BuildKeepAlivePacket(CsBleKeepAliveType::Polo);
        _device.WriteRaw(polo.data(), polo.size());
        return;
    }

    if (header.IsStatusKeepAlivePolo())
    {
        ESP_LOGI(CsBleQueueProcessor::Tag(), "Keep-alive Polo received");
        return;
    }

    ESP_LOGW(CsBleQueueProcessor::Tag(), "Keep-alive received with unknown subtype");
}

void CsBleQueueProcessor::OnTimeoutAckReceived(const CsBlePacketTransportHeader& header)
{
    ARG_UNUSED(header);
    ESP_LOGW(CsBleQueueProcessor::Tag(), "Timeout ACK received");
}

void CsBleQueueProcessor::OnTimeoutQueryReceived(const CsBlePacketTransportHeader& header)
{
    ARG_UNUSED(header);
    ESP_LOGW(CsBleQueueProcessor::Tag(), "Timeout query received");

    const auto timeoutAck = BuildTimeoutPacket(CsBleTimeoutType::Ack);
    _device.WriteRaw(timeoutAck.data(), timeoutAck.size());
}

void CsBleQueueProcessor::OnDataPacketAccepted(const CsBlePacketTransportHeader& header)
{
    ARG_UNUSED(header);

    const auto ack = BuildAckNakPacket(CsBleAckNakType::Ack);
    _device.WriteRaw(ack.data(), ack.size());
}

void CsBleQueueProcessor::OnCrcError(const CsBlePacketTransportHeader& header)
{
    (void)header;

    ESP_LOGW(CsBleQueueProcessor::Tag(), "CRC error received");
    const auto nak = BuildAckNakPacket(CsBleAckNakType::Nak);
    _device.WriteRaw(nak.data(), nak.size());
}

void CsBleQueueProcessor::OnProtocolError(const CsBlePacketTransportHeader& header)
{
    (void)header;
    ESP_LOGW(CsBleQueueProcessor::Tag(), "Protocol error received");
}

void CsBleQueueProcessor::OnPacketDecoded(const std::vector<CsBlePacketChange>& changes)
{
    _device.OnPacketDecoded(changes);
}
