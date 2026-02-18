#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("=== ESP32-C3 Supermini NODE START ===");
}

void loop() {
  delay(1000);
  Serial.println("[INFO] Alive");
}

