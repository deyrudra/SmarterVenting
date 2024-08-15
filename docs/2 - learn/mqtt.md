# MQTT
- MQTT stands for Message Queuing Telemetry Transport
- Messaging protocol used for communcation between IoT devices.
- Features of MQTT:
    - Publish/Subscribe Model:
        - Clients (our devices) *publish* messages to a model called a **broker**. This broker then redistributes these message to all subscribers of their respective topics.
    - Lightweight
    - Reliable
    - Scalability

- Publish/Subscribe (PUB/SUB) Model in Depth
    - Broker:
        - Receives all messages from publishers.
        - Filters the received messages based on topics.
        - Distributes the messages to appropriate subscribers
    - Topics:
        - A topic is just a string. It serves to route messages.
        - Topics resemble file paths: `home/kitchen/temperature`.
            - This means topics are hierarchial.
        - Publishers send messages to specific topics, subscribers express interest in one or more topics.
        
    - Publisher:
        - A device or application that sends messages to the broker under specific topics.
        - These publishers are only concerned about sending their data.
            - There is no concern about who the subscribers are, just need to send the data to the broker with the topic.
        - Ex. temperature sensor in smart home can publlish readings to the topic `home/kitchen/temperature`.
    - Subscriber:
        - A device or application that has interest in one or more topics.
        - Subscribers receive all messages published to their topic.
        - Ex. a smart thermostat might subscribe to `home/kitchen/temperature` to receive updates and adjust the temperature accordingly.
        - Under the hierarchy, if a subscriber chooses to, they can subscribe to multiple topics at once, without having to specifiy each one. They can just simply subscribe to `home/kitchen/#`. The `#` refers to all subtopics under the `home/kitchen` topic.

- QoS: QoS 1 ensures the message which has been published/subscribed to will establish connection atleast once. (ex. your device runs out of battery and you charge it back up, it will get the message after it's back alive).

- Retain: Allows published packets to keep it's value on the borker, such that the retained flags of the client act as a last save restoration, pulling info from the broker itself.
## Setting up an MQTT Broker on Home Assistant
- The guides I followed:
    - https://www.home-assistant.io/integrations/mqtt/#:~:text=press%20%E2%80%9CPUBLISH%E2%80%9D%20.-,Go%20to%20Settings%20%3E%20Devices%20%26%20services.,Select%20Publish.
    - https://github.com/home-assistant/addons/blob/master/mosquitto/DOCS.md
    - https://www.youtube.com/watch?v=WYyPMHqMV-w
