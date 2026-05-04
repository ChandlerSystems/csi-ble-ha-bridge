# CSI BLE → Home Assistant Bridge (Example)

> Example BLE → MQTT → Home Assistant integration using ESP32 and ESP-IDF

This project demonstrates a reference implementation for integrating a Chandler Systems, Inc. BLE device
with Home Assistant via MQTT Discovery.

---

## Features

- BLE device integration (Signature Valve example)
- MQTT transport layer
- Home Assistant MQTT Discovery
- Example sensors:
  - Time
  - Regen state
  - Salt remaining

---

## Requirements

- ESP32 development board
- ESP-IDF (v5.x recommended)
- WiFi network (2.4 GHz)
- MQTT broker (e.g., Mosquitto)
- Home Assistant instance with MQTT integration enabled

---

## Configuration

Edit the following file before building:

main/CsAppConfig.h

Update:

- WiFi SSID and password
- MQTT host, port, and credentials
- Target BLE device (optional)

---

## Build & Flash

idf.py build
idf.py -p <PORT> flash monitor

---

## Verification

1. Ensure the device connects to WiFi and MQTT (see serial logs)
2. Verify MQTT topics using an MQTT client (e.g., MQTT Explorer)
3. Confirm that entities appear in Home Assistant via MQTT Discovery

---

## Notes

- This project is intended as an example/reference implementation only
- It is not a complete or production-ready integration
- The implementation intentionally exposes a limited set of sensors for demonstration purposes

---

## NOTICE

This project is provided as an example/reference implementation and is not intended for production use.

All code and related materials are provided “AS IS”, without warranty of any kind, express or implied. Any use or
misuse of this code, or of the Chandler Systems, Inc. Bluetooth Device API, is solely the responsibility of the user.
Chandler Systems, Inc. assumes no liability for any damages arising from its use.
