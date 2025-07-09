#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

const char* ssid = "ICS Ryder on Olive";
const char* password = "RUSzZz#6P7";

// Fixed URL to check latest version and get firmware
const char* version_url = "";
const char* firmware_url = "https://github.com/Alumina-deLina/ESP32_OTA_updates/blob/main/CounterPlus1_OTA1.ino";


const String current_version = "1.0.6"; // Update this when you build a new .bin

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected.");
  Serial.println("Checking for firmware update...");

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);  // Follow redirects
  http.begin(version_url);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String new_version = http.getString();
    new_version.trim();
    Serial.printf("Current version: %s, Available version: %s\n", current_version.c_str(), new_version.c_str());

    if (new_version != current_version) {
      Serial.println("New firmware available. Starting update...");

      // Add progress callback
      httpUpdate.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Update Progress: %u%%\n", (progress * 100) / total);
      });

      
      WiFiClientSecure client;
      client.setInsecure();  // Skip certificate check (⚠️ safe for GitHub, but avoid in prod)
      client.setTimeout(35000);  // 35 seconds instead of default 5

      httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      t_httpUpdate_return ret = httpUpdate.update(client, firmware_url);
      
      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("Update failed. Error: %s\n", httpUpdate.getLastErrorString().c_str());
          break;
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("No update.");
          break;
        case HTTP_UPDATE_OK:
          Serial.println("Update successful.");
          break;
      }
    } else {
      Serial.println("Firmware is up to date.");
    }
  } else {
    Serial.printf("Failed to check version file. HTTP code: %d\n", httpCode);
  }

  http.end();
}

void loop() {
  static unsigned long lastPrint = 0;
  static int counter = 0;

  if (millis() - lastPrint >= 1000) {
    Serial.printf("Counter: %d\n", counter);
    counter += 30;  // increment by 20
    lastPrint = millis();
  }
}
