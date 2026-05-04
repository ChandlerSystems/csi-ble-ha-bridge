#include "CsBleUtilities.h"
#include "CsCore.h"
#include "CsCoreUtils.h"
#include <esp_log.h>
#include <host/util/util.h>

using namespace SignatureBridge::BluetoothCore::BleUtilities;
using namespace SignatureBridge::Core;

namespace {
    static constexpr size_t MinAdvPayloadLength = 4;
    static constexpr size_t ManufacturerIdLength = 2;
}

CsBleUtilities& CsBleUtilities::Instance()
{
    static CsBleUtilities instance;
    return instance;
}

std::string CsBleUtilities::FormatBleAddress(const ble_addr_t* address) const
{
    if (!address)
    {
        return "N/A";
    }

    char buffer[CS_BLE_ADDR_STR_LEN];

    // The address bytes are in little-endian format, so we need to reverse the order when formatting the string.
    std::snprintf(
        buffer,
        sizeof(buffer),
        "%02X:%02X:%02X:%02X:%02X:%02X",
        address->val[5], address->val[4], address->val[3],
        address->val[2], address->val[1], address->val[0]
    );

    return CsBleUtilities::Instance().NormalizeMacAddress(std::string(buffer));
}

void CsBleUtilities::ParseBleAddress(const std::string& address, ble_addr_t* parsedAddress)
{
    uint8_t buffer[6];

    sscanf(
        address.c_str(),
        "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
        &buffer[5], &buffer[4], &buffer[3],
        &buffer[2], &buffer[1], &buffer[0]
    );

    memcpy(parsedAddress->val, buffer, 6);
}

std::string CsBleUtilities::NormalizeMacAddress(const std::string& macAddress)
{
    std::string normalized = macAddress;

    std::transform(
        normalized.begin(),
        normalized.end(),
        normalized.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); }
    );

    return normalized;
}

bool CsBleUtilities::IsMacAddressValid(const std::string& macAddress)
{
    if (macAddress.empty() || macAddress.length() != 17)
    {
        return false;
    }

    for (size_t index = 0; index < macAddress.length(); ++index)
    {
        if ((index + 1U) % 3U == 0U)
        {
            if (macAddress[index] != ':')
            {
                return false;
            }

            continue;
        }

        if (!std::isxdigit(static_cast<unsigned char>(macAddress[index])))
        {
            return false;
        }
    }

    return true;
}

std::string CsBleUtilities::ParseUuid128(const uint8_t* buffer, uint8_t length)
{
    if (length != 16)
    {
        return "N/A";
    }

    char uuidStr[37];

    // The UUID bytes are in little-endian format, so we need to reverse the order when formatting the string.
    std::snprintf(
        uuidStr,
        sizeof(uuidStr),
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        buffer[15], buffer[14], buffer[13], buffer[12],
        buffer[11], buffer[10],
        buffer[9],  buffer[8],
        buffer[7],  buffer[6],
        buffer[5],  buffer[4], buffer[3], buffer[2], buffer[1], buffer[0]
    );

    return std::string(uuidStr);
}

std::string CsBleUtilities::UuidToString(const ble_uuid_t* uuid) const
{
    if (uuid == nullptr)
    {
        return "N/A";
    }

    char buffer[BLE_UUID_STR_LEN];
    ble_uuid_to_str(uuid, buffer);
    return std::string(buffer);
}

std::vector<uint8_t> CsBleUtilities::CombineAdvertisementPayloads(
    const std::vector<uint8_t>& manufacturerData,
    const std::vector<uint8_t>& responseData
)
{
    // The advertisement payload has two parts: the manufacturer data and the scan response data. The complete
    // information about the device is split between these two parts, so we need to combine them to parse the complete
    // payload that contains all the information. However, we want it to look like a single payload that we would get
    // if all the information was in the manufacturer data, so we need to strip the manufacturer ID from the scan
    // response data if it matches our expected manufacturer ID, since that information is already included in the
    // manufacturer data.

    const size_t totalLength = manufacturerData.size() + responseData.size();

    if (totalLength < MinAdvPayloadLength)
    {
        ESP_LOGW(
            CsBleUtilities::Tag(),
            "Advertisement payloads too short [Manufacturer Data Length: %d bytes] "
                                             "[Scan Response Data Length: %d bytes]",
            manufacturerData.size(),
            responseData.size()
        );

        return {};
    }

    std::vector<uint8_t> combinedData;

    if (manufacturerData.size() >= ManufacturerIdLength)
    {
        combinedData.insert(combinedData.end(), manufacturerData.begin(), manufacturerData.end());
    }

    if (responseData.size() >= ManufacturerIdLength)
    {
        const uint16_t manufacturerId = CsCoreUtils::Instance().ReadLittleEndianU16(
            responseData.data(),
            responseData.size()
        );

        combinedData.insert(
            combinedData.end(),
            responseData.begin() + (manufacturerId == CS_BLE_MANUFACTURER_ID ? ManufacturerIdLength : 0),
            responseData.end()
        );
    }

    return combinedData;
}

std::string CsBleUtilities::ConnectionStateToString(CsBleConnectionState state) const
{
    switch (state)
    {
        case CsBleConnectionState::Idle: return "Idle";
        case CsBleConnectionState::Connecting: return "Connecting";
        case CsBleConnectionState::Connected: return "Connected";
        case CsBleConnectionState::DiscoveringServices: return "DiscoveringServices";
        case CsBleConnectionState::DiscoveringCharacteristics: return "DiscoveringCharacteristics";
        case CsBleConnectionState::NegotiatingMtu: return "NegotiatingMtu";
        case CsBleConnectionState::Initializing: return "Initializing";
        case CsBleConnectionState::Ready: return "Ready";
        case CsBleConnectionState::Reconnecting: return "Reconnecting";
        default: return "Unknown";
    }
}
