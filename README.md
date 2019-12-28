# MQTT_Server_RPi
For this first part of the project we'll be setting up the a mosquitto MQTT server with a RPi being the server and an ESP8266 as a client. 

- mention topics, messages, subscribers and publishers as the benefit of MQTT

Via the terminal or command line, enter

| `sudo apt install mosquitto mosquitto-clients`

in order to install the mosquitto MQTT broker using apt.

Now to enable the broker, run:

`sudo systemctl enable mosquitto`

The broker status can be confirmed via:

`sudo systemctl status mosquitto`

- mention MQTT topics, and file path





<<<<<<< HEAD
### Setting up the ESP8266 

I used the Arduino IDE, so a couple things that had to be done were:

- Navigate to Tools --> Board, and select "NodeMCU 1.0 (ESP-12E Module)"

- In Tools, Select the correct port where the board is attached
- In Tools --> Manage Libraries, install the PubSubClient library to connect to the MQTT server.

https://appcodelabs.com/introduction-to-iot-how-to-build-a-simple-mqtt-subscriber-using-esp8266

finished this tutorial, can publish to the led on the esp8266
[](images/adxl362_nodemcu.jpg)

- need to figure out how to publish temp and accel data from esp8266 to a topic

  
=======
### Setting up the ESP8266 for Accelerometer (ADXL362)

- fill in after exams
>>>>>>> 06d10b963dbbc43b634afed2964bf84751657739
