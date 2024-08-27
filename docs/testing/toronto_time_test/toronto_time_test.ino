#include <ESP8266WiFi.h>            // or use <ESP8266WiFi.h> for ESP8266
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace these with your network credentials
const char* ssid     = "Rudra's Nest";
const char* password = "74deanscroft";

// Create UDP object
WiFiUDP ntpUDP;
// Define NTP Client to get time
NTPClient timeClient(ntpUDP, "pool.ntp.org", -14400, 60000);  // -14400 for Toronto timezone (UTC-4), adjust for daylight saving

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTP Client
  timeClient.begin();
}

void loop() {
  timeClient.update();

  // Print current time
  Serial.println(timeClient.getFormattedTime());

  delay(1000);
}