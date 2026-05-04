idf_component_register(
    SRCS "CsMqttClient.cpp"
    INCLUDE_DIRS .
    REQUIRES Core esp_event mqtt
)
