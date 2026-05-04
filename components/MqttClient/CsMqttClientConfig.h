#pragma once
#include <cstdint>
#include <string>

namespace SignatureBridge::Mqtt {

struct CsMqttClientConfig
{
    std::string Host;
    uint16_t Port = 1883;
    std::string Username;
    std::string Password;
    std::string ClientId = "csi-ha-ble-bridge";
    bool AutoReconnect = true;
};

}
