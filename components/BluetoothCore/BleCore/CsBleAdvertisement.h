#pragma once
#include <string>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleCore {

struct CsBleAdvertisement
{
    /**
     * The manufacturer ID extracted from the advertisement's manufacturer data. This is typically used to identify the
     * manufacturer of the device that is advertising. The manufacturer ID is usually a 16-bit value that is
     * assigned by the Bluetooth SIG. For Chandler Systems, Inc. the manufacturer ID is 0x073A (1850 in decimal).
     */
    uint16_t ManufacturerId = 0;

    /**
     * The address type of the device that is advertising. This indicates whether the address is a public address or a
     * random address. The address type is important for establishing connections to the device, as it determines how
     * the address should be used in the connection process.
     */
    uint8_t PrimaryAddressType = 0xFF;

    /**
     * The event type of the advertisement, which indicates the type of advertising event that occurred. This is used to
     * determine how to interpret the advertisement data and how to respond to it. For example, an event type of
     * BLE_HCI_ADV_RPT_EVTYPE_ADV_IND indicates a connectable undirected advertisement, while
     * BLE_HCI_ADV_RPT_EVTYPE_DIR_IND indicates a connectable directed advertisement.
     */
    uint8_t PrimaryEventType = 0xFF;

    /**
     * The name of the device as extracted from the advertisement data. This is typically used to identify the device
     * and may contain a prefix that indicates the type of device. For example, a device with the name "C2_01" indicates
     * that it is a Signature Valve device, where "C2_" is the prefix used to identify that type of device.
     */
    std::string Name;

    /**
     * The Bluetooth address of the device that is advertising.
     */
    std::string Address;

    /**
     * The address type of the device that is advertising. This indicates whether the address is a public address or a
     * random address. The address type is important for establishing connections to the device, as it determines how
     * the address should be used in the connection process.
     */
    uint8_t AddressType = 0;

    /**
     * The manufacturer-specific data extracted from the advertisement.
     */
    std::vector<uint8_t> ManufacturerData;

    /**
     * The scan response data extracted from the advertisement.
     */
    std::vector<uint8_t> ScanResponseData;
};

}
