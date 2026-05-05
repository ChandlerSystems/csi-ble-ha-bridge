#include "Common/CsDeviceOnOffState.h"
#include "Common/CsDeviceSaltSensorStatus.h"
#include "BleUtilities/CsBleUtilities.h"
#include "CsCoreUtils.h"
#include "CsSignatureValveHaStateValueResolver.h"
#include "CsSignatureValveDevice.h"
#include "CsSignatureValveSeries.h"
#include "CsSignatureValveType.h"
#include "CsSignatureValveWriter.h"
#include "CsMqttClient.h"
#include <esp_log.h>

using namespace SignatureBridge::BluetoothCore::BleUtilities;
using namespace SignatureBridge::Core;
using namespace SignatureBridge::Devices::Common;
using namespace SignatureBridge::Devices::SignatureValve;
using namespace SignatureBridge::Devices::SignatureValve::HomeAssistant;
using namespace SignatureBridge::Mqtt;

namespace {
    static constexpr size_t ExpectedAdvPayloadLength = 8;
    static constexpr const char* DeviceIdentifierPrefix = "C2_";
}

CsSignatureValveDevice::CsSignatureValveDevice(const CsBleAdvertisement& advertisement)
{
    UpdateAdvertisement(advertisement);
    _writer = std::make_unique<CsSignatureValveWriter>(*this);

    _haPublisher.SetPublishHandler(
        [](const std::string& topic, const std::string& payload, bool retain)
        {
            return CsMqttClient::Instance().Publish(topic, payload, retain);
        }
    );
}

void CsSignatureValveDevice::Register()
{
    CsBleDeviceRegistry::Instance().Register(
        DeviceIdentifierPrefix,
        [](const CsBleAdvertisement& advertisement) -> std::unique_ptr<CsBleDevice>
        {
            return std::make_unique<CsSignatureValveDevice>(advertisement);
        }
    );
}

bool CsSignatureValveDevice::SupportsHaEntity(const CsSignatureValveHaEntityDefinition& entity) const
{
    return entity.RequiredCapabilities == CsSignatureValveHaCapability::None ||
           HasCapability(_capabilities, entity.RequiredCapabilities);
}

CsBleDeviceType CsSignatureValveDevice::GetDeviceType() const
{
    return CsBleDeviceType::SignatureValve;
}

std::string CsSignatureValveDevice::GetDeviceIdPrefix() const
{
    return DeviceIdentifierPrefix;
}

CsBleGattProfile CsSignatureValveDevice::GetGattProfile() const
{
    return CsBleGattProfile {
        .ServiceUuid = "a725458c-bee1-4d2e-9555-edf5a8082303",
        .ReadCharacteristicUuid = "a725458c-bee2-4d2e-9555-edf5a8082303",
        .WriteCharacteristicUuid = "a725458c-bee3-4d2e-9555-edf5a8082303",
    };
}

void CsSignatureValveDevice::OnAdvertisementUpdated()
{
    const auto& combinedData = GetCombinedAdvertisementData();

    _advertisementPayload = ParseAdvertisementPayload(combinedData);

    if (!_advertisementPayload.IsValid)
    {
        return;
    }

    const CsBleAdvertisement& advertisement = GetAdvertisement();

    ESP_LOGI(
        CsSignatureValveDevice::Tag(),
        "Advertisement Payload [%s] [%s]: %s",
        advertisement.Address.c_str(),
        advertisement.Name.c_str(),
        CsCoreUtils::Instance().BytesToHexAsciiString(combinedData).c_str()
    );
}

bool CsSignatureValveDevice::OnInitialize()
{
    ESP_LOGI(CsSignatureValveDevice::Tag(), "Initializing Signature Valve device...");

    const std::vector<uint8_t> idPacket = { 0xEA };

    if (!QueueRawPacket(idPacket, BleTransport::CsBleQueueCompletionType::Response))
    {
        ESP_LOGE(CsSignatureValveDevice::Tag(), "Failed to queue ID packet");
        return false;
    }

    ESP_LOGI(CsSignatureValveDevice::Tag(), "ID packet queued [0xEA]");
    return true;
}

void CsSignatureValveDevice::OnConnectFailed(int status)
{
    ESP_LOGE(CsSignatureValveDevice::Tag(), "Connect failed [Status=%d]", status);
}

void CsSignatureValveDevice::OnDisconnected(int reason)
{
    ESP_LOGW(CsSignatureValveDevice::Tag(), "Disconnected [Reason=%d]", reason);
    _haPublisher.PublishAvailability(*this, false);
}

void CsSignatureValveDevice::OnReady()
{
    ESP_LOGI(CsSignatureValveDevice::Tag(), "Signature Valve is ready");
}

void CsSignatureValveDevice::OnPayloadReceived(const std::vector<uint8_t>& payload)
{
    ESP_LOGI(
        CsSignatureValveDevice::Tag(),
        "Payload received [Length: %u]: %s",
        static_cast<unsigned>(payload.size()),
        CsCoreUtils::Instance().BytesToHexAsciiString(payload).c_str()
    );
}

void CsSignatureValveDevice::OnPacketDecoded(const std::vector<CsBlePacketChange>& changes)
{
    ESP_LOGI(
        CsSignatureValveDevice::Tag(),
        "Packet decoded [%u Changes]",
        static_cast<unsigned>(changes.size())
    );

    for (const auto& change : changes)
    {
        ESP_LOGI(
            CsSignatureValveDevice::Tag(),
            " - [Key: %s] [Type: %s] [Value: %s]",
            change.Key.c_str(),
            CsBlePacketValue::ToString(change.Value.Type).c_str(),
            CsBlePacketValue::ToDebugString(change.Value).c_str()
        );
    }

    if (!_packetParser.ApplyChanges(_state, changes))
    {
        ESP_LOGW(CsSignatureValveDevice::Tag(), "Failed to apply packet changes to state.");
        return;
    }

    const bool capabilitiesChanged = UpdateHaCapabilities();

    if (!_state.GlDataFullySent)
    {
        ESP_LOGD(
            CsSignatureValveDevice::Tag(),
            "Skipping Home Assistant publishing until initial valve data is fully sent"
        );

        return;
    }

    if (capabilitiesChanged || !_haDiscoveryPublished)
    {
        ESP_LOGI(CsSignatureValveDevice::Tag(), "Publishing Home Assistant discovery");

        if (_haPublisher.PublishDiscovery(*this))
        {
            _haDiscoveryPublished = true;
        }
    }

    _haPublisher.PublishAvailability(*this, true);
    _haPublisher.PublishState(*this);
}

CsSignatureValveAdvPayload CsSignatureValveDevice::ParseAdvertisementPayload(const std::vector<uint8_t>& combinedData)
{
    CsSignatureValveAdvPayload payload;

    if (combinedData.size() < ExpectedAdvPayloadLength)
    {
        return payload;
    }

    const auto bits = CsCoreUtils::Instance().ToBits(combinedData[0]);

    ESP_LOGD(
        CsSignatureValveDevice::Tag(),
        "Parsing advertisement payload bits: %s",
        CsCoreUtils::Instance().BytesToHexAsciiString(combinedData).c_str()
    );

    payload.StatusBits = combinedData[2];
    payload.ErrorState = combinedData[3];
    payload.TimeHours = combinedData[4];
    payload.TimeMinutes = combinedData[5];
    payload.ValveType = CsSignatureValveTypeTryFromInt(combinedData[6]);
    payload.Series = CsSignatureValveSeriesTryFromInt(combinedData[7]);
    payload.FirmwareMajor = combinedData[combinedData.size() - 2U];
    payload.FirmwareMinor = combinedData[combinedData.size() - 1U];
    payload.saltSensorStatus = CsDeviceSaltSensorStatusTryFromInt(bits[0]);
    payload.waterState = CsDeviceOnOffStateTryFromInt(bits[1]);
    payload.bypassState = CsDeviceOnOffStateTryFromInt(bits[2]);
    payload.IsValid = true;

    return payload;
}

void CsSignatureValveDevice::HandlePacketTransportEvent(const CsBlePacketTransportEvent& event)
{
    ESP_LOGW(CsSignatureValveDevice::Tag(), "XXX");

    if (event.Type != CsBlePacketTransportEventType::PacketDecoded)
    {
        ESP_LOGW(
            CsSignatureValveDevice::Tag(),
            "[HandlePacketTransportEvent] Unsupported event type: %u",
            static_cast<unsigned>(event.Type)
        );
        return;
    }

    if (!_packetParser.ApplyChanges(_state, event.Changes))
    {
        ESP_LOGW(
            CsSignatureValveDevice::Tag(),
            "[HandlePacketTransportEvent] Failed to apply packet changes to state."
        );

        return;
    }

    const bool capabilitiesChanged = UpdateHaCapabilities();

    if (capabilitiesChanged)
    {
        ESP_LOGI(CsSignatureValveDevice::Tag(), "Home Assistant capabilities changed");
    }

    _haPublisher.PublishDiscovery(*this);
    _haPublisher.PublishState(*this);
}

bool CsSignatureValveDevice::UpdateHaCapabilities()
{
    CsSignatureValveHaCapability caps = CsSignatureValveHaCapability::None;

    if (CsSignatureValveTypeIsMeteredSoftener(_state.GenDeviceType, _state.DlIsTwinValve))
    {
        caps = caps | CsSignatureValveHaCapability::IsSoftener;
    }

    if (CsSignatureValveTypeIsAerationValve(_state.GenDeviceType, _state.DlIsTwinValve))
    {
        caps = caps | CsSignatureValveHaCapability::IsAerationFilter;
    }

    if (CsSignatureValveTypeIsCommercial(_state.GenDeviceType, _state.DlIsTwinValve))
    {
        caps = caps | CsSignatureValveHaCapability::IsCommercial;
    }

    if (CsSignatureValveTypeIsSdValve(_state.GenDeviceType, _state.DlIsTwinValve))
    {
        caps = caps | CsSignatureValveHaCapability::IsSd;
    }

    if (CsSignatureValveTypeIsSdMeteredSoftener(_state.GenDeviceType, _state.DlIsTwinValve))
    {
        caps = caps | CsSignatureValveHaCapability::IsSdMeteredSoftener;
    }

    if (CsSignatureValveTypeHasAerationCycle(_state.GenDeviceType, _state.DlIsTwinValve))
    {
        caps = caps | CsSignatureValveHaCapability::HasAerationCycle;
    }

    if (CsSignatureValveTypeHasSaltDose(_state.GenDeviceType, _state.DlIsTwinValve))
    {
        caps = caps | CsSignatureValveHaCapability::HasSaltDose;
    }

    if (caps == _capabilities)
    {
        return false;
    }

    _capabilities = caps;

    ESP_LOGI(
        CsSignatureValveDevice::Tag(),
        "Home Assistant capabilities updated [0x%08X]",
        static_cast<uint32_t>(_capabilities)
    );

    return true;
}

bool CsSignatureValveDevice::TryGetHaEntityState(
    const HomeAssistant::CsSignatureValveHaEntityDefinition& entity,
    std::string& value
) const
{
    return HomeAssistant::CsSignatureValveHaStateValueResolver::TryResolve(
        _state,
        entity.PacketKey,
        value
    );
}
