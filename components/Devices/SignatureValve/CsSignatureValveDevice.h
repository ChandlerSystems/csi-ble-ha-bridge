#pragma once

#include "CsBleDevice.h"
#include "CsBleDeviceRegistry.h"
#include "CsSignatureValveAdvPayload.h"
#include "CsSignatureValveHaEntityDefinition.h"
#include "CsSignatureValveHaPublisher.h"
#include "CsSignatureValvePacketParser.h"
#include "CsSignatureValveState.h"
#include "CsSignatureValveWriteBuilder.h"

namespace SignatureBridge::Devices::SignatureValve {

using namespace BluetoothCore::BleDeviceManagement;
using namespace SignatureBridge::BluetoothCore::BleTransport;
using namespace SignatureBridge::Devices::SignatureValve::SignatureCore;
using namespace SignatureBridge::Devices::SignatureValve::HomeAssistant;

class CsSignatureValveWriter;

class CsSignatureValveDevice : public CsBleDevice
{
    CS_CLASS_TAG(CsSignatureValveDevice)

public:
    explicit CsSignatureValveDevice(const CsBleAdvertisement& advertisement);

public:
    static void Register();

public:
    bool SupportsHaEntity(const CsSignatureValveHaEntityDefinition& entity) const;
    bool TryGetHaEntityState(const CsSignatureValveHaEntityDefinition& entity, std::string& value) const;

public:
    CsSignatureValveState& State() { return _state; }
    const CsSignatureValveState& State() const { return _state; }
    CsSignatureValveWriter& Writer() { return *_writer; }
    CsSignatureValveHaCapability GetCapabilities() const { return _capabilities; }

public:
    CsBleDeviceType GetDeviceType() const override;
    std::string GetDeviceIdPrefix() const override;
    CsBleGattProfile GetGattProfile() const override;

protected:
    void OnAdvertisementUpdated() override;
    bool OnInitialize() override;
    void OnConnectFailed(int status) override;
    void OnDisconnected(int reason) override;
    void OnReady() override;
    void OnPayloadReceived(const std::vector<uint8_t>& payload) override;
    void OnPacketDecoded(const std::vector<CsBlePacketChange>& changes) override;

private:
    static CsSignatureValveAdvPayload ParseAdvertisementPayload(const std::vector<uint8_t>& combinedData);
    void HandlePacketTransportEvent(const CsBlePacketTransportEvent& event);
    bool UpdateHaCapabilities();

private:
    CsSignatureValveState _state;
    bool _haDiscoveryPublished = false;
    CsSignatureValveHaPublisher _haPublisher;
    CsSignatureValvePacketParser _packetParser;
    std::unique_ptr<CsSignatureValveWriter> _writer;
    CsSignatureValveAdvPayload _advertisementPayload = {};
    CsSignatureValveHaCapability _capabilities = CsSignatureValveHaCapability::None;
};

}
