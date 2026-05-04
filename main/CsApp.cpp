#include "CsApp.h"
#include "CsAppConfig.h"
#include "CsDeviceBootstrap.h"
#include "CsBleDeviceManager.h"
#include "CsWifiClient.h"
#include "CsMqttClient.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

using namespace SignatureBridge;
using namespace SignatureBridge::Config;
using namespace SignatureBridge::Devices;
using namespace SignatureBridge::Mqtt;
using namespace SignatureBridge::Wifi;

void CsApp::Initialize()
{
    ESP_LOGI(CsApp::Tag(), "Initializing application...");

    if (!StartWifi())
    {
        ESP_LOGE(CsApp::Tag(), "Failed to start WiFi; MQTT and HA publishing will be unavailable");
    }
    else if (!StartMqtt())
    {
        ESP_LOGE(CsApp::Tag(), "Failed to start MQTT; HA publishing will be unavailable");
    }

    CsDeviceBootstrap::BootstrapDevices();

    CsBleConnectionOptions options;
    options.AuthToken = AuthToken;

    if (TargetMacAddress != nullptr && TargetMacAddress[0] != '\0')
    {
        options.TargetMacAddress = TargetMacAddress;
    }
    else
    {
        options.TargetMacAddress.reset();
    }

    SetBleConnectionOptions(options);

    if (!_bleSessionManager.Initialize())
    {
        ESP_LOGE(CsApp::Tag(), "Failed to initialize BLE session manager");
        return;
    }

    ESP_LOGI(CsApp::Tag(), "Initialization complete");
    _initialized = true;
}

bool CsApp::StartWifi()
{
    CsWifiClientConfig wifiConfig;
    wifiConfig.Ssid = WifiSsid;
    wifiConfig.Password = WifiPassword;
    wifiConfig.MaxRetryCount = 8;

    return CsWifiClient::Instance().Start(wifiConfig);
}

bool CsApp::StartMqtt()
{
    CsMqttClientConfig mqttConfig;
    mqttConfig.Host = MqttHost;
    mqttConfig.Port = MqttPort;
    mqttConfig.Username = MqttUsername;
    mqttConfig.Password = MqttPassword;
    mqttConfig.ClientId = MqttClientId;

    if (!CsMqttClient::Instance().Start(mqttConfig))
    {
        return false;
    }

    if (!CsMqttClient::Instance().WaitForConnected(10000))
    {
        ESP_LOGW(CsApp::Tag(), "MQTT client did not connect before timeout; it may reconnect asynchronously");
        return true;
    }

    CsMqttClient::Instance().Publish(
        "signature_bridge/status",
        "{\"status\":\"online\"}",
        true
    );

    return true;
}

void CsApp::SetBleConnectionOptions(const CsBleConnectionOptions& options)
{
    CsBleDeviceManager::Instance().SetConnectionOptions(options);
}

void CsApp::Run()
{
    ESP_LOGI(CsApp::Tag(), "Running application...");

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void app_main(void)
{
    static CsApp app;
    app.Initialize();
    app.Run();
}
