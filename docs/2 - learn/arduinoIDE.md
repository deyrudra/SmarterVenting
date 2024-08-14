- Instead of writing the script in a yaml configuartion on Home Assistant, I opted to type a script in Arduino. Then compile it into it's binary, and upload through OTA update on the hosted webserver.

- To start developing in the Arduino IDE for the ESP devices, we need to do the following:
    1. Install ESP8266 and ESP32 Add-on in Arduino IDE
        - Go to Preferences in the IDE, and put `https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json` for additional boards manager URLs.
    2. Open the Boards Manager, and install the ESP32 or ESP8266 boards manager.

- ESP8266WiFi Library: `https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html`
- PubSubClient Library (MQTT): `https://www.arduino.cc/reference/en/libraries/pubsubclient/`