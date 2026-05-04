#include "CsBleScanner.h"
#include "CsCore.h"
#include "CsCoreUtils.h"
#include "BleCore/CsBleAdvertisement.h"
#include "BleDeviceManagement/CsBleDeviceManager.h"
#include "BleDeviceManagement/CsBleDeviceRegistry.h"
#include "BleUtilities/CsBleUtilities.h"
#include <esp_log.h>
#include <host/ble_hs.h>
#include <host/ble_hs_adv.h>
#include <host/ble_hs_id.h>
#include <host/util/util.h>
#include <unordered_map>
#include <vector>

using namespace SignatureBridge::BluetoothCore;
using namespace SignatureBridge::BluetoothCore::BleCore;
using namespace SignatureBridge::BluetoothCore::BleScanner;
using namespace SignatureBridge::BluetoothCore::BleUtilities;
using namespace SignatureBridge::BluetoothCore::BleDeviceManagement;
using namespace SignatureBridge::Core;

static int BleGapEventHandler(struct ble_gap_event* event, void* arg);
static void OnDeviceDiscovered(struct ble_gap_event* event);

static std::unordered_map<std::string, CsBleAdvertisement> sPendingAdvertisements;

bool CsBleScanner::StartScan()
{
    if (_isScanning)
    {
        ESP_LOGW(CsBleScanner::Tag(), "Scanner already running");
        return true;
    }

    const struct ble_gap_disc_params params = {
        .itvl = 0x0010,
        .window = 0x0010,
        .filter_policy = 0,
        .limited = 0,
        .passive = 0,
        .filter_duplicates = 1,
        .disable_observer_mode = 0,
    };

    const int result = ble_gap_disc(0, BLE_HS_FOREVER, &params, BleGapEventHandler, nullptr);

    if (result != ESUCCESS)
    {
        ESP_LOGE(CsBleScanner::Tag(), "Failed to start scan: %d", result);
        return false;
    }

    ESP_LOGI(CsBleScanner::Tag(), "Scanning started...");
    _isScanning = true;
    return true;
}

bool CsBleScanner::StopScan()
{
    if (!_isScanning)
    {
        ESP_LOGW(CsBleScanner::Tag(), "Scanner is not running");
        return true;
    }

    const int result = ble_gap_disc_cancel();

    if (result != ESUCCESS)
    {
        ESP_LOGE(CsBleScanner::Tag(), "Failed to stop scan: %d", result);
        return false;
    }

    ESP_LOGI(CsBleScanner::Tag(), "Scan stopped");
    _isScanning = false;
    return true;
}

static int BleGapEventHandler(struct ble_gap_event* event, void* arg)
{
    ARG_UNUSED(arg);

    switch (event->type)
    {
        case BLE_GAP_EVENT_DISC:
        {
            OnDeviceDiscovered(event);
            break;
        }

        case BLE_GAP_EVENT_DISC_COMPLETE:
        {
            ESP_LOGI(CsBleScanner::Tag(), "Scan complete");
            break;
        }

        default:
            break;
    }

    return ESUCCESS;
}

static void OnDeviceDiscovered(struct ble_gap_event* event)
{
    constexpr uint8_t nullTerminatorSizeSize = 1U;
    const ble_gap_disc_desc* desc = &event->disc;
    std::vector<uint8_t> advertisementData;
    struct ble_hs_adv_fields fields;
    std::string name;

    const int result = ble_hs_adv_parse_fields(&fields, desc->data, desc->length_data);
    char nameBuffer[CS_MAX_ADV_NAME_LEN] = { 0 };

    if (result == ESUCCESS && fields.name != nullptr && fields.name_len > 0)
    {
        size_t copyLength = fields.name_len < (CS_MAX_ADV_NAME_LEN - nullTerminatorSizeSize) ?
            fields.name_len :
            (CS_MAX_ADV_NAME_LEN - nullTerminatorSizeSize);

        memcpy(nameBuffer, fields.name, copyLength);
        nameBuffer[copyLength] = '\0';
        name = nameBuffer;
    }

    if (result == ESUCCESS && fields.mfg_data != nullptr && fields.mfg_data_len > 0)
    {
        advertisementData.assign(fields.mfg_data, fields.mfg_data + fields.mfg_data_len);
    }

    const std::string address = CsBleUtilities::Instance().FormatBleAddress(&desc->addr);
    auto& advertisement = sPendingAdvertisements[address];

    advertisement.Name = name;
    advertisement.Address = address;
    advertisement.AddressType = desc->addr.type;

    const bool isPrimaryAdv =
        desc->event_type == BLE_HCI_ADV_RPT_EVTYPE_ADV_IND ||
        desc->event_type == BLE_HCI_ADV_RPT_EVTYPE_DIR_IND ||
        desc->event_type == BLE_HCI_ADV_RPT_EVTYPE_SCAN_IND ||
        desc->event_type == BLE_HCI_ADV_RPT_EVTYPE_NONCONN_IND;

    if (isPrimaryAdv)
    {
        advertisement.PrimaryAddressType = desc->addr.type;
        advertisement.PrimaryEventType = desc->event_type;
    }

    if (desc->event_type == BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP)
    {
        advertisement.ScanResponseData = advertisementData;
    }
    else
    {
        advertisement.ManufacturerData = advertisementData;
    }

    if (!advertisement.ManufacturerData.empty())
    {
        advertisement.ManufacturerId = CsCoreUtils::Instance().ReadLittleEndianU16(
            advertisement.ManufacturerData.data(),
            advertisement.ManufacturerData.size()
        );
    }
    else if (!advertisement.ScanResponseData.empty())
    {
        advertisement.ManufacturerId = CsCoreUtils::Instance().ReadLittleEndianU16(
            advertisement.ScanResponseData.data(),
            advertisement.ScanResponseData.size()
        );
    }

    if (advertisement.ManufacturerId != CS_BLE_MANUFACTURER_ID ||
        !CsBleDeviceRegistry::Instance().MatchesRegisteredPrefix(advertisement.Name))
    {
        return;
    }

    ESP_LOGI(
        CsBleScanner::Tag(),
        "Device Discovered [Address: %s] [RSSI: %d] [Name: %s] [Manufacturer ID: 0x%04X] "
                          "[Manufacturer Data Length: %d] [Response Data Length: %d]",
        advertisement.Address.c_str(),
        desc->rssi,
        advertisement.Name.c_str(),
        advertisement.ManufacturerId,
        static_cast<int>(advertisement.ManufacturerData.size()),
        static_cast<int>(advertisement.ScanResponseData.size())
    );

    if (!advertisement.Name.empty())
    {
        CsBleDeviceManager::Instance().OnAdvertisement(advertisement, desc);
    }
}
