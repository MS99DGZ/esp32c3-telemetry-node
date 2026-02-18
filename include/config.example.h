#include <Arduino.h>


// ===== I2C PINS (ESP32-C3 SuperMini) =====

 static const int I2C_SDA_PIN = 8; // GPIO8
 static const int I2C_SCL_PIN = 9; // GPIO9

 // ===== ESP-NOW Channel =====

 static const uint8_t ESP_NOW_CHANNEL = 1; // ESP-NOW operates on Wi-Fi channels 1


// ===== Sensor Offset =====

static const float TEMP_OFFSET_C = -4.0f; // Adjust this value to calibrate the sensor readings
static const float RH_OFFSET_PTC = +15.0f; // Adjust this value to calibrate the sensor readings

// ===== Sensor Read Interval =====

static const uint32_t SEND_INTERVAL_MS = 1000; // Send data every seconds (1000 milliseconds)

