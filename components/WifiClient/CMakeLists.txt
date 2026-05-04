idf_component_register(
    SRCS "CsWifiClient.cpp"
    INCLUDE_DIRS "."
    REQUIRES Core esp_wifi esp_netif esp_event nvs_flash
)
