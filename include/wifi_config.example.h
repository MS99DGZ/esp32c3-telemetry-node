#pragma once

// === WiFi configuration (public template) ===
// Copy this file to wifi_config.h and adjust the values.
// WARNING: wifi_config.h is ignored by git and MUST NOT be committed.

#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"

// Hostname shown in the network during OTA updates
#define OTA_HOSTNAME   "esp32c3-telemetry-node"

// OTA password used to protect firmware uploads
#define OTA_PASSWORD   "change-me-ota-password"