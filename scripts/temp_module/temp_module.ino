// This script connects to the wifi automatically.

#include "secrets.h"

#include <ESP8266WiFi.h>
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
      Serial.println("Hey this worked atleast");
      if (strcmp(message,"OFF") == 0) {
        ledState = HIGH;// Turn the LED off
      } else if (strcmp(message,"ON") == 0) {
        ledState = LOW;// Turn the LED on
      }
      digitalWrite(BUILTIN_LED, ledState);  
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

}
 

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) { // 5 second transfer
    lastMsg = now;

    // Publish messages here
    
    // Current Temperature Celsius
    double temp_celsius = dht.readTemperature();
    snprintf (msg, MSG_BUFFER_SIZE, "%lf", temp_celsius);
    Serial.print("Publish message: ");
    Serial.print(msg);
    client.publish("room_1/dht_sensor/temperature/celsius", msg, true);

    // Current Temperature Ferenehit
    double temp_ferenheit = dht.readTemperature(true);
    snprintf (msg, MSG_BUFFER_SIZE, "%lf", temp_ferenheit);
    Serial.print("Publish message: ");
    Serial.print(msg);
    client.publish("room_1/dht_sensor/temperature/ferenheit", msg, true);
   
    // Current Humidity
    double humidity = dht.readHumidity();
    snprintf (msg, MSG_BUFFER_SIZE, "%lf", humidity);
    Serial.print("Publish message: ");
    Serial.print(msg);
    client.publish("room_1/dht_sensor/humidity", msg, true);

    // Indicator Light Status
    // if (ledState == HIGH) { //OFF
    //   snprintf (msg, MSG_BUFFER_SIZE, "OFF");
    // }
    // else { // ON
    //   snprintf (msg, MSG_BUFFER_SIZE, "ON");
    // }
    // Serial.print("Publish message: ");
    // Serial.print(msg);
    // client.publish("room_1/dht_sensor/light", msg);
    
    // Offline Status
    snprintf (msg, MSG_BUFFER_SIZE, "online");
    Serial.print("Publish message: ");
    Serial.print(msg);
    client.publish("room_1/dht_sensor/status", msg, true);



  }
}