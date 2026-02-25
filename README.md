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

## License

This project is intended for educational and portfolio purposes.