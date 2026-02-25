#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <string.h>

#include <config.h>

// === ESP-NOW Helper Functions ===

static inline bool macIsZero(const uint8_t *mac)
{
    for (int i = 0; i <6; i++)
    {
        if(mac[i] != 0) return false;
    }
    return true;
}

static void printMAC(const uint8_t* mac)
{
    Serial.printf("%02X:%02X:%2X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

}

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

// === ESP-NOW: Peer Managment ===

static bool addPeer(const uint8_t* mac)
{
    if(macIsZero(mac))
    {
        Serial.println("[PEER] Invalid MAC (all zeros)");
        return false;
    }

// Remove if already exists (to update info)

    esp_now_del_peer(mac); 
    esp_now_peer_info_t peer{};
    memcpy(peer.peer_addr, mac, 6);
    peer.channel = ESPNOW_CHANNEL;
    peer.encrypt = false;

    const esp_err_t err = esp_now_add_peer(&peer);
    if (err != ESP_OK)
    {
       Serial.print("[AddPeer] Failed to add peer "); 
       printMAC(mac);
       Serial.println();
       return false;
    }
} 

// === Telemetry Payload Structure ===

typedef struct __attribute__((packed))
 {
  uint8_t protocol_version;   // telemetry versioning (for future compatibility)
  uint8_t node_id;            // logical node identifier (set in config.h)
  uint16_t reserved;          // laignment / future use 
  float temperature_c;        // corrected temperature in Celsius
  float humidity_pct;         // corrected humidity in percent
  uint32_t counter;
 }TelemetryPayload;

static TelemetryPayload payload;
static uint32_t lastSendMs = 0;

// === Arduino Setup & Loop ===

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
  if (esp_now_init() != ESP_OK) 
  {
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


void loop() 
{
    const uint32_t now = millis();

    if(now - lastSendMs >= SEND_INTERVAL_MS)
    {
        lastSendMs = now;

        // == File telemetry data (dummy data for now) ===
        payload.protocol_version = 1;
        payload.node_id = NODE_ID;
        payload.reserved = 0;
        payload.temperature_c = 0.0f;
        payload.humidity_pct = 0.0f;
        payload.counter++;

        Serial.printf("[SEND] v:%u temp:%.2fC rh:%.1f%% cnt:%lu\n",
           (unsigned long) payload.protocol_version,
           (unsigned long) payload.node_id,
           payload.temperature_c,
           payload.humidity_pct,
           (unsigned long) payload.counter
          );

        esp_now_send(PEER_A_MAC, (uint8_t*)&payload, sizeof(payload));
        esp_now_send(PEER_B_MAC, (uint8_t*)&payload, sizeof(payload));
        esp_now_send(PEER_C_MAC, (uint8_t*)&payload, sizeof(payload));    
    }
}

