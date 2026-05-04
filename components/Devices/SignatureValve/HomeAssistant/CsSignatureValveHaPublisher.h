#pragma once
#include "CsCore.h"
#include <functional>
#include <string>

namespace SignatureBridge::Devices::SignatureValve {
    class CsSignatureValveDevice;
}

namespace SignatureBridge::Devices::SignatureValve::HomeAssistant {

class CsSignatureValveHaPublisher
{
    CS_CLASS_TAG(CsSignatureValveHaPublisher)

public:
    using PublishHandler = std::function<bool(const std::string& topic, const std::string& payload, bool retain)>;

public:
    CsSignatureValveHaPublisher() = default;
    explicit CsSignatureValveHaPublisher(PublishHandler publishHandler);

public:
    void SetPublishHandler(PublishHandler publishHandler);
    bool HasPublishHandler() const;

public:
    bool PublishDiscovery(const CsSignatureValveDevice& device) const;
    bool PublishState(const CsSignatureValveDevice& device) const;
    bool PublishAvailability(const CsSignatureValveDevice& device, bool online) const;

public:
    void LogDiscoveryEntities(const CsSignatureValveDevice& device) const;
    void LogStateEntities(const CsSignatureValveDevice& device) const;

private:
    bool Publish(const std::string& topic, const std::string& payload, bool retain) const;

private:
    PublishHandler _publishHandler;
};

}
