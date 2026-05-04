#include "CsBleDeviceManager.h"
#include "CsBleDeviceRegistry.h"
#include "CsBleSessionManager.h"
#include "CsBleUtilities.h"
#include "CsBleScanner.h"
#include <esp_log.h>

using namespace SignatureBridge::BluetoothCore;
using namespace SignatureBridge::BluetoothCore::BleScanner;
using namespace SignatureBridge::BluetoothCore::BleUtilities;
using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;

CsBleDeviceManager& CsBleDeviceManager::Instance()
{
    static CsBleDeviceManager instance;
    return instance;
}

void CsBleDeviceManager::SetSessionManager(CsBleSessionManager* session)
{
    _sessionManager = session;
}

void CsBleDeviceManager::SetConnectionOptions(const CsBleConnectionOptions& options)
{
    _connectionOptions = options;

    ESP_LOGI(
        CsBleDeviceManager::Tag(),
        "Connection options updated [Has Auth Token: %s] [Target MAC Address: %s]",
        _connectionOptions.HasAuthToken() ? "true" : "false",
        _connectionOptions.HasTargetMacAddress() ? _connectionOptions.TargetMacAddress->c_str() : "<none>"
    );
}

const CsBleConnectionOptions& CsBleDeviceManager::GetConnectionOptions() const
{
    return _connectionOptions;
}

bool CsBleDeviceManager::ShouldConnectToAdvertisement(const ble_gap_disc_desc* discovery) const
{
    if (discovery == nullptr)
    {
        ESP_LOGW(CsBleDeviceManager::Tag(), "Cannot evaluate null discovery result");
        return false;
    }

    if (!_connectionOptions.HasTargetMacAddress())
    {
        return false;
    }

    return DoesAdvertisedDeviceMatchTarget(discovery);
}

bool CsBleDeviceManager::DoesAdvertisedDeviceMatchTarget(const ble_gap_disc_desc* discovery) const
{
    if (discovery == nullptr || !_connectionOptions.HasTargetMacAddress())
    {
        return false;
    }

    const std::string advertisedAddress = CsBleUtilities::Instance().FormatBleAddress(&discovery->addr);

    if (advertisedAddress == *_connectionOptions.TargetMacAddress)
    {
        ESP_LOGI(CsBleDeviceManager::Tag(), "Target device matched [%s]", advertisedAddress.c_str());
        return true;
    }

    ESP_LOGI(
        CsBleDeviceManager::Tag(),
        "Ignoring compatible device [%s], waiting for target [%s]",
        advertisedAddress.c_str(),
        _connectionOptions.TargetMacAddress->c_str()
    );

    return false;
}

void CsBleDeviceManager::OnAdvertisement(const CsBleAdvertisement& advertisement, const ble_gap_disc_desc* discovery)
{
    if (advertisement.Address.empty() || discovery == nullptr)
    {
        return;
    }

    auto it = _devices.find(advertisement.Address);

    if (it == _devices.end())
    {
        auto device = CsBleDeviceRegistry::Instance().TryCreate(advertisement);

        if (!device)
        {
            return;
        }

        ESP_LOGI(CsBleDeviceManager::Tag(), "New device discovered: %s", advertisement.Name.c_str());

        device->UpdateAdvertisement(advertisement);
        CsBleDevice* devicePtr = device.get();
        _devices.emplace(advertisement.Address, std::move(device));

        if (_connectionOptions.HasTargetMacAddress())
        {
            TryConnectToTarget(devicePtr, discovery);
        }
        else
        {
            TryConnectToFirstMatch();
        }
    }
    else
    {
        it->second->UpdateAdvertisement(advertisement);

        if (_connectionOptions.HasTargetMacAddress())
        {
            TryConnectToTarget(it->second.get(), discovery);
        }
        else
        {
            TryConnectToFirstMatch();
        }
    }
}

void CsBleDeviceManager::TryConnectToTarget(CsBleDevice* device, const ble_gap_disc_desc* discovery)
{
    if (device == nullptr || discovery == nullptr)
    {
        return;
    }

    if (!_connectionOptions.HasAuthToken())
    {
        ESP_LOGW(
            CsBleDeviceManager::Tag(),
            "Skipping target connection attempt because authentication token is not configured"
        );
        return;
    }

    if (!ShouldConnectToAdvertisement(discovery))
    {
        return;
    }

    if (!device->IsConnected() && !device->IsConnecting())
    {
        ESP_LOGI(
            CsBleDeviceManager::Tag(),
            "Attempting connection to target device %s",
            device->GetAdvertisement().Address.c_str()
        );

        if (_sessionManager != nullptr)
        {
            _sessionManager->RequestConnection(device, _connectionOptions);
        }
    }
}

void CsBleDeviceManager::TryConnectToFirstMatch()
{
    if (_sessionManager == nullptr)
    {
        return;
    }

    if (!_connectionOptions.HasAuthToken())
    {
        ESP_LOGW(
            CsBleDeviceManager::Tag(),
            "Skipping connection attempt because authentication token is not configured"
        );
        return;
    }

    for (auto& [address, device] : _devices)
    {
        if (!device->IsConnected() && !device->IsConnecting())
        {
            ESP_LOGI(CsBleDeviceManager::Tag(), "Attempting connection to %s", address.c_str());
            _sessionManager->RequestConnection(device.get(), _connectionOptions);
            return;
        }
    }
}
