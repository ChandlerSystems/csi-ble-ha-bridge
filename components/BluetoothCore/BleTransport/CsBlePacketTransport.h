#pragma once
#include "CsBlePacketTransportEvent.h"
#include "BleCore/CsBleAckNakType.h"
#include "BleCore/CsBleKeepAliveType.h"
#include "BleCore/CsBleTimeoutType.h"
#include "CsCore.h"
#include "cJSON.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleTransport {

using namespace SignatureBridge::BluetoothCore::BleCore;

namespace {
    constexpr size_t CsBlePacketHeaderSize = 1U;
    constexpr size_t CsBlePacketCrcSize = 2U;
    constexpr size_t CsBlePacketOverhead = CsBlePacketHeaderSize + CsBlePacketCrcSize;
}

using CsBlePacketTransportEventHandler = std::function<void(const CsBlePacketTransportEvent&)>;

class CsBlePacketTransport
{
    CS_CLASS_TAG(CsBlePacketTransport)

public:
    CsBlePacketTransport();
    virtual ~CsBlePacketTransport() = default;

public:
    /**
     * Function to reset the state of the packet transport, including any in-progress reassembly of packets. This
     * should be called when a connection is lost or reset to ensure that the transport is in a clean state for the
     * next connection.
     */
    void Reset();

    /**
     * Function to process incoming payload data. This function will attempt to parse the buffer as either a status
     * packet or a data packet, and will dispatch the appropriate events based on the contents of the buffer. If the
     * buffer cannot be parsed, a protocol error event will be dispatched.
     *
     * @param[in] buffer Pointer to the buffer containing the payload data.
     * @param[in] length Length of the buffer.
     */
    void ProcessPayload(const uint8_t* buffer, size_t length);

    /**
     * Function to set the event handler for packet transport events. The event handler is a callback function that will
     * be called whenever a packet transport event occurs, such as receiving an ACK, NAK, keep-alive, timeout, data
     * packet, CRC error, or protocol error. The event handler will receive a CsBlePacketTransportEvent object that
     * contains information about the event.
     *
     * @param[in] handler The event handler callback function.
     */
    void SetEventHandler(CsBlePacketTransportEventHandler handler);

    /**
     * Function to be called when a notification is received from the BLE stack. This function will take the raw buffer
     * and length of the notification, and will process it as a payload by calling the ProcessPayload function. This is
     * the main entry point for incoming data from the BLE stack, and will trigger the parsing and event dispatching for
     * any received data.
     *
     * @param[in] buffer Pointer to the buffer containing the notification data.
     * @param[in] length Length of the buffer.
     */
    void OnNotificationReceived(const uint8_t* buffer, size_t length);

public:
    /**
     * Function to build an ACK or NAK packet based on the specified type. This function will create a packet with the
     * appropriate header for the ACK or NAK type, and will return the raw bytes of the packet that can be sent over
     * BLE.
     *
     * @param[in] type The type of ACK or NAK packet to build.
     * @return The raw bytes of the packet.
     */
    std::vector<uint8_t> BuildAckNakPacket(CsBleAckNakType type) const;

    /**
     * Function to build a keep-alive packet based on the specified type. This function will create a packet with the
     * appropriate header for the keep-alive type, and will return the raw bytes of the packet that can be sent over
     * BLE.
     *
     * @param[in] type The type of keep-alive packet to build.
     * @return The raw bytes of the packet.
     */
    std::vector<uint8_t> BuildKeepAlivePacket(CsBleKeepAliveType type) const;

    /**
     * Function to build a timeout packet based on the specified type. This function will create a packet with the
     * appropriate header for the timeout type, and will return the raw bytes of the packet that can be sent over BLE.
     *
     * @param[in] type The type of timeout packet to build.
     * @return The raw bytes of the packet.
     */
    std::vector<uint8_t> BuildTimeoutPacket(CsBleTimeoutType type) const;

    /**
     * Function to build a raw packet with the specified payload. This function will not create any headers or add any
     * CRC to the payload, and will return the raw bytes of the packet that can be sent over BLE.
     *
     * @note This function essentially returns the payload as-is. It's can be used to show clear intent to send a
     * packet that doesn't conform to the expected structure. This can be useful for sending the authentication token,
     * commands (like the reset command), etc.
     *
     * @param[in] payload The payload data to include in the packet.
     *
     * @return The raw bytes of the packet.
     */
    std::vector<uint8_t> BuildRawPacket(const std::vector<uint8_t>& payload) const;

    /**
     * Function to build one or more data packets from the specified payload, based on the negotiated MTU. This function
     * will split the payload into appropriately sized chunks, add the necessary headers and CRC for each packet, and
     * return a vector of packets, where each packet is represented as a vector of bytes that can be sent over BLE.
     *
     * @param[in] payload The payload data to include in the packets.
     * @param[in] negotiatedMtu The negotiated Maximum Transmission Unit (MTU) size.
     *
     * @return A vector of packets, where each packet is represented as a vector of bytes.
     */
    std::vector<std::vector<uint8_t>> BuildDataPackets(const std::vector<uint8_t>& payload, size_t negotiatedMtu) const;

protected:
    virtual void OnPayloadReceived(const std::vector<uint8_t>& payload) { (void)payload; }
    virtual void OnAckReceived() {}
    virtual void OnNakReceived() {}
    virtual void OnKeepAliveReceived(const CsBlePacketTransportHeader& header) { (void)header; }
    virtual void OnTimeoutAckReceived(const CsBlePacketTransportHeader& header) { (void)header; }
    virtual void OnTimeoutQueryReceived(const CsBlePacketTransportHeader& header) { (void)header; }
    virtual void OnDataPacketAccepted(const CsBlePacketTransportHeader& header) { (void)header; }
    virtual void OnCrcError(const CsBlePacketTransportHeader& header) { (void)header; }
    virtual void OnProtocolError(const CsBlePacketTransportHeader& header) { (void)header; }
    virtual void LogTransportDebug(const char* format, ...) { (void)format; }
    virtual void OnPacketDecoded(const std::vector<CsBlePacketChange>& changes) { (void)changes; }

private:
    bool TryHandleStatusPacket(const uint8_t* buffer, const size_t length, CsBlePacketTransportEvent& event);
    bool TryHandleDataPacket(const uint8_t* buffer, const size_t length, CsBlePacketTransportEvent& event);
    bool ValidatePacketCrc(const uint8_t* buffer, const size_t length) const;

private:
    bool TryDecodePayload(const std::vector<uint8_t>& payload, std::vector<CsBlePacketChange>& changes) const;
    bool TryDecodeJsonValue(const cJSON* item, CsBlePacketValue& value) const;

private:
    size_t GetMaxPayloadPerPacket(size_t negotiatedMtu) const;
    std::vector<uint8_t> BuildPacketChunk(const uint8_t* buffer, size_t length, bool isFirst, bool isLast) const;

private:
    void ResetReassembly();
    void RaiseEvent(const CsBlePacketTransportEvent& event);
    void DispatchEvent(const CsBlePacketTransportEvent& event);

private:
    bool _isReassembling = false;
    std::vector<uint8_t> _reassemblyBuffer;
    CsBlePacketTransportEventHandler _eventHandler;
};

}
