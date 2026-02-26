# ESP32-C3 Telemetry Node

Modular ESP32-C3 based telemetry node using the Arduino framework and PlatformIO.

---

## Overview

This project implements a structured telemetry node based on the ESP32-C3.

The node:

- Reads temperature and relative humidity from an SHT31 sensor (I2C)
- Applies optional calibration offsets
- Packages the measurements into a fixed binary telemetry frame
- Transmits the frame periodically via ESP-NOW

The goal of this project is to demonstrate:

- Structured embedded firmware design
- Fixed-size binary protocol definition
- Clean separation of configuration and logic
- Reusable wireless transport architecture

---

## Hardware

- ESP32-C3 (tested with ESP32-C3 DevKitM-1)
- SHT31 temperature & humidity sensor (I2C)
- 3.3V power supply

### I2C Wiring

| Signal | ESP32-C3 Pin |
|--------|--------------|
| SDA    | 8            |
| SCL    | 9            |
| VCC    | 3.3V         |
| GND    | GND          |

> Pin numbers can be adjusted in `config.h`.

---

## Telemetry Frame Structure

The telemetry frame is transmitted as a packed binary struct:

```cpp
typedef struct __attribute__((packed))
{
  uint8_t  protocol_version;
  uint8_t  node_id;
  uint16_t reserved;
  float    temperature_c;
  float    humidity_pct;
  uint32_t counter;
} TelemetryPayload;
```

### Field Description

| Field             | Type       | Description                          |
|------------------|------------|--------------------------------------|
| protocol_version | uint8_t    | Version of telemetry format          |
| node_id          | uint8_t    | Logical identifier of the node       |
| reserved         | uint16_t   | Reserved for future use              |
| temperature_c    | float      | Calibrated temperature in °C         |
| humidity_pct     | float      | Calibrated relative humidity in %    |
| counter          | uint32_t   | Monotonic transmission counter       |

The struct is packed to guarantee a fixed binary layout for ESP-NOW transmission.

---

## Configuration

Configuration values are defined in:

include/config.h

This file is excluded from version control.

To configure the project:

1. Copy `include/config.example.h`
2. Rename it to `include/config.h`
3. Adjust the following parameters:

- `NODE_ID`
- `ESPNOW_CHANNEL`
- Peer MAC addresses
- I2C pins
- Sensor calibration offsets
- Send interval

---

## Build & Flash

1. Open this project folder in VS Code
2. Install the PlatformIO extension
3. Select environment: `esp32-c3-devkitm-1`
4. Build → Upload → Monitor

Serial monitor baud rate:

115200

---

## Runtime Output (Example)

Example serial output during operation:

```
[SHT31] Sensor init OK
[SEND] v:1 node:1 temp:24.53C rh:47.8% cnt:1
[SEND] v:1 node:1 temp:24.58C rh:47.9% cnt:2
```

---

## Architecture Notes

- Configuration is separated from firmware logic
- Fixed-width integer types are used for deterministic memory layout
- Telemetry struct uses explicit alignment strategy
- Designed for integration into a larger modular embedded control system


---

## OTA Firmware Update

This project supports secure Over-The-Air (OTA) firmware updates.

### Configuration Concept

OTA settings are defined **only once** in:

include/wifi_config.h


```cpp
#define OTA_HOSTNAME   "esp32c3-telemetry-node"
#define OTA_PASSWORD   "your-ota-password"

The file wifi_config.h is excluded from version control.

Automated PlatformIO Integration

The script:

scripts/ota_from_wifi_config.py

automatically reads:

OTA_HOSTNAME

OTA_PASSWORD

and injects them into the PlatformIO build process.

This automatically sets:

UPLOAD_PORT = <OTA_HOSTNAME>.local

--auth=<OTA_PASSWORD>

No manual editing of platformio.ini is required after configuration.

OTA Upload Workflow

First flash must be done via USB:

pio run -t upload

After WiFi connection is established:

pio run -t upload

The device is addressed automatically via:

<OTA_HOSTNAME>.local

No IP address configuration is required.
## License

This project is intended for educational and portfolio purposes.
```

---

## Mechanical Housing

The production-ready 3D housing for this telemetry node  
(ESP32-C3 + SHT31 sensor configuration) is published separately:

MakerWorld Model:
https://makerworld.com/de/models/2449935-esp32-c3-telemetry-node-housing-sht31-sensor#profileId-2689591

Developed under the NorthForge3D technical label.
