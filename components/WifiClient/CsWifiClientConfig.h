#pragma once
#include <cstdint>
#include <string>

namespace SignatureBridge::Wifi {

struct CsWifiClientConfig
{
    std::string Ssid;
    std::string Password;
    uint8_t MaxRetryCount = 8;
};

}
