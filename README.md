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

#### Accelerometer (ADXL362)

Setting up the accelerometer is quite simple, as the pins are already in order as the ESP8266 requires them to be (The CLK, MOSI, MISO, CS...). Now just upload the code given in esp8266_accel and it should output the x, y, z values for acceleration.



<img src="images/adxl362_nodemcu.jpg" width="200">



#### Temperature Sensor (DHT 22 (AM2302))

For the sensor, connect Pin 1 to Vin on the ESP8266, Pin 2 to GPIO2 (any GPIO pin will work), and Pin 4 to Gnd. Code is given under the esp8266_temp folder. Notice that the sensor gives readings around 17-18 degrees, which is quite cold but makes sense as it was in a basement. There was a slow temperature rise as I held my finger on the sensor.

<img src="images/dht2_nodemcu.jpg" width="400">



