#pragma once
#include <stdint.h>

// === HOW TO USE ===
// 1) Copy this file to: include/config.h
// 2) Adjust values in include/config.h for your setup
// 3) Do NOT commit include/config.h (it is ignored by .gitignore)

// === I2C PINS (ESP32-C3) ===
// Default pins for many ESP32-C3 DevKit setups.
// Adjust if your wiring is different.
static const int I2C_SDA = 8;
static const int I2C_SCL = 9;

// === ESP-NOW CHANNEL ===
// All ESP-NOW nodes must use the same 2.4 GHz WiFi channel.
static const uint8_t ESPNOW_CHANNEL = 1;

// === PEER MAC ADDRESSES (PLACEHOLDERS) ===
// Replace these in include/config.h with real target MACs.
// Format: {0xAA,0xBB,0xCC,0x11,0x22,0x33}
static const uint8_t PEER_A_MAC[6] = {0xAA,0xBB,0xCC,0x11,0x22,0x33};
static const uint8_t PEER_B_MAC[6] = {0xAA,0xBB,0xCC,0x44,0x55,0x66};
static const uint8_t PEER_C_MAC[6] = {0xAA,0xBB,0xCC,0x77,0x88,0x99};

// === SENSOR CALIBRATION OFFSETS ===
// Applied to raw measurements before transmission.
static const float TEMP_OFFSET_C = 0.0f;
static const float RH_OFFSET_PCT = 0.0f;

// === SEND INTERVAL ===
// Telemetry send interval in milliseconds.
static const uint32_t SEND_INTERVAL_MS = 1000;
