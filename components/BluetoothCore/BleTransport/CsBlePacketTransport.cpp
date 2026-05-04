#include "CsBlePacketTransport.h"
#include "CsCoreUtils.h"
#include "CsCrc16.h"
#include <cJSON.h>
#include <cstring>
#include <esp_log.h>
#include <string>

using namespace SignatureBridge::BluetoothCore::BleTransport;
using namespace SignatureBridge::Core;

CsBlePacketTransport::CsBlePacketTransport()
{
    Reset();
}

void CsBlePacketTransport::Reset()
{
    ResetReassembly();
}

void CsBlePacketTransport::SetEventHandler(CsBlePacketTransportEventHandler handler)
{
    _eventHandler = std::move(handler);
}

void CsBlePacketTransport::RaiseEvent(const CsBlePacketTransportEvent& event)
{
    if (_eventHandler)
    {
        _eventHandler(event);
    }
}

void CsBlePacketTransport::OnNotificationReceived(const uint8_t* buffer, size_t length)
{
    if (buffer == nullptr || length == 0U)
    {
        return;
    }

    CsBlePacketTransportEvent event;
    event.Type = CsBlePacketTransportEventType::PacketReceived;

    RaiseEvent(event);
    ProcessPayload(buffer, length);
}

void CsBlePacketTransport::ProcessPayload(const uint8_t* buffer, const size_t length)
{
    CsBlePacketTransportEvent event;

    if (TryHandleStatusPacket(buffer, length, event))
    {
        DispatchEvent(event);
        RaiseEvent(event);
        return;
    }

    if (TryHandleDataPacket(buffer, length, event))
    {
        DispatchEvent(event);
        RaiseEvent(event);

        if (event.Type == CsBlePacketTransportEventType::PayloadComplete)
        {
            CsBlePacketTransportEvent decodedEvent;
            decodedEvent.Type = CsBlePacketTransportEventType::PacketDecoded;
            decodedEvent.Header = event.Header;
            decodedEvent.Payload = event.Payload;

            if (TryDecodePayload(decodedEvent.Payload, decodedEvent.Changes))
            {
                DispatchEvent(decodedEvent);
                RaiseEvent(decodedEvent);
            }
            else
            {
                CsBlePacketTransportEvent errorEvent;
                errorEvent.Type = CsBlePacketTransportEventType::ProtocolError;
                errorEvent.Header = event.Header;

                DispatchEvent(errorEvent);
                RaiseEvent(errorEvent);
            }
        }

        return;
    }

    CsBlePacketTransportEvent errorEvent;
    errorEvent.Type = CsBlePacketTransportEventType::ProtocolError;
    errorEvent.Header = CsBlePacketTransportHeader(
        (buffer != nullptr && length > 0U) ? buffer[0] : CsBlePacketTransportHeader::Nop
    );

    DispatchEvent(errorEvent);
    RaiseEvent(errorEvent);
}

bool CsBlePacketTransport::TryHandleStatusPacket(
    const uint8_t* buffer,
    const size_t length,
    CsBlePacketTransportEvent& event
)
{
    if (buffer == nullptr || length != CsBlePacketHeaderSize)
    {
        return false;
    }

    CsBlePacketTransportHeader header = CsBlePacketTransportHeader(buffer[0]);

    if (header.IsNoop() || !header.IsStatus())
    {
        return false;
    }

    event.Header = header;

    if (header.IsStatusAck())
    {
        event.Type = CsBlePacketTransportEventType::Ack;
        return true;
    }

    if (header.IsStatusNak())
    {
        event.Type = CsBlePacketTransportEventType::Nak;
        return true;
    }

    if (header.IsStatusKeepAlive())
    {
        event.Type = CsBlePacketTransportEventType::KeepAlive;
        return true;
    }

    if (header.IsStatusTimeoutAck())
    {
        event.Type = CsBlePacketTransportEventType::TimeoutAck;
        return true;
    }

    if (header.IsStatusTimeoutQuery())
    {
        event.Type = CsBlePacketTransportEventType::TimeoutQuery;
        return true;
    }

    event.Type = CsBlePacketTransportEventType::ProtocolError;
    return true;
}

bool CsBlePacketTransport::TryHandleDataPacket(
    const uint8_t* buffer,
    const size_t length,
    CsBlePacketTransportEvent& event
)
{
    if (buffer == nullptr || length < CsBlePacketOverhead)
    {
        return false;
    }

    const CsBlePacketTransportHeader header(buffer[0]);

    if (header.IsStatus() || header.IsIdPacket())
    {
        return false;
    }

    if (!ValidatePacketCrc(buffer, length))
    {
        event.Type = CsBlePacketTransportEventType::CrcError;
        event.Header = header;
        return true;
    }

    const size_t payloadOffset = CsBlePacketHeaderSize;
    const size_t payloadLength = length - payloadOffset - CsBlePacketCrcSize;
    const uint8_t* payload = &buffer[payloadOffset];

    event.Header = header;

    if (header.IsSinglePacket())
    {
        OnDataPacketAccepted(header);
        event.Type = CsBlePacketTransportEventType::PayloadComplete;
        event.Payload.assign(payload, payload + payloadLength);
        return true;
    }

    if (header.IsFirstPacket())
    {
        ResetReassembly();
        OnDataPacketAccepted(header);

        _isReassembling = true;
        _reassemblyBuffer.insert(_reassemblyBuffer.end(), payload, payload + payloadLength);

        if (header.IsLastPacket())
        {
            event.Type = CsBlePacketTransportEventType::PayloadComplete;
            event.Payload = _reassemblyBuffer;
            ResetReassembly();
        }
        else
        {
            event.Type = CsBlePacketTransportEventType::None;
        }

        return true;
    }

    if (!_isReassembling)
    {
        event.Type = CsBlePacketTransportEventType::ProtocolError;
        return true;
    }

    _reassemblyBuffer.insert(_reassemblyBuffer.end(), payload, payload + payloadLength);
    OnDataPacketAccepted(header);

    if (header.IsLastPacket())
    {
        event.Type = CsBlePacketTransportEventType::PayloadComplete;
        event.Payload = _reassemblyBuffer;
        ResetReassembly();
    }
    else
    {
        event.Type = CsBlePacketTransportEventType::None;
    }

    return true;
}

bool CsBlePacketTransport::ValidatePacketCrc(const uint8_t* buffer, const size_t length) const
{
    if (buffer == nullptr || length < CsBlePacketOverhead)
    {
        return false;
    }

    const uint16_t expectedCrc = CsCoreUtils::Instance().ReadLittleEndianU16(
        &buffer[length - CsBlePacketCrcSize],
        CsBlePacketCrcSize
    );

    const uint16_t actualCrc = CsCrc16::Instance().Compute(buffer, length - CsBlePacketCrcSize);

    if (expectedCrc != actualCrc)
    {
        ESP_LOGW(
            CsBlePacketTransport::Tag(),
            "CRC validation failed [Expected CRC: 0x%04X] [Actual CRC: 0x%04X]",
            expectedCrc,
            actualCrc
        );
    }

    return expectedCrc == actualCrc;
}

std::vector<uint8_t> CsBlePacketTransport::BuildAckNakPacket(CsBleAckNakType type) const
{
    const uint8_t header = CsBlePacketTransportHeader::SinglePacket | CsBlePacketTransportHeader::AckNak;

    const uint8_t ackNakType = (type == CsBleAckNakType::Ack) ?
        CsBlePacketTransportHeader::AckNakType :
        CsBlePacketTransportHeader::Nop;

    return { static_cast<uint8_t>(header | ackNakType) };
}

std::vector<uint8_t> CsBlePacketTransport::BuildKeepAlivePacket(CsBleKeepAliveType type) const
{
    const uint8_t header = CsBlePacketTransportHeader::SinglePacket | CsBlePacketTransportHeader::KeepAlive;

    const uint8_t keepAliveType = (type == CsBleKeepAliveType::Polo) ?
        CsBlePacketTransportHeader::KeepAliveType :
        CsBlePacketTransportHeader::Nop;

    return { static_cast<uint8_t>(header | keepAliveType) };
}

std::vector<uint8_t> CsBlePacketTransport::BuildTimeoutPacket(CsBleTimeoutType type) const
{
    const uint8_t header = CsBlePacketTransportHeader::SinglePacket | CsBlePacketTransportHeader::Timeout;

    const uint8_t timeoutType = (type == CsBleTimeoutType::Ack) ?
        CsBlePacketTransportHeader::TimeoutType :
        CsBlePacketTransportHeader::Nop;

    return { static_cast<uint8_t>(header | timeoutType) };
}

std::vector<uint8_t> CsBlePacketTransport::BuildRawPacket(const std::vector<uint8_t>& payload) const
{
    return payload;
}

std::vector<std::vector<uint8_t>> CsBlePacketTransport::BuildDataPackets(
    const std::vector<uint8_t>& payload,
    size_t negotiatedMtu
) const
{
    const size_t maxPayloadPerPacket = GetMaxPayloadPerPacket(negotiatedMtu);
    std::vector<std::vector<uint8_t>> packets;
    size_t offset = 0U;

    if (payload.empty() || maxPayloadPerPacket == 0U)
    {
        return packets;
    }


    while (offset < payload.size())
    {
        const size_t remaining = payload.size() - offset;
        const size_t chunkLength = std::min(maxPayloadPerPacket, remaining);
        const bool isFirst = (offset == 0U);
        const bool isLast = (offset + chunkLength >= payload.size());

        packets.push_back(BuildPacketChunk(
            &payload[offset],
            chunkLength,
            isFirst,
            isLast
        ));

        offset += chunkLength;
    }

    return packets;
}

size_t CsBlePacketTransport::GetMaxPayloadPerPacket(size_t negotiatedMtu) const
{
    return (negotiatedMtu > CsBlePacketOverhead) ? (negotiatedMtu - CsBlePacketOverhead) : 0U;
}

std::vector<uint8_t> CsBlePacketTransport::BuildPacketChunk(
    const uint8_t* buffer,
    size_t length,
    bool isFirst,
    bool isLast
) const
{
    std::vector<uint8_t> packet;
    uint8_t header = 0U;

    header |= isFirst ? CsBlePacketTransportHeader::FirstPacket : CsBlePacketTransportHeader::Nop;
    header |= isLast ? CsBlePacketTransportHeader::LastPacket : CsBlePacketTransportHeader::Nop;

    packet.reserve(CsBlePacketHeaderSize + length + CsBlePacketCrcSize);
    packet.push_back(header);
    packet.insert(packet.end(), buffer, buffer + length);

    const uint16_t crc = CsCrc16::Instance().Compute(packet.data(), packet.size());
    packet.resize(packet.size() + CsBlePacketCrcSize);
    CsCoreUtils::Instance().WriteLittleEndianU16(&packet[packet.size() - CsBlePacketCrcSize], crc);

    return packet;
}

void CsBlePacketTransport::ResetReassembly()
{
    _isReassembling = false;
    _reassemblyBuffer.clear();
}

void CsBlePacketTransport::DispatchEvent(const CsBlePacketTransportEvent& event)
{
    switch (event.Type)
    {
        case CsBlePacketTransportEventType::Ack:
            OnAckReceived();
            break;

        case CsBlePacketTransportEventType::Nak:
            OnNakReceived();
            break;

        case CsBlePacketTransportEventType::KeepAlive:
            OnKeepAliveReceived(event.Header);
            break;

        case CsBlePacketTransportEventType::TimeoutAck:
            OnTimeoutAckReceived(event.Header);
            break;

        case CsBlePacketTransportEventType::TimeoutQuery:
            OnTimeoutQueryReceived(event.Header);
            break;

        case CsBlePacketTransportEventType::PayloadComplete:
            OnPayloadReceived(event.Payload);
            break;

        case CsBlePacketTransportEventType::PacketReceived:
            break;

        case CsBlePacketTransportEventType::PacketDecoded:
            OnPacketDecoded(event.Changes);
            break;

        case CsBlePacketTransportEventType::CrcError:
            OnCrcError(event.Header);
            break;

        case CsBlePacketTransportEventType::ProtocolError:
            OnProtocolError(event.Header);
            break;

        case CsBlePacketTransportEventType::None:
        default:
            break;
    }
}

bool CsBlePacketTransport::TryDecodePayload(
    const std::vector<uint8_t>& payload,
    std::vector<CsBlePacketChange>& changes
) const
{
    changes.clear();

    if (payload.empty())
    {
        return false;
    }

    const std::string jsonText(payload.begin(), payload.end());

    cJSON* root = cJSON_Parse(jsonText.c_str());

    if (root == nullptr)
    {
        ESP_LOGW(CsBlePacketTransport::Tag(), "Failed to parse JSON payload");
        return false;
    }

    if (!cJSON_IsObject(root))
    {
        ESP_LOGW(CsBlePacketTransport::Tag(), "JSON payload root is not an object");
        cJSON_Delete(root);
        return false;
    }

    cJSON* item = nullptr;

    cJSON_ArrayForEach(item, root)
    {
        if (item == nullptr || item->string == nullptr)
        {
            continue;
        }

        CsBlePacketChange change;
        change.Key = item->string;

        if (!TryDecodeJsonValue(item, change.Value))
        {
            ESP_LOGW(CsBlePacketTransport::Tag(), "Failed to decode JSON value for key: %s", item->string);
            continue;
        }

        changes.push_back(std::move(change));
    }

    cJSON_Delete(root);

    return !changes.empty();
}

bool CsBlePacketTransport::TryDecodeJsonValue(const cJSON* item, CsBlePacketValue& value) const
{
    if (item == nullptr)
    {
        return false;
    }

    value = {};

    if (cJSON_IsNumber(item))
    {
        value.Type = CsBlePacketValueType::Int;
        value.IntValue = item->valueint;
        return true;
    }

    if (cJSON_IsBool(item))
    {
        value.Type = CsBlePacketValueType::Int;
        value.IntValue = cJSON_IsTrue(item) ? 1 : 0;
        return true;
    }

    if (cJSON_IsString(item))
    {
        value.Type = CsBlePacketValueType::ByteArray;

        if (item->valuestring != nullptr)
        {
            const auto* text = reinterpret_cast<const uint8_t*>(item->valuestring);
            const size_t length = std::strlen(item->valuestring);
            value.ByteArrayValue.assign(text, text + length);
        }

        return true;
    }

    if (cJSON_IsArray(item))
    {
        const int count = cJSON_GetArraySize(item);

        value.Type = CsBlePacketValueType::Array;
        value.ArrayValue.reserve(static_cast<size_t>(count));

        for (int i = 0; i < count; ++i)
        {
            const cJSON* child = cJSON_GetArrayItem(item, i);

            CsBlePacketValue childValue;

            if (!TryDecodeJsonValue(child, childValue))
            {
                return false;
            }

            value.ArrayValue.push_back(std::move(childValue));
        }

        return true;
    }

    if (cJSON_IsObject(item))
    {
        value.Type = CsBlePacketValueType::Object;
        cJSON* child = nullptr;

        cJSON_ArrayForEach(child, item)
        {
            if (child == nullptr || child->string == nullptr)
            {
                continue;
            }

            CsBlePacketObjectEntry entry;
            entry.Key = child->string;

            if (!TryDecodeJsonValue(child, entry.Value))
            {
                return false;
            }

            value.ObjectValue.emplace_back(child->string, std::move(entry.Value));
        }

        return true;
    }

    value.Type = CsBlePacketValueType::Unknown;
    return false;
}
