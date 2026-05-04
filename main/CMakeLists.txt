idf_component_register(
    SRCS "CsApp.cpp"
    INCLUDE_DIRS "."
    REQUIRES Core BluetoothCore Devices MqttClient WifiClient
)
