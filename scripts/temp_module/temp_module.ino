#include "secrets.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include "DHT.h"

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

#define DHTPIN 0
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

int ledState;
double temp_offset_celsius = 0;

void connect_wifi() { // Wifi Connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to the wifi...");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP Address: ");
  Serial.println(WiFi.localIP());

}

void connect_mqtt_broker() { // MQTT Connection
  client.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);
  client.setCallback(callback);
}

void arduino_ota() {
  ArduinoOTA.setHostname("myesp8266");
  ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void callback(char* topic, byte* payload, unsigned int length) { // Reading MQTT Messages
  char message[length + 1];
  // Logging received topic and payload.
  Serial.println();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println("LENGTH: " + length);
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.println(message);

  // Custom Commands (Set/Subscription)
  if (strcmp(topic, "room_1/dht_sensor/light") == 0){
      if (strcmp(message,"OFF") == 0) {
        ledState = HIGH;// Turn the LED off
      } else if (strcmp(message,"ON") == 0) {
        ledState = LOW;// Turn the LED on
      }
      digitalWrite(BUILTIN_LED, ledState);  
  }

  if (strcmp(topic, "room_1/dht_sensor/calibrate") == 0){
    char *eptr;
    temp_offset_celsius = strtod(message, &eptr);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "temp_humidity_sensor_room_";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASS, "room_1/dht_sensor/status", 1, true, "offline")) {
      // Connected succesfully.
      Serial.println("Succesfully connected to MQTT Broker!");

      // Sending a online status message.
      client.publish("room_1/dht_sensor/status", "online", true); 
      
      // Subscribing to topics
      client.subscribe("room_1/dht_sensor/#", 0);
      client.subscribe("room_1/dht_sensor/status", 1);



    } else {
      // Trying to reconnect.
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Trying again in 10 seconds");
      // Waiting about 8 seconds before retrying
      for (int i = 0; i < 4; i++) {
        ledState = LOW;
        digitalWrite(BUILTIN_LED, ledState);
        delay(1000);
        ledState = HIGH;
        digitalWrite(BUILTIN_LED, ledState);
        delay(1000);
      }
    }
  }
}


void setup() {
  // Serial Connection
  Serial.begin(115200);
  Serial.println();

  // Built in LED indicator
  pinMode(BUILTIN_LED, OUTPUT);

  // Start the Temperature & Humidity Sensor DHT22
  dht.begin(); 

  // Connect ESP8266 to wifi
  connect_wifi();

  // Connect to MQTT broker
  connect_mqtt_broker();

  // Connect with Arduino OTA updates
  arduino_ota();

}
 

void loop() {

  //PubSub Client
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  ArduinoOTA.handle(); // OTA Handler

  // Operations
  unsigned long now = millis();
  if (now - lastMsg > 5000) { // 5 second transfer
    lastMsg = now;

    // Publish messages here
    
    // Current Temperature Celsius
    double temp_celsius = dht.readTemperature() + (double) temp_offset_celsius;
    snprintf (msg, MSG_BUFFER_SIZE, "%.1lf", temp_celsius);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/dht_sensor/temperature/celsius", msg, true);

    // Current Temperature Ferenehit
    double temp_ferenheit = temp_celsius * (9.0/5.0) + 32.0;
    snprintf (msg, MSG_BUFFER_SIZE, "%.1lf", temp_ferenheit);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/dht_sensor/temperature/ferenheit", msg, true);
   
    // Current Humidity
    double humidity = dht.readHumidity();
    snprintf (msg, MSG_BUFFER_SIZE, "%.1lf%%", humidity); // %% for printing %
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/dht_sensor/humidity", msg, true);

    // Offline Status
    snprintf (msg, MSG_BUFFER_SIZE, "online");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/dht_sensor/status", msg, true);



  }
}