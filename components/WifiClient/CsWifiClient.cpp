#include "CsWifiClient.h"
#include <algorithm>
#include <cstring>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

namespace SignatureBridge::Wifi {

namespace {
    constexpr size_t WifiSsidCapacity = 32;
    constexpr size_t WifiPasswordCapacity = 64;
}

CsWifiClient& CsWifiClient::Instance()
{
    static CsWifiClient instance;
    return instance;
}

CsWifiClient::~CsWifiClient()
{
    Stop();
}

bool CsWifiClient::EnsurePlatformInitialized()
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(CsWifiClient::Tag(), "NVS needs erase before init [Error: 0x%x]", err);

        err = nvs_flash_erase();
        if (err != ESP_OK)
        {
            ESP_LOGE(CsWifiClient::Tag(), "Failed to erase NVS [Error: 0x%x]", err);
            return false;
        }

        err = nvs_flash_init();
    }

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to initialize NVS [Error: 0x%x]", err);
        return false;
    }

    err = esp_netif_init();

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to initialize esp_netif [Error: 0x%x]", err);
        return false;
    }

    err = esp_event_loop_create_default();

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to create default event loop [Error: 0x%x]", err);
        return false;
    }

    return true;
}

bool CsWifiClient::Start(const CsWifiClientConfig& config)
{
    if (_started)
    {
        ESP_LOGW(CsWifiClient::Tag(), "WiFi client already started");
        return true;
    }

    if (config.Ssid.empty())
    {
        ESP_LOGE(CsWifiClient::Tag(), "WiFi SSID is empty");
        return false;
    }

    if (config.Ssid.size() >= WifiSsidCapacity)
    {
        ESP_LOGE(CsWifiClient::Tag(), "WiFi SSID is too long");
        return false;
    }

    if (config.Password.size() >= WifiPasswordCapacity)
    {
        ESP_LOGE(CsWifiClient::Tag(), "WiFi password is too long");
        return false;
    }

    if (!EnsurePlatformInitialized())
    {
        return false;
    }

    _eventGroup = xEventGroupCreate();

    if (_eventGroup == nullptr)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to create WiFi event group");
        return false;
    }

    _stationNetif = esp_netif_create_default_wifi_sta();

    if (_stationNetif == nullptr)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to create default WiFi station netif");
        vEventGroupDelete(_eventGroup);
        _eventGroup = nullptr;
        return false;
    }

    wifi_init_config_t initConfig = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t err = esp_wifi_init(&initConfig);

    if (err != ESP_OK)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to initialize WiFi [Error: 0x%x]", err);
        return false;
    }

    err = esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &CsWifiClient::HandleWifiEvent,
        this,
        &_wifiEventHandler
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to register WiFi event handler [Error: 0x%x]", err);
        return false;
    }

    err = esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &CsWifiClient::HandleIpEvent,
        this,
        &_ipEventHandler
    );

    if (err != ESP_OK)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to register IP event handler [Error: 0x%x]", err);
        return false;
    }

    wifi_config_t wifiConfig = {};
    std::memcpy(wifiConfig.sta.ssid, config.Ssid.c_str(), config.Ssid.size());
    std::memcpy(wifiConfig.sta.password, config.Password.c_str(), config.Password.size());
    wifiConfig.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifiConfig.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    _maxRetryCount = config.MaxRetryCount;
    _retryCount = 0;
    _connected = false;

    err = esp_wifi_set_mode(WIFI_MODE_STA);

    if (err != ESP_OK)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to set WiFi mode [Error: 0x%x]", err);
        return false;
    }

    err = esp_wifi_set_config(WIFI_IF_STA, &wifiConfig);

    if (err != ESP_OK)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to set WiFi config [Error: 0x%x]", err);
        return false;
    }

    err = esp_wifi_start();

    if (err != ESP_OK)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to start WiFi [Error: 0x%x]", err);
        return false;
    }

    _started = true;
    ESP_LOGI(CsWifiClient::Tag(), "Connecting to WiFi [SSID=%s]", config.Ssid.c_str());

    EventBits_t bits = xEventGroupWaitBits(
        _eventGroup,
        ConnectedBit | FailedBit,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(30000)
    );

    if ((bits & ConnectedBit) != 0)
    {
        ESP_LOGI(CsWifiClient::Tag(), "WiFi connected");
        return true;
    }

    if ((bits & FailedBit) != 0)
    {
        ESP_LOGE(CsWifiClient::Tag(), "Failed to connect to WiFi [SSID=%s]", config.Ssid.c_str());
        return false;
    }

    ESP_LOGE(CsWifiClient::Tag(), "Timed out waiting for WiFi connection [SSID=%s]", config.Ssid.c_str());
    return false;
}

void CsWifiClient::Stop()
{
    if (!_started && _eventGroup == nullptr)
    {
        return;
    }

    if (_ipEventHandler != nullptr)
    {
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, _ipEventHandler);
        _ipEventHandler = nullptr;
    }

    if (_wifiEventHandler != nullptr)
    {
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, _wifiEventHandler);
        _wifiEventHandler = nullptr;
    }

    if (_reconnectTimer != nullptr)
    {
        esp_timer_stop(_reconnectTimer);
        esp_timer_delete(_reconnectTimer);
        _reconnectTimer = nullptr;
    }

    if (_started)
    {
        esp_wifi_stop();
        esp_wifi_deinit();
    }

    if (_stationNetif != nullptr)
    {
        esp_netif_destroy_default_wifi(_stationNetif);
        _stationNetif = nullptr;
    }

    if (_eventGroup != nullptr)
    {
        vEventGroupDelete(_eventGroup);
        _eventGroup = nullptr;
    }

    _started = false;
    _connected = false;
    _retryCount = 0;
}

bool CsWifiClient::IsStarted() const
{
    return _started;
}

bool CsWifiClient::IsConnected() const
{
    return _connected;
}

void CsWifiClient::HandleWifiEvent(void* handlerArgs, esp_event_base_t eventBase, int32_t eventId, void* eventData)
{
    auto* self = static_cast<CsWifiClient*>(handlerArgs);

    if (self != nullptr)
    {
        self->OnWifiEvent(eventBase, eventId, eventData);
    }
}

void CsWifiClient::HandleIpEvent(void* handlerArgs, esp_event_base_t eventBase, int32_t eventId, void* eventData)
{
    auto* self = static_cast<CsWifiClient*>(handlerArgs);

    if (self != nullptr)
    {
        self->OnIpEvent(eventBase, eventId, eventData);
    }
}

void CsWifiClient::OnWifiEvent(esp_event_base_t, int32_t eventId, void*)
{
    if (eventId == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(CsWifiClient::Tag(), "WiFi station started");
        esp_wifi_connect();
        return;
    }

    if (eventId == WIFI_EVENT_STA_DISCONNECTED)
    {
        _connected = false;

        if (_retryCount < _maxRetryCount)
        {
            ++_retryCount;
            ESP_LOGW(
                CsWifiClient::Tag(),
                "WiFi disconnected; retrying [%u/%u]",
                static_cast<unsigned>(_retryCount),
                static_cast<unsigned>(_maxRetryCount)
            );
            esp_wifi_connect();
            return;
        }

        ESP_LOGE(CsWifiClient::Tag(), "WiFi connection failed after retries; retrying in the background");
        xEventGroupSetBits(_eventGroup, FailedBit);
        ScheduleReconnect();
    }
}

void CsWifiClient::ScheduleReconnect()
{
    if (_reconnectTimer == nullptr)
    {
        const esp_timer_create_args_t timerArgs = {
            .callback = &CsWifiClient::ReconnectTimerCallback,
            .arg = this,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "wifi_reconnect",
            .skip_unhandled_events = true
        };

        if (esp_timer_create(&timerArgs, &_reconnectTimer) != ESP_OK)
        {
            ESP_LOGE(CsWifiClient::Tag(), "Failed to create the WiFi reconnect timer");
            _reconnectTimer = nullptr;
            return;
        }
    }

    ESP_LOGW(CsWifiClient::Tag(), "Retrying WiFi in %lu ms", static_cast<unsigned long>(_reconnectDelayMs));

    esp_timer_stop(_reconnectTimer);
    esp_timer_start_once(_reconnectTimer, static_cast<uint64_t>(_reconnectDelayMs) * 1000ULL);

    _reconnectDelayMs = std::min(_reconnectDelayMs * 2U, MaxReconnectDelayMs);
}

void CsWifiClient::ReconnectTimerCallback(void* arg)
{
    auto* self = static_cast<CsWifiClient*>(arg);

    if (self != nullptr && self->_started && !self->_connected)
    {
        esp_wifi_connect();
    }
}

void CsWifiClient::OnIpEvent(esp_event_base_t, int32_t eventId, void* eventData)
{
    if (eventId != IP_EVENT_STA_GOT_IP || eventData == nullptr)
    {
        return;
    }

    auto* event = static_cast<ip_event_got_ip_t*>(eventData);
    _retryCount = 0;
    _reconnectDelayMs = MinReconnectDelayMs;
    _connected = true;

    ESP_LOGI(
        CsWifiClient::Tag(),
        "WiFi got IP [IP=" IPSTR "]",
        IP2STR(&event->ip_info.ip)
    );

    xEventGroupSetBits(_eventGroup, ConnectedBit);
}

}
