#pragma once
#include "CsCore.h"
#include "CsWifiClientConfig.h"
#include <cstdint>
#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_timer.h>

namespace SignatureBridge::Wifi {

class CsWifiClient
{
    CS_CLASS_TAG(CsWifiClient)

public:
    static CsWifiClient& Instance();

public:
    bool Start(const CsWifiClientConfig& config);
    void Stop();

public:
    bool IsStarted() const;
    bool IsConnected() const;

private:
    CsWifiClient() = default;
    CsWifiClient(const CsWifiClient&) = delete;
    CsWifiClient& operator=(const CsWifiClient&) = delete;
    ~CsWifiClient();

private:
    static void HandleWifiEvent(void* handlerArgs, esp_event_base_t eventBase, int32_t eventId, void* eventData);
    static void HandleIpEvent(void* handlerArgs, esp_event_base_t eventBase, int32_t eventId, void* eventData);

private:
    bool EnsurePlatformInitialized();
    void OnWifiEvent(esp_event_base_t eventBase, int32_t eventId, void* eventData);
    void OnIpEvent(esp_event_base_t eventBase, int32_t eventId, void* eventData);
    void ScheduleReconnect();
    static void ReconnectTimerCallback(void* arg);

private:
    static constexpr EventBits_t ConnectedBit = BIT0;
    static constexpr EventBits_t FailedBit = BIT1;
    static constexpr uint32_t MinReconnectDelayMs = 5000U;
    static constexpr uint32_t MaxReconnectDelayMs = 60000U;

private:
    bool _started = false;
    bool _connected = false;
    uint8_t _retryCount = 0;
    uint8_t _maxRetryCount = 8;
    EventGroupHandle_t _eventGroup = nullptr;
    esp_netif_t* _stationNetif = nullptr;
    esp_event_handler_instance_t _wifiEventHandler = nullptr;
    esp_event_handler_instance_t _ipEventHandler = nullptr;
    esp_timer_handle_t _reconnectTimer = nullptr;
    uint32_t _reconnectDelayMs = MinReconnectDelayMs;
};

}
