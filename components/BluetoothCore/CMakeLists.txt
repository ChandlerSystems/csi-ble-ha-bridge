idf_component_register(
    SRCS "CsBleSessionManager.cpp"
         "BleCore/CsBlePacketValue.cpp"
         "BleDeviceManagement/CsBleDevice.cpp"
         "BleDeviceManagement/CsBleDeviceManager.cpp"
         "BleDeviceManagement/CsBleDeviceRegistry.cpp"
         "BleScanner/CsBleScanner.cpp"
         "BleTransport/CsBlePacketTransport.cpp"
         "BleTransport/CsBlePacketTransportHeader.cpp"
         "BleTransport/CsBleQueueProcessor.cpp"
         "BleUtilities/CsBleUtilities.cpp"
    INCLUDE_DIRS . BleCore BleDeviceManagement BleScanner BleTransport BleUtilities
    REQUIRES bt Core json nvs_flash
)
