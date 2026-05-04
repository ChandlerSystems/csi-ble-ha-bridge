#pragma once
#include "CsCore.h"
#include "CsMqttClientConfig.h"
#include <cstdint>
#include <mutex>
#include <string>

#include <mqtt_client.h>

namespace SignatureBridge::Mqtt {

class CsMqttClient
{
    CS_CLASS_TAG(CsMqttClient)

public:
    static CsMqttClient& Instance();

public:
    bool Start(const CsMqttClientConfig& config);
    void Stop();

public:
    bool IsStarted() const;
    bool IsConnected() const;
    bool WaitForConnected(uint32_t timeoutMs) const;
    bool Publish(const std::string& topic, const std::string& payload, bool retain = false, int qos = 0);

private:
    CsMqttClient() = default;
    CsMqttClient(const CsMqttClient&) = delete;
    CsMqttClient& operator=(const CsMqttClient&) = delete;
    ~CsMqttClient();

private:
    static void HandleMqttEvent(void* handlerArgs, esp_event_base_t base, int32_t eventId, void* eventData);
    void OnMqttEvent(esp_mqtt_event_handle_t event);

private:
    mutable std::mutex _mutex;
    esp_mqtt_client_handle_t _client = nullptr;
    bool _started = false;
    bool _connected = false;
};

}
