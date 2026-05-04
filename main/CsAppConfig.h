#pragma once
#include <cstdint>
#include <string>

namespace SignatureBridge::Config
{
    constexpr const char* WifiSsid = "CHANGE_ME"; // Example: "Tell My WiFi Love Her"
    constexpr const char* WifiPassword = "CHANGE_ME"; // Example: "Lorem Ipsum Noelle, hello world?"

    constexpr const char* MqttHost = "CHANGE_ME"; // Example: "192.168.4.179"
    constexpr int MqttPort = 1883;
    constexpr const char* MqttUsername = "CHANGE_ME"; // Example: "mqtt"
    constexpr const char* MqttPassword = "CHANGE_ME"; // Example: "correct horse battery staple"
    constexpr const char* MqttClientId = "csi-ha-ble-bridge";

    constexpr const char* HaDiscoveryPrefix = "homeassistant";
    constexpr const char* HaBaseTopic = "csi_ble_ha_bridge";

    constexpr const char* AuthToken = "CHANGE_ME"; // Example: "F8DF7EEA93B94D8F8E1C7C4F97844DD0"
    constexpr const char* TargetMacAddress = "";
}
