#pragma once
#include "CsCore.h"
#include "BleCore/CsBleConnectionState.h"
#include "BleCore/CsBlePacketValueType.h"
#include "host/ble_hs.h"
#include <stdint.h>
#include <string>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleUtilities {

using namespace SignatureBridge::BluetoothCore::BleCore;

class CsBleUtilities
{
    CS_CLASS_TAG(CsBleUtilities)

public:
    /**
     * Get the singleton instance of CsBleUtilities.
     *
     * @return Reference to the singleton instance of CsBleUtilities.
     */
    static CsBleUtilities& Instance();

private:
    /**
     * Private constructor to enforce singleton pattern. This prevents external code from creating instances of
     * CsBleUtilities, ensuring that only one instance exists and is accessed through the Instance() method.
     */
    CsBleUtilities() = default;

public:
    /**
     * Formats a BLE address into a human-readable string format (e.g. "01:23:45:67:89:AB").
     *
     * @param[in] address The BLE address to format.
     *
     * @return The formatted BLE address on success, or "N/A" if the input address is null.
     */
    std::string FormatBleAddress(const ble_addr_t* address) const;

    /**
     * Parses a BLE address from a string format (e.g. "01:23:45:67:89:AB") into a ble_addr_t structure.
     *
     * @param[in] address The string representation of the BLE address.
     * @param[out] parsedAddress The ble_addr_t structure to store the parsed address.
     */
    void ParseBleAddress(const std::string& address, ble_addr_t* parsedAddress);

    /**
     * Normalizes a MAC address string by converting it to uppercase and ensuring it is in the standard format (e.g.
     * "01:23:45:67:89:AB"). This is useful for comparing MAC addresses in a consistent way, regardless of the input
     * format.
     *
     * @param[in] macAddress The MAC address string to normalize.
     *
     * @return The normalized MAC address string.
     */
    std::string NormalizeMacAddress(const std::string& macAddress);

    /**
     * Validates a MAC address string to ensure it is in the correct format (e.g. "01:23:45:67:89:AB").
     *
     * @param[in] macAddress The MAC address string to validate.
     *
     * @return True if the MAC address is valid, false otherwise.
     */
    bool IsMacAddressValid(const std::string& macAddress);

public:
    /**
     * Parses a 128-bit UUID from a byte buffer and logs it in the standard UUID string format
     * (e.g. "12345678-1234-5678-1234-567812345678").
     *
     * @param[in] buffer The byte buffer containing the UUID.
     * @param[in] length The length of the byte buffer.
     *
     * @return The formatted UUID string on success, or "N/A" if the input is invalid.
     */
    std::string ParseUuid128(const uint8_t* buffer, uint8_t length);

    /**
    * Converts a ble_uuid_t structure to a human-readable string format. This function handles both 16-bit and 128-bit
    * UUIDs, and formats them according to the standard UUID string representation.
    *
    * @param[in] uuid The ble_uuid_t structure containing the UUID to convert.
    *
    * @return The formatted UUID string on success, or "N/A" if the input is invalid.
    */
    std::string UuidToString(const ble_uuid_t* uuid) const;

public:
    /**
     * Combines the manufacturer data and the scan response data into a single payload. This is necessary because the
     * information about the Bluetooth devices are split between the manufacturer data and the scan response data in the
     * advertisement, and we need to combine them to parse the complete payload that contains all the relevant
     * information about the device.
     *
     * @param[in] manufacturerData The manufacturer data from the advertisement.
     * @param[in] responseData The scan response data from the advertisement.
     *
     * @return The combined payload containing all relevant information about the device.
     */
    std::vector<uint8_t> CombineAdvertisementPayloads(
        const std::vector<uint8_t>& manufacturerData,
        const std::vector<uint8_t>& responseData
    );

public:
    /**
     * Converts a CsBleConnectionState enum value to a human-readable string representation. This is useful for logging
     * and debugging purposes, as it allows us to easily understand the current connection state of a BLE device when
     * reading logs.
     *
     * @param[in] state The CsBleConnectionState enum value to convert.
     * @return The human-readable string representation of the connection state.
     */
    std::string ConnectionStateToString(CsBleConnectionState state) const;
};

}
