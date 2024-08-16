# MQTT Topics and Payloads Documentation

## Overview
This document shares details on the project's MQTT topics, payloads, and their respective purposes used in the system. It also indicated when the topic should be QoS 0, QoS 1, and/or retain the published packets.
---

## 1. Topic Structure

### General Topic Format

### Topic Segments:
- **room**: The specific room (e.g., `room_1`, `room_2`).
- **component**: The device or sensor (e.g., `vent`, `dht_sensor`).
- **detail**: Depends on the device/sensor, documented below for the specific component.
- Ex. Full Topic: `room_1/vent/power_mode`
---

## 2. Topics
- Any "Get" topics are to be published by the components, and any "Set" topics are to be subscribed to by the components.

### 2.1 DHT Sensor Component

#### Get Current Temperature Celsius
- **Topic**: `room_x/dht_sensor/temperature/celsius`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the current temperature in celcius from the sensor.
- QoS: 0

#### Get Current Temperature Ferenheit
- **Topic**: `room_x/dht_sensor/temperature/ferenheit`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the current temperature in ferenheit from the sensor.
- QoS: 0

#### Get Current Humidity
- **Topic**: `room_x/dht_sensor/humidity`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the current humidity percentage from the sensor.
- QoS: 0

#### Get Indicator Light Status
- **Topic**: `room_x/dht_sensor/light`
- **Payload**: 
  - **Type**: null
  - **Description**: Requests the current status of the light.
- QoS: 0
- Retain: True

#### Get Calibrate Offset
- **Topic**: `room_x/dht_sensor/calibrate`
- **Payload**: 
  - **Type**: null
  - **Description**: Requests the current temperature offset integer.
- Retain: True
 
#### Set Indicator Light Status
- **Topic**: `room_x/dht_sensor/light`
- **Payload**: 
  - **Type**: ON or OFF
  - **Description**: Sets the current status light to ON or OFF

#### Set Calibrate Offset
- **Topic**: `room_x/dht_sensor/calibrate`
- **Payload**: Any integer between -10 to 10
  - **Type**: int
  - **Description**: Offsets the temperature by an integer.

#### Last Will and Testament (LWT)
- **Topic**: `room_x/dht_sensor/status`
- **Payload**: "offline", "online"
- **QoS**: `1`
- **Retain**: `True`
- **Description**: Sent when the device goes offline unexpectedly.

### 2.2 Vent Component

#### Get Current Vent Angle
- **Topic**: `room_x/vent/angle`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the current vent angle from the component.
- QoS: 1
- Retain: True
#### Get Current Vent Closure State
- **Topic**: `room_x/vent/closure_state`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the current vent closure state from the component.
- QoS: 1
- Retain: True
#### Get Current Vent Power Mode
- **Topic**: `room_x/vent/power_mode`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the current vent power mode from the component.
- QoS: 1
- Retain: True
#### Get Current Vent Climate Mode
- **Topic**: `room_x/vent/climate_mode`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the current vent climate mode from the component.
- QoS: 1
- Retain: True
#### Get Current Vent Control Mode
- **Topic**: `room_x/vent/control_mode`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the current control mode from the component.
- QoS: 1
- Retain: True
#### Get Current Vent Desired Temperature
- **Topic**: `room_x/vent/desired_temp`
- **Payload**: 
  - **Type**: null
  - **Description**: Request the desired temperature from the vent component.
- QoS: 1
- Retain: True

#### Get Desired Temperature Reached State
- **Topic**: `room_x/vent/desired_temp_state`
- **Payload**: 
  - **Type**: null
  - **Description**: Request to see if the desired temperature has been reached, or if it's still ongoing.
- QoS: 1
- Retain: True

#### Set Current Vent Angle
- **Topic**: `room_x/vent/angle`
- **Payload**: An integer between 0 and 90
  - **Type**: int
  - **Description**: Sets the current vent angle.
- QoS: 1

#### Set Current Vent Closure State
- **Topic**: `room_x/vent/closure_state`
- **Payload**: "Closed", "Semi-Closed", "Open", "Semi-Open"
  - **Type**: String
  - **Description**: Sets the current vent closure state.
- QoS: 1

#### Set Current Vent Power Mode
- **Topic**: `room_x/vent/power_mode`
- **Payload**: "Default", "Eco", "Max-Power"
  - **Type**: String
  - **Description**: Sets the current vent power mode.
- QoS: 1

#### Set Current Vent Climate Mode
- **Topic**: `room_x/vent/climate_mode`
- **Payload**: "Heating", "Cooling"
  - **Type**: String
  - **Description**: Sets the current vent climate mode.
- QoS: 1

#### Set Current Vent Control Mode
- **Topic**: `room_x/vent/control_mode`
- **Payload**: "Auto", "Manual"
  - **Type**: String
  - **Description**: Sets the current control mode.
- QoS: 1

#### Set Current Vent Desired Temperature
- **Topic**: `room_x/vent/desired_temp`
- **Payload**: Any integer between 17 to 26.
  - **Type**: int
  - **Description**: Sets the desired temperature.
- QoS: 1

#### Last Will and Testament (LWT)
- **Topic**: `room_x/vent/status`
- **Payload**: "offline", "online", "eco-online"
- **QoS**: `1`
- **Retain**: `True`
- **Description**: Sent when the device goes offline unexpectedly or due to eco mode.

---

## 3. QoS and Retain Flag

### Quality of Service (QoS)
- **QoS 0**: Best effort, no acknowledgment.
- **QoS 1**: Ensured delivery, message delivered at least once.
- **QoS 2**: Exactly once delivery, message is delivered only once.

### Retain Flag
- **True**: The last message sent on this topic is saved by the broker.
- **False**: The broker does not save the last message.

### Practical Reasons
- QoS: QoS 1 ensures the message which has been published/subscribed to will establish connection atleast once. (ex. your device runs out of battery and you charge it back up, it will get the message after it's back alive).
  - All vent control should be QoS 1 to increase user experience. 
- Retain: Allows published packets to keep it's value on the borker, such that the retained flags of the client act as a last save restoration, pulling info from the broker itself.


---
