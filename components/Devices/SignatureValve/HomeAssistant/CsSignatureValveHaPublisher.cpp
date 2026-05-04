#include "CsSignatureValveHaEntities.h"
#include "CsSignatureValveHaPublisher.h"
#include "CsSignatureValveDevice.h"
#include <algorithm>
#include <cctype>
#include <esp_log.h>
#include <sstream>

namespace SignatureBridge::Devices::SignatureValve::HomeAssistant {

using namespace SignatureBridge::Devices::SignatureValve;

namespace {
    static constexpr const char* DiscoveryPrefix = "homeassistant";
    static constexpr const char* BaseTopic = "csi_ble_ha_bridge";
    static constexpr const char* DeviceIdPrefix = "signature_valve";
    static constexpr const char* Manufacturer = "Chandler Systems, Inc.";
    static constexpr const char* Model = "Signature Valve";
}

static std::string JsonEscape(const std::string& value)
{
    std::string output;
    output.reserve(value.size() + 8U);

    for (const char ch : value)
    {
        switch (ch)
        {
            case '\\': output += "\\\\"; break;
            case '"': output += "\\\""; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                output += ch;
                break;
        }
    }

    return output;
}

static std::string SanitizeTopicPart(const std::string& value)
{
    std::string output;
    output.reserve(value.size());

    bool lastWasUnderscore = false;

    for (const char ch : value)
    {
        const auto c = static_cast<unsigned char>(ch);

        if (std::isalnum(c))
        {
            output += static_cast<char>(std::tolower(c));
            lastWasUnderscore = false;
            continue;
        }

        if (!lastWasUnderscore)
        {
            output += '_';
            lastWasUnderscore = true;
        }
    }

    while (!output.empty() && output.front() == '_')
    {
        output.erase(output.begin());
    }

    while (!output.empty() && output.back() == '_')
    {
        output.pop_back();
    }

    return output.empty() ? "unknown" : output;
}

static const char* ToComponent(const CsSignatureValveHaEntityType type)
{
    switch (type)
    {
        case CsSignatureValveHaEntityType::BinarySensor:
            return "binary_sensor";

        case CsSignatureValveHaEntityType::Sensor:
        default:
            return "sensor";
    }
}

static std::string BuildDeviceId(const CsSignatureValveDevice& device)
{
    const auto& advertisement = device.GetAdvertisement();

    if (!advertisement.Address.empty())
    {
        return std::string(DeviceIdPrefix) + "_" + SanitizeTopicPart(advertisement.Address);
    }

    if (!advertisement.Name.empty())
    {
        return std::string(DeviceIdPrefix) + "_" + SanitizeTopicPart(advertisement.Name);
    }

    return DeviceIdPrefix;
}

static std::string BuildDeviceName(const CsSignatureValveDevice& device)
{
    const auto& advertisement = device.GetAdvertisement();

    if (!advertisement.Name.empty())
    {
        return advertisement.Name;
    }

    return "Signature Valve";
}

static std::string BuildStateTopic(const std::string& deviceId)
{
    return std::string(BaseTopic) + "/" + deviceId + "/state";
}

static std::string BuildAvailabilityTopic(const std::string& deviceId)
{
    return std::string(BaseTopic) + "/" + deviceId + "/availability";
}

static std::string BuildDiscoveryTopic(
    const std::string& deviceId,
    const CsSignatureValveHaEntityDefinition& entity
)
{
    return std::string(DiscoveryPrefix) + "/" + ToComponent(entity.Type) + "/" + deviceId + "/" +
           SanitizeTopicPart(entity.ObjectId) + "/config";
}

static std::string BuildUniqueId(
    const std::string& deviceId,
    const CsSignatureValveHaEntityDefinition& entity
)
{
    return deviceId + "_" + SanitizeTopicPart(entity.ObjectId);
}

static std::string BuildDiscoveryPayload(
    const CsSignatureValveDevice& device,
    const CsSignatureValveHaEntityDefinition& entity
)
{
    const std::string deviceId = BuildDeviceId(device);
    const std::string stateTopic = BuildStateTopic(deviceId);
    const std::string availabilityTopic = BuildAvailabilityTopic(deviceId);
    const std::string uniqueId = BuildUniqueId(deviceId, entity);
    const std::string objectId = uniqueId;
    const std::string deviceName = BuildDeviceName(device);

    std::ostringstream json;

    json << "{";
    json << "\"name\":\"" << JsonEscape(entity.Name) << "\",";
    json << "\"object_id\":\"" << JsonEscape(objectId) << "\",";
    json << "\"unique_id\":\"" << JsonEscape(uniqueId) << "\",";
    json << "\"state_topic\":\"" << JsonEscape(stateTopic) << "\",";
    json << "\"value_template\":\"{{ value_json." << JsonEscape(SanitizeTopicPart(entity.ObjectId)) << " }}\",";
    json << "\"availability_topic\":\"" << JsonEscape(availabilityTopic) << "\",";
    json << "\"payload_available\":\"online\",";
    json << "\"payload_not_available\":\"offline\",";

    if (entity.Type == CsSignatureValveHaEntityType::BinarySensor)
    {
        json << "\"payload_on\":\"ON\",";
        json << "\"payload_off\":\"OFF\",";
    }

    json << "\"device\":{";
    json << "\"identifiers\":[\"" << JsonEscape(deviceId) << "\"],";
    json << "\"name\":\"" << JsonEscape(deviceName) << "\",";
    json << "\"manufacturer\":\"" << Manufacturer << "\",";
    json << "\"model\":\"" << Model << "\"";
    json << "}";
    json << "}";

    return json.str();
}

static std::string BuildStatePayload(const CsSignatureValveDevice& device)
{
    size_t count = 0;
    const auto* entities = GetSignatureValveHaEntities(count);

    std::ostringstream json;
    json << "{";

    bool wroteAny = false;

    for (size_t i = 0; i < count; ++i)
    {
        const auto& entity = entities[i];

        if (!device.SupportsHaEntity(entity))
        {
            continue;
        }

        std::string value;

        if (entity.CustomResolver != nullptr)
        {
            if (!entity.CustomResolver(device.State(), value))
            {
                ESP_LOGD(
                    CsSignatureValveHaPublisher::Tag(),
                    "Skipping HA state [ObjectId: %s] [Name: %s] [Reason: Custom resolver returned no value]",
                    entity.ObjectId,
                    entity.Name
                );

                continue;
            }
        }
        else if (!device.TryGetHaEntityState(entity, value))
        {
            ESP_LOGD(
                CsSignatureValveHaPublisher::Tag(),
                "Skipping HA state [ObjectId: %s] [Name: %s] [Reason: No value]",
                entity.ObjectId,
                entity.Name
            );

            continue;
        }

        if (wroteAny)
        {
            json << ",";
        }

        json << "\"" << JsonEscape(SanitizeTopicPart(entity.ObjectId)) << "\":\"" << JsonEscape(value) << "\"";
        wroteAny = true;
    }

    json << "}";

    return json.str();
}

CsSignatureValveHaPublisher::CsSignatureValveHaPublisher(PublishHandler publishHandler) :
    _publishHandler(std::move(publishHandler))
{
}

void CsSignatureValveHaPublisher::SetPublishHandler(PublishHandler publishHandler)
{
    _publishHandler = std::move(publishHandler);
}

bool CsSignatureValveHaPublisher::HasPublishHandler() const
{
    return static_cast<bool>(_publishHandler);
}

bool CsSignatureValveHaPublisher::PublishDiscovery(const CsSignatureValveDevice& device) const
{
    size_t count = 0;
    const auto* entities = GetSignatureValveHaEntities(count);
    const std::string deviceId = BuildDeviceId(device);

    bool success = true;

    ESP_LOGI(CsSignatureValveHaPublisher::Tag(), "Publishing Home Assistant discovery entities");

    for (size_t i = 0; i < count; ++i)
    {
        const auto& entity = entities[i];

        if (!device.SupportsHaEntity(entity))
        {
            ESP_LOGD(
                CsSignatureValveHaPublisher::Tag(),
                "Skipping HA discovery [ObjectId: %s] [Name: %s]",
                entity.ObjectId,
                entity.Name
            );

            continue;
        }

        const std::string topic = BuildDiscoveryTopic(deviceId, entity);
        const std::string payload = BuildDiscoveryPayload(device, entity);

        ESP_LOGI(
            CsSignatureValveHaPublisher::Tag(),
            "Publishing HA discovery [Topic: %s] [Name: %s]",
            topic.c_str(),
            entity.Name
        );

        success = Publish(topic, payload, true) && success;
    }

    return success;
}

bool CsSignatureValveHaPublisher::PublishState(const CsSignatureValveDevice& device) const
{
    const std::string deviceId = BuildDeviceId(device);
    const std::string topic = BuildStateTopic(deviceId);
    const std::string payload = BuildStatePayload(device);

    ESP_LOGI(
        CsSignatureValveHaPublisher::Tag(),
        "Publishing HA state [Topic: %s] [Payload: %s]",
        topic.c_str(),
        payload.c_str()
    );

    return Publish(topic, payload, false);
}

bool CsSignatureValveHaPublisher::PublishAvailability(const CsSignatureValveDevice& device, bool online) const
{
    const std::string deviceId = BuildDeviceId(device);
    const std::string topic = BuildAvailabilityTopic(deviceId);
    const std::string payload = online ? "online" : "offline";

    ESP_LOGI(
        CsSignatureValveHaPublisher::Tag(),
        "Publishing HA availability [Topic: %s] [Payload: %s]",
        topic.c_str(),
        payload.c_str()
    );

    return Publish(topic, payload, true);
}

void CsSignatureValveHaPublisher::LogDiscoveryEntities(const CsSignatureValveDevice& device) const
{
    size_t count = 0;
    const auto* entities = GetSignatureValveHaEntities(count);

    ESP_LOGI(CsSignatureValveHaPublisher::Tag(), "Home Assistant discovery entities:");

    for (size_t i = 0; i < count; ++i)
    {
        const auto& entity = entities[i];

        if (!device.SupportsHaEntity(entity))
        {
            ESP_LOGD(
                CsSignatureValveHaPublisher::Tag(),
                "Skipping HA entity [ObjectId: %s] [Name: %s]",
                entity.ObjectId,
                entity.Name
            );

            continue;
        }

        ESP_LOGI(
            CsSignatureValveHaPublisher::Tag(),
            "HA entity [ObjectId: %s] [Name: %s] [Type: %d]",
            entity.ObjectId,
            entity.Name,
            static_cast<int>(entity.Type)
        );
    }
}

void CsSignatureValveHaPublisher::LogStateEntities(const CsSignatureValveDevice& device) const
{
    ESP_LOGI(
        CsSignatureValveHaPublisher::Tag(),
        "Home Assistant state payload: %s",
        BuildStatePayload(device).c_str()
    );
}

bool CsSignatureValveHaPublisher::Publish(const std::string& topic, const std::string& payload, bool retain) const
{
    if (!_publishHandler)
    {
        ESP_LOGW(
            CsSignatureValveHaPublisher::Tag(),
            "MQTT publish handler not configured [Topic: %s] [Retain: %s] [Payload: %s]",
            topic.c_str(),
            retain ? "true" : "false",
            payload.c_str()
        );

        return false;
    }

    return _publishHandler(topic, payload, retain);
}

}
