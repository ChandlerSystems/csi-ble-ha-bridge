#include "CsBleDevice.h"
#include "CsBleJsonWriteCommand.h"
#include "CsBleUtilities.h"
#include "CsCoreUtils.h"
#include "BleTransport/CsBleQueueProcessor.h"
#include <algorithm>
#include <esp_log.h>
#include <esp_timer.h>
#include <host/ble_gap.h>
#include <host/ble_gatt.h>
#include <host/ble_hs.h>

using namespace SignatureBridge::BluetoothCore;
using namespace SignatureBridge::BluetoothCore::BleUtilities;
using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;
using namespace SignatureBridge::Core;

CsBleDevice::CsBleDevice()
    : _queueProcessor(std::make_unique<BleTransport::CsBleQueueProcessor>(*this)) { }

CsBleDevice::~CsBleDevice()
{
    DestroyReconnectTimer();
}

void CsBleDevice::UpdateAdvertisement(const CsBleAdvertisement& advertisement)
{
    InitializeReconnectTimer();

    _advertisement = advertisement;

    _combinedAdvertisementData = CsBleUtilities::Instance().CombineAdvertisementPayloads(
        advertisement.ManufacturerData,
        advertisement.ScanResponseData
    );

    OnAdvertisementUpdated();
}

bool CsBleDevice::Connect(const bool autoReconnect)
{
    if (_advertisement.Address.empty())
    {
        ESP_LOGE(CsBleDevice::Tag(), "Cannot connect: address missing");
        return false;
    }

    if (!_authToken.has_value())
    {
        ESP_LOGE(CsBleDevice::Tag(), "Cannot connect: authentication token not configured");
        return false;
    }

    ble_addr_t address{};
    CsBleUtilities::Instance().ParseBleAddress(_advertisement.Address, &address);

    if (_advertisement.PrimaryAddressType != 0xFF)
    {
        address.type = _advertisement.PrimaryAddressType;
    }
    else
    {
        address.type = _advertisement.AddressType;
        ESP_LOGW(CsBleDevice::Tag(), "PrimaryAddressType missing; falling back to AddressType=%u", address.type);
    }

    _autoReconnectEnabled = autoReconnect;

    if (!_autoReconnectEnabled)
    {
        CancelReconnect();
    }

    SetConnectionState(CsBleConnectionState::Connecting);

    const int result = ble_gap_connect(
        BLE_OWN_ADDR_RANDOM,
        &address,
        30000,
        nullptr,
        CsBleDevice::BleGapEventCallback,
        this
    );

    if (result != ESUCCESS)
    {
        SetConnectionState(CsBleConnectionState::Idle);
        ESP_LOGE(CsBleDevice::Tag(), "Connection failed to start: %d", result);
        return false;
    }

    ESP_LOGI(CsBleDevice::Tag(), "Connecting to %s", _advertisement.Address.c_str());
    return true;
}

void CsBleDevice::Disconnect()
{
    _autoReconnectEnabled = false;
    CancelReconnect();

    if (_connHandle == BLE_HS_CONN_HANDLE_NONE)
    {
        return;
    }

    SendDisconnectCommand();
    vTaskDelay(pdMS_TO_TICKS(150));
    ble_gap_terminate(_connHandle, BLE_ERR_REM_USER_CONN_TERM);
}

bool CsBleDevice::WriteRaw(const uint8_t* buffer, size_t length)
{
    if (_resolvedGattProfile.WriteCharacteristic == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Write characteristic is not resolved");
        return false;
    }

    if (buffer == nullptr || length == 0)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Write payload is empty");
        return false;
    }

    const uint16_t valueHandle = _resolvedGattProfile.WriteCharacteristic->ValueHandle;

    const int result = ble_gattc_write_flat(
        _connHandle,
        valueHandle,
        buffer,
        static_cast<uint16_t>(length),
        nullptr,
        nullptr
    );

    if (result != 0)
    {
        ESP_LOGE(
            CsBleDevice::Tag(),
            "Write failed [Handle: %u] [Length: %u] [Result: %d]",
            valueHandle,
            static_cast<unsigned>(length),
            result
        );
        return false;
    }

    ESP_LOGI(
        CsBleDevice::Tag(),
        "[>>> Write] Transmitted [Handle: %u] [Length: %u]: %s",
        valueHandle,
        static_cast<unsigned>(length),
        CsCoreUtils::Instance().BytesToHexAsciiString(std::vector<uint8_t>(buffer, buffer + length)).c_str()
    );

    return true;
}

int CsBleDevice::BleGapEventCallback(struct ble_gap_event* event, void* arg)
{
    auto* device = static_cast<CsBleDevice*>(arg);

    if (device == nullptr)
    {
        return 0;
    }

    return device->BleGapEventHandler(event);
}

int CsBleDevice::BleGapEventHandler(struct ble_gap_event* event)
{
    if (event == nullptr)
    {
        return 0;
    }

    switch (event->type)
    {
        case BLE_GAP_EVENT_CONNECT:
        {
            if (event->connect.status == 0)
            {
                CancelReconnect();
                _connHandle = event->connect.conn_handle;
                _authState = CsBleAuthState::Unauthenticated;
                SetConnectionState(CsBleConnectionState::Connected);

                ESP_LOGI(CsBleDevice::Tag(), "Connected! [Connection Handle: %u]", _connHandle);

                OnConnected();
                AdvanceInitializationPipeline();
            }
            else
            {
                ESP_LOGE(CsBleDevice::Tag(), "Connect failed [Status: %d]", event->connect.status);
                ClearConnectionSession();
                SetConnectionState(CsBleConnectionState::Idle);
                OnConnectFailed(event->connect.status);

                if (ShouldAutoReconnect(event->connect.status))
                {
                    ScheduleReconnect();
                }
            }

            return 0;
        }

        case BLE_GAP_EVENT_DISCONNECT:
        {
            const int reason = event->disconnect.reason;

            ESP_LOGW(CsBleDevice::Tag(), "Disconnected [Reason: %d]", reason);

            ClearConnectionSession();
            SetConnectionState(CsBleConnectionState::Idle);
            OnDisconnected(reason);

            if (ShouldAutoReconnect(reason))
            {
                ScheduleReconnect();
            }

            return 0;
        }

        case BLE_GAP_EVENT_NOTIFY_RX:
        {
            const auto valueHandle = event->notify_rx.attr_handle;
            const auto* buffer = event->notify_rx.om->om_data;
            const auto length = event->notify_rx.om->om_len;

            const CsBleGattCharacteristic* matchedCharacteristic = nullptr;

            for (const auto& characteristic : _characteristics)
            {
                if (characteristic.ValueHandle == valueHandle)
                {
                    matchedCharacteristic = &characteristic;
                    break;
                }
            }

            if (matchedCharacteristic != nullptr)
            {
                ESP_LOGI(
                    CsBleDevice::Tag(),
                    "[<<<  Read] Received [Handle: %u] [Length: %u]: %s",
                    valueHandle,
                    static_cast<unsigned>(length),
                    CsCoreUtils::Instance().BytesToHexAsciiString(std::vector<uint8_t>(buffer, buffer + length)).c_str()
                );

                OnNotificationReceived(matchedCharacteristic, buffer, length);
            }

            return 0;
        }

        case BLE_GAP_EVENT_MTU:
        {
            _negotiatedMtu = event->mtu.value;
            ESP_LOGI(CsBleDevice::Tag(), "MTU updated: %u", _negotiatedMtu);
            return 0;
        }

        default:
            return 0;
    }
}

void CsBleDevice::OnNotificationReceived(
    const CsBleGattCharacteristic* characteristic,
    const uint8_t* buffer,
    size_t length
)
{
    ARG_UNUSED(characteristic);

    if (_queueProcessor == nullptr)
    {
        return;
    }

    _queueProcessor->ProcessIncomingNotification(buffer, length);
}

bool CsBleDevice::ResolveGattProfile()
{
    const auto profile = GetGattProfile();

    _resolvedGattProfile = {};
    _resolvedGattProfile.Service = FindServiceByUuidString(profile.ServiceUuid);

    if (_resolvedGattProfile.Service == nullptr)
    {
        ESP_LOGE(
            CsBleDevice::Tag(),
            "Could not find service %s for device %s",
            profile.ServiceUuid.c_str(),
            _advertisement.Name.c_str()
        );
        return false;
    }

    _resolvedGattProfile.ReadCharacteristic = FindCharacteristicByUuidString(profile.ReadCharacteristicUuid);

    if (_resolvedGattProfile.ReadCharacteristic == nullptr)
    {
        ESP_LOGE(
            CsBleDevice::Tag(),
            "Could not find read characteristic %s (service %s) for device %s",
            profile.ReadCharacteristicUuid.c_str(),
            profile.ServiceUuid.c_str(),
            _advertisement.Name.c_str()
        );
        return false;
    }

    _resolvedGattProfile.WriteCharacteristic = FindCharacteristicByUuidString(profile.WriteCharacteristicUuid);

    if (_resolvedGattProfile.WriteCharacteristic == nullptr)
    {
        ESP_LOGE(
            CsBleDevice::Tag(),
            "Could not find write characteristic %s (service %s) for device %s",
            profile.WriteCharacteristicUuid.c_str(),
            profile.ServiceUuid.c_str(),
            _advertisement.Name.c_str()
        );
        return false;
    }

    ESP_LOGI(
        CsBleDevice::Tag(),
        "Resolved GATT profile for %s [Service: %s] [Read: %u] [Write: %u]",
        _advertisement.Name.c_str(),
        profile.ServiceUuid.c_str(),
        _resolvedGattProfile.ReadCharacteristic->ValueHandle,
        _resolvedGattProfile.WriteCharacteristic->ValueHandle
    );

    return true;
}

bool CsBleDevice::SubscribeToReadCharacteristic()
{
    if (_resolvedGattProfile.ReadCharacteristic == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Read characteristic is not resolved");
        return false;
    }

    const auto properties = _resolvedGattProfile.ReadCharacteristic->Properties;
    const bool canNotify = (properties & BLE_GATT_CHR_F_NOTIFY) != 0;
    const bool canIndicate = (properties & BLE_GATT_CHR_F_INDICATE) != 0;

    if (!canNotify && !canIndicate)
    {
        ESP_LOGE(
            CsBleDevice::Tag(),
            "Read characteristic does not support notify/indicate [Properties: 0x%02X]",
            properties
        );
        return false;
    }

    ESP_LOGI(
        CsBleDevice::Tag(),
        "Read characteristic supports %s [Value: %u]",
        canNotify ? "notify" : "indicate",
        _resolvedGattProfile.ReadCharacteristic->ValueHandle
    );

    const uint16_t characteristicValueHandle = _resolvedGattProfile.ReadCharacteristic->ValueHandle;
    const uint16_t endHandle = _resolvedGattProfile.Service->EndHandle;

    const int result = ble_gattc_disc_all_dscs(
        _connHandle,
        characteristicValueHandle,
        endHandle,
        CsBleDevice::OnDescriptorDiscovered,
        this
    );

    if (result != 0)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Descriptor discovery failed to start: %d", result);
        return false;
    }

    return true;
}

void CsBleDevice::SetConnectionState(CsBleConnectionState newState)
{
    _connectionState = newState;
}

void CsBleDevice::ClearConnectionSession()
{
    if (_queueProcessor != nullptr)
    {
        _queueProcessor->Reset();
    }

    _initializationQueuedWorkPending = false;
    _connHandle = BLE_HS_CONN_HANDLE_NONE;
    _negotiatedMtu = CsBleMinMtu;
    _services.clear();
    _characteristics.clear();
    _characteristicDiscoveryServiceIndex = 0;
    _cccdHandle = 0;
    _resolvedGattProfile = {};
    _authState = CsBleAuthState::Unknown;
}

void CsBleDevice::InitializeReconnectTimer()
{
    if (_reconnectTimer != nullptr)
    {
        return;
    }

    const esp_timer_create_args_t timerArgs = {
        .callback = &CsBleDevice::ReconnectTimerCallback,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ble_reconnect",
        .skip_unhandled_events = true
    };

    const auto result = esp_timer_create(&timerArgs, &_reconnectTimer);

    if (result != ESP_OK)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Failed to create reconnect timer: %s", esp_err_to_name(result));
        _reconnectTimer = nullptr;
    }
}

void CsBleDevice::DestroyReconnectTimer()
{
    if (_reconnectTimer == nullptr)
    {
        return;
    }

    esp_timer_stop(_reconnectTimer);
    esp_timer_delete(_reconnectTimer);
    _reconnectTimer = nullptr;
}

void CsBleDevice::ScheduleReconnect()
{
    if (!_autoReconnectEnabled)
    {
        ESP_LOGI(CsBleDevice::Tag(), "Auto reconnect disabled");
        return;
    }

    if (_reconnectScheduled)
    {
        ESP_LOGI(CsBleDevice::Tag(), "Reconnect already scheduled");
        return;
    }

    if (_connectionState == CsBleConnectionState::Connecting ||
        _connectionState == CsBleConnectionState::Reconnecting)
    {
        ESP_LOGI(CsBleDevice::Tag(), "Reconnect skipped because connection is already in progress");
        return;
    }

    InitializeReconnectTimer();

    if (_reconnectTimer == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Reconnect timer unavailable");
        return;
    }

    const auto delayMs = GetNextReconnectDelayMs();
    _reconnectScheduled = true;

    SetConnectionState(CsBleConnectionState::Reconnecting);
    ESP_LOGI(CsBleDevice::Tag(), "Reconnect scheduled in %lu ms", static_cast<unsigned long>(delayMs));
    esp_timer_stop(_reconnectTimer);

    const auto result = esp_timer_start_once(_reconnectTimer, static_cast<uint64_t>(delayMs) * 1000ULL);

    if (result != ESP_OK)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Failed to start reconnect timer: %s", esp_err_to_name(result));
        _reconnectScheduled = false;
        SetConnectionState(CsBleConnectionState::Idle);
    }
}

void CsBleDevice::CancelReconnect()
{
    _reconnectScheduled = false;

    if (_reconnectTimer != nullptr)
    {
        esp_timer_stop(_reconnectTimer);
    }
}

void CsBleDevice::AttemptReconnect()
{
    if (_connectionState == CsBleConnectionState::Connected ||
        _connectionState == CsBleConnectionState::Authenticating ||
        _connectionState == CsBleConnectionState::Ready ||
        _connectionState == CsBleConnectionState::Connecting)
    {
        ESP_LOGI(CsBleDevice::Tag(), "Reconnect aborted because device is already connected or connecting");
        return;
    }

    ESP_LOGI(CsBleDevice::Tag(), "Attempting reconnect [Attempt: %u]", static_cast<unsigned>(_reconnectAttempts + 1U));
    SetConnectionState(CsBleConnectionState::Connecting);

    if (!Connect())
    {
        ESP_LOGW(CsBleDevice::Tag(), "Reconnect attempt failed immediately");
        ++_reconnectAttempts;
        SetConnectionState(CsBleConnectionState::Idle);
        ScheduleReconnect();
        return;
    }

    ++_reconnectAttempts;
}

void CsBleDevice::HandleReconnectTimer()
{
    _reconnectScheduled = false;
    AttemptReconnect();
}

void CsBleDevice::ResetReconnectBackoff()
{
    _reconnectAttempts = 0U;
}

uint32_t CsBleDevice::GetNextReconnectDelayMs() const
{
    uint32_t delayMs = _minReconnectDelayMs;

    for (uint8_t i = 0; i < _reconnectAttempts; ++i)
    {
        if (delayMs >= _maxReconnectDelayMs)
        {
            return _maxReconnectDelayMs;
        }

        delayMs *= 2U;
    }

    return std::min(delayMs, _maxReconnectDelayMs);
}

void CsBleDevice::SetReady()
{
    CancelReconnect();
    ResetReconnectBackoff();
    SetConnectionState(CsBleConnectionState::Ready);
    OnReady();
}

bool CsBleDevice::SendDisconnectCommand()
{
    static constexpr uint8_t Command = 'R';

    if (!WriteRaw(&Command, sizeof(Command)))
    {
        ESP_LOGW(CsBleDevice::Tag(), "Failed to send disconnect command");
        return false;
    }

    ESP_LOGI(CsBleDevice::Tag(), "Disconnect command sent");

    return true;
}

void CsBleDevice::ReconnectTimerCallback(void* arg)
{
    auto* device = static_cast<CsBleDevice*>(arg);

    if (device == nullptr)
    {
        return;
    }

    device->HandleReconnectTimer();
}

void CsBleDevice::AdvanceInitializationPipeline()
{
    if (_connHandle == BLE_HS_CONN_HANDLE_NONE)
    {
        return;
    }

    StartServiceDiscovery();
}

bool CsBleDevice::StartServiceDiscovery()
{
    SetConnectionState(CsBleConnectionState::DiscoveringServices);
    _services.clear();

    const int result = ble_gattc_disc_all_svcs(
        _connHandle,
        CsBleDevice::OnServiceDiscovered,
        this
    );

    if (result != ESUCCESS)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Service discovery failed to start: %d", result);
        SetConnectionState(CsBleConnectionState::Connected);
        return false;
    }

    return true;
}

bool CsBleDevice::StartCharacteristicDiscovery()
{
    _characteristics.clear();
    _characteristicDiscoveryServiceIndex = 0;
    SetConnectionState(CsBleConnectionState::DiscoveringCharacteristics);
    return DiscoverCharacteristicsForNextService();
}

bool CsBleDevice::DiscoverCharacteristicsForNextService()
{
    if (_characteristicDiscoveryServiceIndex >= _services.size())
    {
        ESP_LOGI(CsBleDevice::Tag(), "Characteristic discovery complete");
        OnCharacteristicsDiscovered();
        return StartMtuExchange();
    }

    const auto& service = _services[_characteristicDiscoveryServiceIndex];

    const int result = ble_gattc_disc_all_chrs(
        _connHandle,
        service.StartHandle,
        service.EndHandle,
        CsBleDevice::OnCharacteristicDiscovered,
        this
    );

    if (result != ESUCCESS)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Characteristic discovery failed to start: %d", result);
        SetConnectionState(CsBleConnectionState::Connected);
        return false;
    }

    return true;
}

bool CsBleDevice::StartMtuExchange()
{
    SetConnectionState(CsBleConnectionState::NegotiatingMtu);

    const int result = ble_gattc_exchange_mtu(
        _connHandle,
        CsBleDevice::OnMtuExchanged,
        this
    );

    if (result != ESUCCESS)
    {
        ESP_LOGE(CsBleDevice::Tag(), "MTU exchange failed to start: %d", result);
        SetConnectionState(CsBleConnectionState::Connected);
        return false;
    }

    return true;
}

void CsBleDevice::ContinueInitializationAfterSubscription()
{
    SetConnectionState(CsBleConnectionState::Initializing);

    if (_queueProcessor != nullptr)
    {
        _queueProcessor->Reset();
    }

    _initializationQueuedWorkPending = false;

    if (!OnInitialize())
    {
        ESP_LOGE(CsBleDevice::Tag(), "Device-specific initialization failed");
        SetConnectionState(CsBleConnectionState::Connected);
        return;
    }

    if (_queueProcessor != nullptr && _queueProcessor->IsBusy())
    {
        _initializationQueuedWorkPending = true;
        ESP_LOGI(CsBleDevice::Tag(), "Waiting for queued initialization work to complete");
        return;
    }

    SetConnectionState(CsBleConnectionState::Authenticating);

    if (!StartAuthentication())
    {
        ESP_LOGE(CsBleDevice::Tag(), "Failed to start authentication");
        SetConnectionState(CsBleConnectionState::Connected);
    }
}

int CsBleDevice::OnServiceDiscovered(
    uint16_t connHandle,
    const ble_gatt_error* error,
    const ble_gatt_svc* service,
    void* arg
)
{
    (void)connHandle;

    auto* device = static_cast<CsBleDevice*>(arg);
    if (device == nullptr)
    {
        return 0;
    }

    device->HandleServiceDiscoveryResult(error, service);
    return 0;
}

int CsBleDevice::OnCharacteristicDiscovered(
    uint16_t connHandle,
    const ble_gatt_error* error,
    const ble_gatt_chr* characteristic,
    void* arg
)
{
    (void)connHandle;

    auto* device = static_cast<CsBleDevice*>(arg);

    if (device == nullptr)
    {
        return 0;
    }

    device->HandleCharacteristicDiscoveryResult(error, characteristic);
    return 0;
}

int CsBleDevice::OnMtuExchanged(
    uint16_t connHandle,
    const ble_gatt_error* error,
    uint16_t mtu,
    void* arg
)
{
    (void)connHandle;

    auto* device = static_cast<CsBleDevice*>(arg);

    if (device == nullptr)
    {
        return 0;
    }

    device->HandleMtuExchangeResult(error, mtu);
    return 0;
}

int CsBleDevice::OnDescriptorDiscovered(
    uint16_t connHandle,
    const ble_gatt_error* error,
    uint16_t chrValHandle,
    const ble_gatt_dsc* descriptor,
    void* arg
)
{
    (void)connHandle;
    (void)chrValHandle;

    auto* device = static_cast<CsBleDevice*>(arg);

    if (!device) {
        return 0;
    }

    device->HandleDescriptorDiscoveryResult(error, descriptor);
    return 0;
}

int CsBleDevice::OnCccdWritten(
    uint16_t connHandle,
    const ble_gatt_error* error,
    ble_gatt_attr* attr,
    void* arg)
{
    (void)connHandle;
    (void)attr;

    auto* device = static_cast<CsBleDevice*>(arg);

    if (device == nullptr)
    {
        return 0;
    }

    device->HandleCccdWriteResult(error);
    return 0;
}

void CsBleDevice::HandleServiceDiscoveryResult(const ble_gatt_error* error, const ble_gatt_svc* service)
{
    if (error == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Service discovery callback returned null error");
        return;
    }

    if (error->status == 0 && service != nullptr)
    {
        CsBleGattService discoveredService = {};
        memcpy(&discoveredService.Uuid, &service->uuid, sizeof(service->uuid));
        discoveredService.StartHandle = service->start_handle;
        discoveredService.EndHandle = service->end_handle;
        _services.push_back(discoveredService);

        ESP_LOGI(CsBleDevice::Tag(), "Service discovered: start=%u end=%u",
            service->start_handle,
            service->end_handle);

        return;
    }

    if (error->status == BLE_HS_EDONE)
    {
        ESP_LOGI(CsBleDevice::Tag(), "Service discovery complete");
        OnServicesDiscovered();
        StartCharacteristicDiscovery();
        return;
    }

    ESP_LOGE(CsBleDevice::Tag(), "Service discovery error: %d", error->status);
    SetConnectionState(CsBleConnectionState::Connected);
}

void CsBleDevice::HandleCharacteristicDiscoveryResult(
    const ble_gatt_error* error,
    const ble_gatt_chr* characteristic)
{
    if (error == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Characteristic discovery callback returned null error");
        return;
    }

    if (error->status == 0 && characteristic != nullptr)
    {
        const auto& currentService = _services[_characteristicDiscoveryServiceIndex];
        CsBleGattCharacteristic discoveredCharacteristic = {};

        memcpy(
            &discoveredCharacteristic.CharacteristicUuid,
            &characteristic->uuid,
            sizeof(characteristic->uuid)
        );

        memcpy(
            &discoveredCharacteristic.ServiceUuid,
            &currentService.Uuid,
            sizeof(currentService.Uuid)
        );

        discoveredCharacteristic.DefinitionHandle = characteristic->def_handle;
        discoveredCharacteristic.ValueHandle = characteristic->val_handle;
        discoveredCharacteristic.Properties = characteristic->properties;

        _characteristics.push_back(discoveredCharacteristic);

        ESP_LOGI(
            CsBleDevice::Tag(),
            "Characteristic discovered [Service - Start Handle: %u; End Handle: %u] [Definition Handle: %u] "
            "[Value Handle: %u] [Properties: 0x%02X]",
            currentService.StartHandle,
            currentService.EndHandle,
            discoveredCharacteristic.DefinitionHandle,
            discoveredCharacteristic.ValueHandle,
            discoveredCharacteristic.Properties
        );

        return;
    }

    if (error->status == BLE_HS_EDONE)
    {
        ++_characteristicDiscoveryServiceIndex;
        DiscoverCharacteristicsForNextService();
        return;
    }

    ESP_LOGE(CsBleDevice::Tag(), "Characteristic discovery error: %d", error->status);
    SetConnectionState(CsBleConnectionState::Connected);
}

void CsBleDevice::HandleMtuExchangeResult(const ble_gatt_error* error, uint16_t mtu)
{
    if (error == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "MTU exchange callback returned null error");
        return;
    }

    if (error->status != 0)
    {
        ESP_LOGE(CsBleDevice::Tag(), "MTU exchange failed: %d", error->status);
        SetConnectionState(CsBleConnectionState::Connected);
        return;
    }

    constexpr uint16_t AttHeaderSize = 3U;

    if (mtu <= AttHeaderSize)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Negotiated MTU too small: %u", mtu);
        SetConnectionState(CsBleConnectionState::Connected);
        return;
    }

    _negotiatedMtu = static_cast<size_t>(mtu - AttHeaderSize);

    ESP_LOGI(CsBleDevice::Tag(), "MTU exchange complete: %u", _negotiatedMtu);
    OnMtuNegotiated(_negotiatedMtu);

    if (!ResolveGattProfile())
    {
        ESP_LOGE(CsBleDevice::Tag(), "Failed to resolve GATT profile");
        SetConnectionState(CsBleConnectionState::Connected);
        return;
    }

    if (!SubscribeToReadCharacteristic())
    {
        ESP_LOGE(CsBleDevice::Tag(), "Failed to subscribe to read characteristic");
        SetConnectionState(CsBleConnectionState::Connected);
        return;
    }
}

void CsBleDevice::HandleDescriptorDiscoveryResult(const ble_gatt_error* error, const ble_gatt_dsc* descriptor)
{
    if (error == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Descriptor discovery callback returned null error");
        return;
    }

    if (error->status == 0 && descriptor != nullptr)
    {
        ESP_LOGI(
            CsBleDevice::Tag(),
            "Descriptor discovered [Handle: %u] [UUID: %s]",
            descriptor->handle,
            CsBleUtilities::Instance().UuidToString(&descriptor->uuid.u).c_str()
        );

        if (descriptor->uuid.u.type == BLE_UUID_TYPE_16)
        {
            const auto* uuid16 =
                reinterpret_cast<const ble_uuid16_t*>(&descriptor->uuid.u);

            if (uuid16->value == 0x2902)
            {
                _cccdHandle = descriptor->handle;
            }
        }

        return;
    }

    if (error->status == BLE_HS_EDONE)
    {
        if (_cccdHandle == 0)
        {
            ESP_LOGE(CsBleDevice::Tag(), "CCCD not found");
            return;
        }

        ESP_LOGI(CsBleDevice::Tag(), "CCCD found [Handle: %u]", _cccdHandle);
        const uint8_t notifyEnableValue[2] = {0x01, 0x00};

        const int result = ble_gattc_write_flat(
            _connHandle,
            _cccdHandle,
            notifyEnableValue,
            sizeof(notifyEnableValue),
            CsBleDevice::OnCccdWritten,
            this
        );

        if (result != 0)
        {
            ESP_LOGE(CsBleDevice::Tag(), "Failed to enable notifications: %d", result);
            return;
        }

        ESP_LOGI(CsBleDevice::Tag(), "Enabling notifications...");
        return;
    }

    ESP_LOGE(CsBleDevice::Tag(), "Descriptor discovery error: %d", error->status);
}

void CsBleDevice::HandleCccdWriteResult(const ble_gatt_error* error)
{
    if (error == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "CCCD write callback returned null error");
        SetConnectionState(CsBleConnectionState::Connected);
        return;
    }

    if (error->status != 0)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Failed to enable notifications: %d", error->status);
        SetConnectionState(CsBleConnectionState::Connected);
        return;
    }

    ESP_LOGI(CsBleDevice::Tag(), "Notifications enabled");
    ContinueInitializationAfterSubscription();
}

const CsBleGattService* CsBleDevice::FindServiceByUuidString(const std::string& uuid) const
{
    for (const auto& service : _services)
    {
        const auto serviceUuid = CsBleUtilities::Instance().UuidToString(&service.Uuid.u);

        if (serviceUuid == uuid)
        {
            return &service;
        }
    }

    return nullptr;
}

const CsBleGattCharacteristic* CsBleDevice::FindCharacteristicByUuid(const ble_uuid_t* uuid) const
{
    if (uuid == nullptr)
    {
        return nullptr;
    }

    for (const auto& characteristic : _characteristics)
    {
        if (ble_uuid_cmp(&characteristic.CharacteristicUuid.u, uuid) == 0)
        {
            return &characteristic;
        }
    }

    return nullptr;
}

const CsBleGattCharacteristic* CsBleDevice::FindCharacteristicByUuid16(uint16_t uuid16) const
{
    ble_uuid16_t uuid = {};
    uuid.u.type = BLE_UUID_TYPE_16;
    uuid.value = uuid16;

    return FindCharacteristicByUuid(&uuid.u);
}

const CsBleGattCharacteristic* CsBleDevice::FindCharacteristicByUuidString(const std::string& uuid) const
{
    for (const auto& characteristic : _characteristics)
    {
        const auto characteristicUuid = CsBleUtilities::Instance().UuidToString(&characteristic.CharacteristicUuid.u);

        if (characteristicUuid == uuid)
        {
            return &characteristic;
        }
    }

    return nullptr;
}

bool CsBleDevice::QueueRawPacket(
    const std::vector<uint8_t>& payload,
    BleTransport::CsBleQueueCompletionType completionType
)
{
    if (_queueProcessor == nullptr)
    {
        return false;
    }

    return _queueProcessor->EnqueueRawPacket(payload, completionType);
}

bool CsBleDevice::QueueDataPayload(
    const std::vector<uint8_t>& payload,
    BleTransport::CsBleQueueCompletionType completionType
)
{
    if (_queueProcessor == nullptr)
    {
        return false;
    }

    return _queueProcessor->EnqueueDataPayload(payload, completionType);
}

bool CsBleDevice::TrySendWriteCommand(
    const std::optional<CsBleJsonWriteCommand>& command,
    const std::string& invalidWriteMessage
)
{
    if (!command.has_value())
    {
        if (!invalidWriteMessage.empty())
        {
            ESP_LOGW(CsBleDevice::Tag(), "%s", invalidWriteMessage.c_str());
        }
        else
        {
            ESP_LOGW(CsBleDevice::Tag(), "Invalid write command requested");
        }

        return false;
    }

    return QueueWriteCommand(command.value());
}

bool CsBleDevice::QueueWriteCommand(const CsBleJsonWriteCommand& command)
{
    return QueueDataPayload(
        command.ToBytes(),
        BleTransport::CsBleQueueCompletionType::Ack
    );
}

bool CsBleDevice::IsReadyForQueueProcessing() const
{
    return _connectionState == CsBleConnectionState::Initializing ||
           _connectionState == CsBleConnectionState::Authenticating ||
           _connectionState == CsBleConnectionState::Ready;
}

void CsBleDevice::OnQueueProcessorIdle()
{
    if (_connectionState == CsBleConnectionState::Initializing && _initializationQueuedWorkPending)
    {
        _initializationQueuedWorkPending = false;
        SetConnectionState(CsBleConnectionState::Authenticating);

        if (!StartAuthentication())
        {
            ESP_LOGE(CsBleDevice::Tag(), "Failed to start authentication");
            SetConnectionState(CsBleConnectionState::Connected);
        }

        return;
    }
}

bool CsBleDevice::SetAuthenticationToken(const std::string& token)
{
    std::string normalized;
    normalized.reserve(32);

    for (char entry : token)
    {
        if (entry != '-')
        {
            normalized.push_back(entry);
        }
    }

    if (normalized.size() != 32U)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Invalid auth token length");
        return false;
    }

    std::array<uint8_t, 16> bytes{};

    for (size_t i = 0; i < 16U; ++i)
    {
        const std::string byteString = normalized.substr(i * 2U, 2U);
        char* end = nullptr;
        const auto value = std::strtoul(byteString.c_str(), &end, 16);

        if (end == nullptr || *end != '\0' || value > 0xFF)
        {
            ESP_LOGE(CsBleDevice::Tag(), "Invalid auth token format");
            return false;
        }

        bytes[i] = static_cast<uint8_t>(value);
    }

    _authToken = bytes;
    ESP_LOGI(CsBleDevice::Tag(), "Authentication token configured");
    return true;
}

void CsBleDevice::UpdateAuthenticationState(const CsBleAuthState authState)
{
    if (_authState == authState)
    {
        return;
    }

    _authState = authState;
    ESP_LOGI(CsBleDevice::Tag(), "Authentication state updated: %d", static_cast<int>(_authState));

    switch (_authState)
    {
        case CsBleAuthState::Authenticated:
            SetReady();
            break;

        case CsBleAuthState::Unauthenticated:
        case CsBleAuthState::RateLimited:
        case CsBleAuthState::AuthRequestTimedOut:
        case CsBleAuthState::Unknown:
        default:
            break;
    }
}

bool CsBleDevice::StartAuthentication()
{
    if (!_authToken.has_value())
    {
        ESP_LOGE(CsBleDevice::Tag(), "Cannot authenticate: authentication token is not configured");
        return false;
    }

    if (_queueProcessor == nullptr)
    {
        ESP_LOGE(CsBleDevice::Tag(), "Cannot authenticate: queue processor is null");
        return false;
    }

    const std::vector<uint8_t> ackPacket = { 0xCC };

    if (!QueueRawPacket(ackPacket, BleTransport::CsBleQueueCompletionType::Immediate))
    {
        ESP_LOGE(CsBleDevice::Tag(), "Failed to queue auth ACK packet");
        return false;
    }

    const auto& tokenBytes = _authToken.value();
    const std::vector<uint8_t> tokenPacket(tokenBytes.begin(), tokenBytes.end());

    if (!QueueRawPacket(tokenPacket, BleTransport::CsBleQueueCompletionType::Immediate))
    {
        ESP_LOGE(CsBleDevice::Tag(), "Failed to queue auth token packet");
        return false;
    }

    ESP_LOGI(CsBleDevice::Tag(), "Authentication packet sequence queued");
    return true;
}
