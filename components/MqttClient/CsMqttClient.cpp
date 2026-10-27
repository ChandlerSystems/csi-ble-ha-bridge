#include "CsMqttClient.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "esp_netif.h"

namespace SignatureBridge::Mqtt {

CsMqttClient& CsMqttClient::Instance()
{
    static CsMqttClient instance;
    return instance;
}

CsMqttClient::~CsMqttClient()
{
    Stop();
}

bool CsMqttClient::Start(const CsMqttClientConfig& config)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_started)
    {
        ESP_LOGW(CsMqttClient::Tag(), "MQTT client already started");
        return true;
    }

    if (config.Host.empty())
    {
        ESP_LOGE(CsMqttClient::Tag(), "MQTT host is empty");
        return false;
    }

    esp_netif_ip_info_t ipInfo;
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    if (netif != nullptr && esp_netif_get_ip_info(netif, &ipInfo) == ESP_OK)
    {
        ESP_LOGI(
            "Network",
            "IP=%d.%d.%d.%d Gateway=%d.%d.%d.%d Netmask=%d.%d.%d.%d",
            IP2STR(&ipInfo.ip),
            IP2STR(&ipInfo.gw),
            IP2STR(&ipInfo.netmask)
        );
    }

    esp_mqtt_client_config_t mqttConfig = {};
    mqttConfig.broker.address.hostname = config.Host.c_str();
    mqttConfig.broker.address.port = config.Port;
    mqttConfig.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
    mqttConfig.credentials.client_id = config.ClientId.empty() ? nullptr : config.ClientId.c_str();
    mqttConfig.credentials.username = config.Username.empty() ? nullptr : config.Username.c_str();
    mqttConfig.credentials.authentication.password = config.Password.empty() ? nullptr : config.Password.c_str();
    mqttConfig.network.disable_auto_reconnect = !config.AutoReconnect;

    _statusTopic = config.StatusTopic;

    if (!_statusTopic.empty())
    {
        mqttConfig.session.last_will.topic = _statusTopic.c_str();
        mqttConfig.session.last_will.msg = "offline";
        mqttConfig.session.last_will.qos = 1;
        mqttConfig.session.last_will.retain = 1;
    }

    _client = esp_mqtt_client_init(&mqttConfig);

    if (_client == nullptr)
    {
        ESP_LOGE(CsMqttClient::Tag(), "Failed to initialize MQTT client");
        return false;
    }

    esp_err_t err = esp_mqtt_client_register_event(
        _client,
        MQTT_EVENT_ANY,
        &CsMqttClient::HandleMqttEvent,
        this
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(CsMqttClient::Tag(), "Failed to register MQTT event handler [err=0x%x]", err);
        esp_mqtt_client_destroy(_client);
        _client = nullptr;
        return false;
    }

    err = esp_mqtt_client_start(_client);

    if (err != ESP_OK)
    {
        ESP_LOGE(CsMqttClient::Tag(), "Failed to start MQTT client [err=0x%x]", err);
        esp_mqtt_client_destroy(_client);
        _client = nullptr;
        return false;
    }

    _started = true;

    ESP_LOGI(
        CsMqttClient::Tag(),
        "MQTT client started [Host=%s] [Port=%u] [ClientId=%s]",
        config.Host.c_str(),
        static_cast<unsigned>(config.Port),
        config.ClientId.c_str()
    );

    return true;
}

void CsMqttClient::Stop()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_client == nullptr)
    {
        _started = false;
        _connected = false;
        return;
    }

    esp_mqtt_client_stop(_client);
    esp_mqtt_client_destroy(_client);
    _client = nullptr;
    _started = false;
    _connected = false;

    ESP_LOGI(CsMqttClient::Tag(), "MQTT client stopped");
}

bool CsMqttClient::IsStarted() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _started;
}

bool CsMqttClient::IsConnected() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _connected;
}

bool CsMqttClient::WaitForConnected(uint32_t timeoutMs) const
{
    const TickType_t delayTicks = pdMS_TO_TICKS(100);
    const TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeoutMs);

    while (xTaskGetTickCount() < deadline)
    {
        if (IsConnected())
        {
            return true;
        }

        vTaskDelay(delayTicks);
    }

    return IsConnected();
}

bool CsMqttClient::Publish(const std::string& topic, const std::string& payload, bool retain, int qos)
{
    esp_mqtt_client_handle_t client = nullptr;

    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_client == nullptr || !_connected)
        {
            ESP_LOGW(
                CsMqttClient::Tag(),
                "Skipping MQTT publish because client is not connected [Topic=%s]",
                topic.c_str()
            );

            return false;
        }

        client = _client;
    }

    const int messageId = esp_mqtt_client_publish(
        client,
        topic.c_str(),
        payload.c_str(),
        static_cast<int>(payload.size()),
        qos,
        retain ? 1 : 0
    );

    if (messageId < 0)
    {
        ESP_LOGE(
            CsMqttClient::Tag(),
            "MQTT publish failed [Topic=%s] [Retain=%s]",
            topic.c_str(),
            retain ? "true" : "false"
        );

        return false;
    }

    ESP_LOGD(
        CsMqttClient::Tag(),
        "MQTT publish queued [MessageId=%d] [Topic=%s] [Retain=%s]",
        messageId,
        topic.c_str(),
        retain ? "true" : "false"
    );

    return true;
}

void CsMqttClient::HandleMqttEvent(void* handlerArgs, esp_event_base_t, int32_t, void* eventData)
{
    auto* self = static_cast<CsMqttClient*>(handlerArgs);

    if (self == nullptr || eventData == nullptr)
    {
        return;
    }

    self->OnMqttEvent(static_cast<esp_mqtt_event_handle_t>(eventData));
}

void CsMqttClient::OnMqttEvent(esp_mqtt_event_handle_t event)
{
    if (event == nullptr)
    {
        return;
    }

    switch (event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
        {
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _connected = true;
                ESP_LOGI(CsMqttClient::Tag(), "MQTT connected");
            }

            if (!_statusTopic.empty())
            {
                esp_mqtt_client_enqueue(_client, _statusTopic.c_str(), "online", 0, 1, 1, true);
            }

            break;
        }

        case MQTT_EVENT_DISCONNECTED:
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _connected = false;
            ESP_LOGW(CsMqttClient::Tag(), "MQTT disconnected");
            break;
        }

        case MQTT_EVENT_ERROR:
            ESP_LOGW(CsMqttClient::Tag(), "MQTT event error");
            break;

        default:
            break;
    }
}

}
