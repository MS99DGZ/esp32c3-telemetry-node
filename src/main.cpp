#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Adafruit_SHT31.h>
#include <string.h>

#include <config.h>
#include <wifi_config.h>

// === Sensor: SHT 31 ===
static Adafruit_SHT31 g_sht31;

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
    return true;
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

  // === WiFi infrastructure connection (required for OTA) ===
  
  Serial.print("[WiFi] Connecting to SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long wifiStartMs = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStartMs < 15000UL)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("[WiFi] Connected, IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("[WiFi] Connection FAILED (timeout), continuing without IP");
  }
  
    // === OTA configuration ===
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA
    .onStart([]()
    {
      Serial.println("[OTA] Update start");
    })
    .onEnd([]()
    {
      Serial.println("[OTA] Update end");
    })
    .onProgress([](unsigned int progress, unsigned int total)
    {
      Serial.printf("[OTA] Progress: %u%%\r", (progress * 100U) / total);
    })
    .onError([](ota_error_t error)
    {
      Serial.printf("[OTA] Error[%u]\n", error);
    });

  ArduinoOTA.begin();

  Serial.println("[OTA] Ready (waiting for updates)");

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

  // === Add Peers ===

  addPeer(PEER_A_MAC);
  addPeer(PEER_B_MAC);
  addPeer(PEER_C_MAC);


  // === Sensor: I2C + SHT 31 initialization ===
  
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000); // 100kHz

  if(!g_sht31.begin(0x44))
  {
    Serial.println("[SHT31] Sensor not found at 0x44");
    while(true) delay(1000);
  }
  else
  {
    Serial.println("[SHT31] Sensor initialized successfully");
  }

  Serial.println("[INFO] ESP-NOW transport ready");
}


void loop() 
{
    // Handle OTA updates (if WiFi is connected)
  ArduinoOTA.handle();

  const uint32_t now = millis();

    if(now - lastSendMs >= SEND_INTERVAL_MS)
    {
        lastSendMs = now;

        // === Read Sensor Data ===
        const float t_raw = g_sht31.readTemperature();
        const float rh_raw = g_sht31.readHumidity();

        if(isnan(t_raw) || isnan(rh_raw))
        {
            Serial.println("[SHT31] Failed to read sensor data");
            return;
        }

        const float t_corr = t_raw + TEMP_OFFSET_C;
        float      h_corr = rh_raw + RH_OFFSET_PTC;

        if(h_corr < 0.0f) h_corr = 0.0f;
        if(h_corr > 100.0f) h_corr = 100.0f;
        
        // == File telemetry data (dummy data for now) ===
        payload.protocol_version = 1;
        payload.node_id = NODE_ID;
        payload.reserved = 0;
        payload.temperature_c = t_corr;
        payload.humidity_pct = h_corr;
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

