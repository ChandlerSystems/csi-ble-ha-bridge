idf_component_register(
    SRCS "CsDeviceBootstrap.cpp"
         "SignatureValve/HomeAssistant/CsSignatureValveHaEntities.cpp"
         "SignatureValve/HomeAssistant/CsSignatureValveHaPublisher.cpp"
         "SignatureValve/HomeAssistant/CsSignatureValveHaStateValueResolver.cpp"
         "SignatureValve/SignatureCore/CsSignatureValvePacketKeyMapper.cpp"
         "SignatureValve/SignatureCore/CsSignatureValvePacketParser.cpp"
         "SignatureValve/CsSignatureValveDevice.cpp"
         "SignatureValve/CsSignatureValveWriter.cpp"
    INCLUDE_DIRS . Common SignatureValve SignatureValve/HomeAssistant SignatureValve/SignatureCore
    REQUIRES BluetoothCore Core MqttClient
)
