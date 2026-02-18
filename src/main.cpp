#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include <config.example.h>



// === ESP-NOW Channel Configuration ===

static void fixChannel ()
{
// Forve radio to operate on fixed channel ( requred for ESP-NOW )
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

Serial.printf("[ESPNOW] Channel fixed to %u\n" , (unsigned) ESPNOW_CHANNEL);
}



// === ESP-NOW: Send Callback ===

static void onSent(const uint8_t *mac, esp_now_send_status_t status)
{
  (void)mac; // unused

  Serial.print("[TX-CB] Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}



void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("=== ESP32-C3 Supermini NODE START ===");

  // === Wifi Configuration for esp-now===
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  fixChannel();

  // === ESP-NOW Initialization ===
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ESPNOW] Initialization failed");
    while (true) delay(1000);
    }

    esp_now_register_send_cb(onSent);

    // === Runtime Info ===
     Serial.print("[INFO] Device MAC: ");
  Serial.println(WiFi.macAddress());

  Serial.print("[INFO] WiFi Channel: ");
  Serial.println(WiFi.channel());

  Serial.println("[INFO] ESP-NOW transport ready");
}


void loop() {
  delay(1000);
  Serial.println("[INFO] Alive (transport initialized)");
}

