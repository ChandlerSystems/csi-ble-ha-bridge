#include "CsBleDeviceManager.h"
#include "CsBleSessionManager.h"
#include "CsBleScanner.h"
#include "CsCore.h"
#include <esp_err.h>
#include <esp_log.h>
#include <host/ble_hs.h>
#include <host/ble_hs_pvcy.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <nvs_flash.h>

using namespace SignatureBridge::BluetoothCore;

static void OnBleHostTask(void *param);
static void OnBleSync(void);

static SignatureBridge::BluetoothCore::CsBleSessionManager* sActiveSession = nullptr;

bool CsBleSessionManager::Initialize()
{
    ESP_LOGI(CsBleSessionManager::Tag(), "Initializing NimBLE...");

    sActiveSession = this;

    CsBleDeviceManager::Instance().SetSessionManager(this);
    esp_err_t result = nvs_flash_init();

    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(CsBleSessionManager::Tag(), "NVS needs erase");
        ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase());
        result = nvs_flash_init();
    }

    if (result != ESP_OK)
    {
        ESP_LOGE(CsBleSessionManager::Tag(), "NVS init failed: %s", esp_err_to_name(result));
        return false;
    }

    const int nimbleResult = nimble_port_init();

    if (nimbleResult != ESUCCESS)
    {
        ESP_LOGE(CsBleSessionManager::Tag(), "nimble_port_init failed: %d", nimbleResult);
        return false;
    }

    ble_hs_cfg.sync_cb = OnBleSync;
    nimble_port_freertos_init(OnBleHostTask);
    ESP_LOGI(CsBleSessionManager::Tag(), "NimBLE initialized");

    return true;
}

bool CsBleSessionManager::StartScan()
{
    return _scanner.StartScan();
}

bool CsBleSessionManager::StopScan()
{
    return _scanner.StopScan();
}

void CsBleSessionManager::RequestConnection(CsBleDevice* device, const CsBleConnectionOptions& options)
{
    if (device == nullptr)
    {
        ESP_LOGE(CsBleSessionManager::Tag(), "Cannot connect: device is null");
        return;
    }

    if (!options.HasAuthToken())
    {
        ESP_LOGE(CsBleSessionManager::Tag(), "Cannot connect: authentication token is empty");
        return;
    }

    _scanner.StopScan();
    vTaskDelay(pdMS_TO_TICKS(50));

    if (!device->SetAuthenticationToken(options.AuthToken))
    {
        ESP_LOGE(CsBleSessionManager::Tag(), "Cannot connect: invalid authentication token");
        return;
    }

    if (!device->Connect(true))
    {
        ESP_LOGE(CsBleSessionManager::Tag(), "Failed to start device connection");
    }
}

static void OnBleHostTask(void *param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

static void OnBleSync(void)
{
    ESP_LOGI(CsBleSessionManager::Tag(), "BLE host synced");

    uint8_t addrType;

    if (ble_hs_id_infer_auto(0, &addrType) != 0)
    {
        ESP_LOGE(CsBleSessionManager::Tag(), "Failed to infer addr type");
        return;
    }

    ble_hs_pvcy_rpa_config(NIMBLE_HOST_ENABLE_RPA);

    if (sActiveSession != nullptr)
    {
        sActiveSession->StartScan();
    }
}
