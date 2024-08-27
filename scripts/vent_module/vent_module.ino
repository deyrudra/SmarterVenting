#include "secrets.h"
#include <WiFi.h>
// #include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	50
char msg[MSG_BUFFER_SIZE];
int value = 0;

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */

int ledState;
int finAngle;
String closureState = "Open";
String powerState = "Max-Power";
String climateState = "Cooling";
String controlState = "Auto";
double desiredTemp = 22.0;
double currentTemp;
String desiredTempState;
Servo myServo;
const int SERVOPIN = 2;
int priority;

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
  ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.setPort(8266);
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

void moveVent(int angle) {
  moveServo(angle);

  if (angle == 0) {
    closureState = "Closed";
  }
  else if (0 < angle <= 22.5) {
    closureState = "Semi-Closed";
  }
  else if (22.5 < angle <= 67.5) {
    closureState = "Neutral";
  }
  else if (67.5 < angle <= 90) {
    closureState = "Semi-Open";
  }
  else if (angle == 90) {
    closureState = "Open";
  }
  else {
    closureState = "NULL";
  }
}


// Callback
void callback(char* topic, byte* payload, unsigned int length) { // Reading MQTT Messages
  char message[length];
  // Logging received topic and payload.
  Serial.println();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  memcpy(message, payload, length);
  message[length] = '\0';
  Serial.println(message);

  // Custom Commands (Set/Subscription)

  if (strcmp(topic, "room_1/vent/angle") == 0) {
    char* pEnd;
    double angle = strtod(message, &pEnd);
    if (0 <= angle <= 90) {
      moveVent(angle);
    }
  }

  // if (strcmp(topic, "room_1/vent/closure_state") == 0) {
  //   if (strcmp(message, "Closed") == 0){
  //     moveVent(0);
  //   }
  //   if (strcmp(message, "Semi-Closed") == 0){
  //     moveVent(22.5);
  //   }
  //   if (strcmp(message, "Neutral") == 0){
  //     moveVent(45);
  //   }
  //   if (strcmp(message, "Semi-Open") == 0){
  //     moveVent(67.5);
  //   }
  //   if (strcmp(message, "Open") == 0){
  //     moveVent(90);
  //   }
  // } 
 
  if (strcmp(topic, "room_1/vent/power_mode") == 0) {
    // ("Default", "Eco", "Max-Power")
    if (strcmp(message, "Default") == 0) {
      powerState = "Default";
    }
    if (strcmp(message, "Eco") == 0) {
      powerState = "Eco";

    }
    if (strcmp(message, "Max-Power") == 0) {
      powerState = "Max-Power";

    }

  } 

  if (strcmp(topic, "room_1/vent/climate_mode") == 0) {
    // Current Vent Climate Mode ("Heating", "Cooling")
    if (strcmp(message, "Heating") == 0){
      climateState = "Heating";
    }
    if (strcmp(message, "Cooling") == 0){
      climateState = "Cooling";
    }
  } 

  if (strcmp(topic, "room_1/vent/control_mode") == 0) {
    // Current Vent Control Mode ("Auto", "Manual")
    if (strcmp(message, "Auto") == 0){
      controlState = "Auto";
    }
    if (strcmp(message, "Manual") == 0){
      controlState = "Manual";
    }
  } 

  if (strcmp(topic, "room_1/vent/desired_temp") == 0) {
    // 17 to 26 degrees
    char* pEnd;
    desiredTemp = strtod(message, &pEnd);
  } 

  if (strcmp(topic, "room_1/dht_sensor/temperature/celsius") == 0) {
    char *eptr;
    currentTemp = strtod(message, &eptr);
  }

  if (strcmp(topic, "room_1/vent/priority") == 0) {
    // 17 to 26 degrees
    char* pEnd;
    priority = (int) strtod(message, &pEnd);
  } 

  // if (strcmp(topic, "room_1/dht_sensor/light") == 0){
  //     if (strcmp(message,"OFF") == 0) {
  //       ledState = HIGH;// Turn the LED off
  //     } else if (strcmp(message,"ON") == 0) {
  //       ledState = LOW;// Turn the LED on
  //     }
  //     digitalWrite(BUILTIN_LED, ledState);  
  // }

  // if (strcmp(topic, "room_1/dht_sensor/calibrate") == 0){
  //   char *eptr;
  //   temp_offset_celsius = strtod(message, &eptr); // Need this for angle
  // }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "temp_humidity_sensor_room_";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASS, "room_1/vent/status", 1, true, "offline")) {
      // Connected succesfully.
      Serial.println("Succesfully connected to MQTT Broker!");

      // Sending a online status message.
      client.publish("room_1/vent/status", "online", true); 
      
      // Subscribing to topics
      client.subscribe("room_1/vent/#", 1);
      client.subscribe("room_1/dht_sensor/temperature/celsius", 1);
      client.subscribe("room_1/dht_sensor/humidity", 1);
      

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

void heatingCycle() {
  // Now based on the desired temperature, current temperature, and possibly outside temperature perform operations.
  if (currentTemp > (desiredTemp - 0.5)) {

    moveVent(0); // To NOT heat the current temp
    desiredTempState = "NO";
  }

  if ((desiredTemp - 0.5) <= currentTemp ) {
    if (currentTemp <= (desiredTemp + 0.5) ) {
      moveVent(45); // To maintain the current temperature
      desiredTempState = "YES";
    }
  }

  if (currentTemp < (desiredTemp + 0.5)) {
    moveVent(90); // To heat the current temp
    desiredTempState = "NO";
  }
}

void coolingCycle() {
  // Now based on the desired temperature, current temperature, and possibly outside temperature perform operations.
  if (currentTemp < (desiredTemp - 0.5)) {
    moveVent(0); // To NOT cool the current temp
    desiredTempState = "NO";
  }

  if ((desiredTemp - 0.5) <= currentTemp ) {
    if (currentTemp <= (desiredTemp + 0.5) ) {
      moveVent(45); // To maintain the current temperature
      desiredTempState = "YES";
    }
  }

  if (currentTemp > (desiredTemp + 0.5)) {
    moveVent(90); // To cool the current temp
    desiredTempState = "NO";
  }
  
}

void moveServo(int angle) {
  // Connect to Servo pin 2.
  angle = 90 - angle;
  angle = angle * 2;
  myServo.attach(SERVOPIN);
  myServo.write(angle);
  delay(3000);
  myServo.detach();
}

void setup() {
  // Serial Connection
  Serial.begin(115200);
  Serial.println();

  // Built in LED indicator
  pinMode(BUILTIN_LED, OUTPUT);

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

    // Current Vent Closure State ("Closed", "Semi-Closed", "Open", "Semi-Open")
    snprintf (msg, MSG_BUFFER_SIZE, "%s", closureState.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/vent/closure_state", msg, true);
   
    // Current Vent Power Mode ("Default", "Eco", "Max-Power")
    snprintf (msg, MSG_BUFFER_SIZE, "%s", powerState.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/vent/power_mode", msg, true);
   
    // Current Vent Climate Mode ("Heating", "Cooling")
    snprintf (msg, MSG_BUFFER_SIZE, "%s", climateState.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/vent/climate_mode", msg, true);
   
    // Current Vent Control Mode ("Auto", "Manual")
    snprintf (msg, MSG_BUFFER_SIZE, "%s", controlState.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/vent/control_mode", msg, true);
   
    // Current Vent Desired Temperature Reached State ("YES", "NO")
    snprintf (msg, MSG_BUFFER_SIZE, "%s", desiredTempState.c_str());
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/vent/desired_temp_reached", msg, true);

    // Offline Status ("offline", "online", "eco-online")
    snprintf (msg, MSG_BUFFER_SIZE, "online");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("room_1/vent/status", msg, true);

    // Check powerState and loop depending on it's state.
    if (powerState.equals("Eco") || powerState.equals("Default")) { // Go into DeepSleep
      if (climateState.equals("Heating")){
        // Call heating cycle function
        heatingCycle();
      }
      if (climateState.equals("Cooling")){
        // Call cooling cycle function
        coolingCycle();
      }

      // Go to sleep code.


    }


    if (powerState.equals("Max-Power")) { // Operate Always
      if (climateState.equals("Heating")){
        // Call heating cycle function
        heatingCycle();
      }
      if (climateState.equals("Cooling")){
        // Call cooling cycle function
        coolingCycle();
      }
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      esp_deep_sleep_start();

    }



  }
}