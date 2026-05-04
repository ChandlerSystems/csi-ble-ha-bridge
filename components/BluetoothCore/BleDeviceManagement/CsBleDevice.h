#pragma once
#include "CsCore.h"
#include "CsBleDeviceType.h"
#include "BleCore/CsBleAdvertisement.h"
#include "BleCore/CsBleAuthState.h"
#include "BleCore/CsBleConnectionState.h"
#include "BleCore/CsBleGattCharacteristic.h"
#include "BleCore/CsBleGattProfile.h"
#include "BleCore/CsBleGattService.h"
#include "BleCore/CsBleJsonWriteCommand.h"
#include "BleCore/CsBleResolvedGattProfile.h"
#include "BleTransport/CsBleQueueProcessor.h"
#include "BleUtilities/CsBleUtilities.h"
#include <array>
#include <cstdint>
#include <esp_timer.h>
#include <host/ble_gap.h>
#include <host/ble_gatt.h>
#include <host/ble_uuid.h>
#include <host/ble_hs.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleDeviceManagement {

using namespace SignatureBridge::BluetoothCore;
using namespace SignatureBridge::BluetoothCore::BleCore;

namespace {
    static constexpr size_t CsBleMinMtu = 23;
}

class CsBleDevice
{
    CS_CLASS_TAG(CsBleDevice)

public:
    friend class BleTransport::CsBleQueueProcessor;

public:
    CsBleDevice();
    virtual ~CsBleDevice();

public:
    virtual CsBleDeviceType GetDeviceType() const = 0;
    virtual std::string GetDeviceIdPrefix() const = 0;
    virtual CsBleGattProfile GetGattProfile() const = 0;
    virtual void UpdateAdvertisement(const CsBleAdvertisement& advertisement);

public:
    /**
     * Initiates a connection to the device. If autoReconnect is set to true, the device will automatically attempt to
     * reconnect if the connection is lost. If autoReconnect is false, the device will not attempt to reconnect after
     * a disconnection, and any existing reconnection attempts will be canceled.
     *
     * @param[in] autoReconnect Flag indicating whether the device should automatically attempt to reconnect if the
     * connection is lost.
     *
     * @return True if the connection process was successfully initiated, false otherwise. Note that a return value of
     * true does not necessarily mean that the device is currently connected, as the connection process is asynchronous
     * and may still be in progress.
     */
    virtual bool Connect(const bool autoReconnect = false);

    /**
     * Disconnects from the device. If the device is currently connected, it will terminate the connection. If the
     * device is in the process of connecting, it will cancel the connection attempt. After calling this method,
     * the device will no longer attempt to reconnect automatically.
     */
    virtual void Disconnect();


public:
    /**
     * Sets the authentication token to be used for authenticating with the device.
     *
     * @param[in] token The authentication token as a string. The token is expected to be a 32-character hexadecimal
     * string, optionally separated by dashes (e.g., "00112233-4455-6677-8899-aabbccddeeff" or
     * "00112233445566778899aabbccddeeff").
     *
     * @return True if the authentication token was successfully set, false otherwise.
     */
    bool SetAuthenticationToken(const std::string& token);

    /**
     * Updates the authentication state of the device. This method should be called whenever there is a change in the
     * authentication state, such as after a successful authentication, a failed authentication attempt, or when the
     * device becomes unauthenticated for any reason. The authentication state is important for determining whether the
     * device is authorized to perform certain operations and for managing the connection state appropriately.
     *
     * @param[in] authState The new authentication state of the device.
     */
    void UpdateAuthenticationState(const CsBleAuthState authState);

private:
    /**
     * Checks if the device is currently in a state where it can process queue entries. This typically means that the
     * device is connected and ready to send or receive data.
     *
     * @return True if the device is ready for queue processing, false otherwise.
     */
    bool IsReadyForQueueProcessing() const;

    /**
     * Called when the queue processor becomes idle. This typically means that there are no pending operations in the
     * queue, and the device is ready to process new requests.
     */
    void OnQueueProcessorIdle();

public:
    /**
     * Writes raw data to the device.
     *
     * @param[in] buffer Pointer to the data to be written.
     * @param[in] length Number of bytes to write.
     *
     * @return True if the write operation was successful, false otherwise.

     */
    bool WriteRaw(const uint8_t* buffer, size_t length);

    /**
     * Queues a raw packet to be sent to the device. The packet will be sent according to the specified completion
     * type, which determines how the queue processor will handle acknowledgments and responses for this packet.
     *
     * @param[in] payload The raw data to be sent.
     * @param[in] completionType The completion type that determines how the queue processor will handle acknowledgments
     *                           and responses for this packet.
     *
     * @return True if the packet was successfully queued, false otherwise.

     */
    bool QueueRawPacket(
        const std::vector<uint8_t>& payload,
        BleTransport::CsBleQueueCompletionType completionType = BleTransport::CsBleQueueCompletionType::Immediate
    );

    /**
     * Queues a data payload to be sent to the device. The payload will be processed and sent according to the specified
     * completion type, which determines how the queue processor will handle acknowledgments and responses for this
     * payload.
     *
     * @param[in] payload The data payload to be sent.
     * @param[in] completionType The completion type that determines how the queue processor will handle acknowledgments
     *                           and responses for this payload.
     *
     * @return True if the payload was successfully queued, false otherwise.
     */
    bool QueueDataPayload(
        const std::vector<uint8_t>& payload,
        BleTransport::CsBleQueueCompletionType completionType = BleTransport::CsBleQueueCompletionType::Ack
    );

public:
    /**
     * Attempts to send a write command to the device if the command is valid. This method checks if the provided
     * command is valid (i.e., it has a value) and, if so, queues it to be sent to the device. If the command is not
     * valid, it logs a warning message and does not attempt to send anything.
     *
     * @param[in] command The optional write command to be sent. If this does not have a value, the method will log a
     *                    warning and return false.
     * @param[in] invalidWriteMessage An optional log message to be used if the command is invalid. If this is not
     *                                provided, a default warning message will be logged.
     *
     * @return True if the command was valid and was successfully queued to be sent, false otherwise.
     */
    bool TrySendWriteCommand(
        const std::optional<CsBleJsonWriteCommand>& command,
        const std::string& invalidWriteMessage = ""
    );

    /**
     * Queues a write command to be sent to the device.
     *
     * @param[in] command The write command to be sent.
     *
     * @return True if the command was successfully queued, false otherwise.
     */
    bool QueueWriteCommand(const CsBleJsonWriteCommand& command);

public:
    virtual bool IsConnected() const { return _connectionState >= CsBleConnectionState::Connected;}
    virtual bool IsConnecting() const { return _connectionState == CsBleConnectionState::Connecting; }
    virtual bool IsReady() const { return _connectionState == CsBleConnectionState::Ready; }

public:
    const CsBleAdvertisement& GetAdvertisement() const { return _advertisement; }
    const std::vector<uint8_t>& GetCombinedAdvertisementData() const { return _combinedAdvertisementData; }
    CsBleConnectionState GetConnectionState() const { return _connectionState; }
    uint16_t GetConnectionHandle() const { return _connHandle; }
    uint16_t GetNegotiatedMtu() const { return _negotiatedMtu; }
    const std::vector<CsBleGattService>& GetServices() const { return _services; }
    const std::vector<CsBleGattCharacteristic>& GetCharacteristics() const { return _characteristics; }
    const CsBleResolvedGattProfile& GetResolvedGattProfile() const { return _resolvedGattProfile; }
    bool HasAuthenticationToken() const { return _authToken.has_value(); }
    bool IsAuthenticated() const { return _authState == CsBleAuthState::Authenticated; }
    CsBleAuthState GetAuthState() const { return _authState; }

public:
    static int BleGapEventCallback(struct ble_gap_event* event, void* arg);

protected:
    int BleGapEventHandler(struct ble_gap_event* event);

private:
    bool ResolveGattProfile();
    bool SubscribeToReadCharacteristic();
    void SetConnectionState(CsBleConnectionState newState);
    void ClearConnectionSession();
    void AdvanceInitializationPipeline();
    bool StartServiceDiscovery();
    bool StartCharacteristicDiscovery();
    bool DiscoverCharacteristicsForNextService();
    bool StartMtuExchange();
    void ContinueInitializationAfterSubscription();

private:
    void InitializeReconnectTimer();
    void DestroyReconnectTimer();
    void ScheduleReconnect();
    void CancelReconnect();
    void AttemptReconnect();
    void HandleReconnectTimer();
    void ResetReconnectBackoff();
    uint32_t GetNextReconnectDelayMs() const;
    bool StartAuthentication();
    void SetReady();
    bool SendDisconnectCommand();

private:
    static void ReconnectTimerCallback(void* arg);

protected:
    static int OnServiceDiscovered(
        uint16_t connHandle,
        const ble_gatt_error* error,
        const ble_gatt_svc* service,
        void* arg
    );

    static int OnCharacteristicDiscovered(
        uint16_t connHandle,
        const ble_gatt_error* error,
        const ble_gatt_chr* characteristic,
        void* arg
    );

    static int OnMtuExchanged(
        uint16_t connHandle,
        const ble_gatt_error* error,
        uint16_t mtu,
        void* arg
    );

    static int OnDescriptorDiscovered(
        uint16_t connHandle,
        const ble_gatt_error* error,
        uint16_t chrValHandle,
        const ble_gatt_dsc* descriptor,
        void* arg
    );

    static int OnCccdWritten(
        uint16_t connHandle,
        const ble_gatt_error* error,
        ble_gatt_attr* attr,
        void* arg
    );

protected:
    void HandleServiceDiscoveryResult(const ble_gatt_error* error, const ble_gatt_svc* service);
    void HandleCharacteristicDiscoveryResult(const ble_gatt_error* error, const ble_gatt_chr* characteristic);
    void HandleMtuExchangeResult(const ble_gatt_error* error, uint16_t mtu);
    void HandleDescriptorDiscoveryResult(const ble_gatt_error* error, const ble_gatt_dsc* descriptor);
    void HandleCccdWriteResult(const ble_gatt_error* error);

protected:
    const CsBleGattService* FindServiceByUuidString(const std::string& uuid) const;
    const CsBleGattCharacteristic* FindCharacteristicByUuid(const ble_uuid_t* uuid) const;
    const CsBleGattCharacteristic* FindCharacteristicByUuid16(uint16_t uuid16) const;
    const CsBleGattCharacteristic* FindCharacteristicByUuidString(const std::string& uuid) const;

protected:
    virtual void OnAdvertisementUpdated() {}
    virtual bool OnInitialize() { return true; }
    virtual void OnConnected() {}
    virtual void OnDisconnected(const int reason) { (void)reason; }
    virtual void OnConnectFailed(const int status) { (void)status; }
    virtual void OnServicesDiscovered() {}
    virtual void OnCharacteristicsDiscovered() {}
    virtual void OnMtuNegotiated(const uint16_t mtu) { (void)mtu; }
    virtual void OnReady() {}
    virtual bool ShouldAutoReconnect(const int reason) const { (void)reason; return _autoReconnectEnabled; }
    virtual void OnNotificationReceived(
        const CsBleGattCharacteristic* characteristic,
        const uint8_t* buffer,
        size_t length
    );
    virtual void OnPacketDecoded(const std::vector<CsBlePacketChange>& changes) { (void)changes; }
    virtual void OnPayloadReceived(const std::vector<uint8_t>& payload) { (void)payload; }
    virtual void OnQueueEntryCompleted(const std::vector<uint8_t>& payload) { (void)payload; }
    virtual void OnQueueEntryFailed(const std::vector<uint8_t>& payload, const char* reason)
    {
        (void)payload;
        (void)reason;
    }

protected:
    CsBleAdvertisement _advertisement = {};
    std::vector<uint8_t> _combinedAdvertisementData = {};
    uint16_t _connHandle = BLE_HS_CONN_HANDLE_NONE;
    uint16_t _negotiatedMtu = CsBleMinMtu;
    CsBleConnectionState _connectionState = CsBleConnectionState::Idle;
    std::vector<CsBleGattService> _services;
    std::vector<CsBleGattCharacteristic> _characteristics;
    size_t _characteristicDiscoveryServiceIndex = 0;
    CsBleResolvedGattProfile _resolvedGattProfile = {};
    uint16_t _cccdHandle = 0;
    CsBleAuthState _authState = CsBleAuthState::Unknown;
    std::optional<std::array<uint8_t, 16>> _authToken;

protected:
    bool _autoReconnectEnabled = false;
    bool _reconnectScheduled = false;
    uint8_t _reconnectAttempts = 0;
    uint32_t _minReconnectDelayMs = 500;
    uint32_t _maxReconnectDelayMs = 5000;
    esp_timer_handle_t _reconnectTimer = nullptr;

private:
    std::unique_ptr<BleTransport::CsBleQueueProcessor> _queueProcessor;
    bool _initializationQueuedWorkPending = false;
};

}
