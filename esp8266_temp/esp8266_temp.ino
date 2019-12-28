#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <stdlib.h>
#include <stdio.h>
#include <DHT.h>

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

int16_t temp;

// max received message length
#define MAX_MSG_LEN (128)

//Wifi configuration
const char* ssid = "CIK1000M_AC-3bd4";
const char* password = "3c9066b63bd4";


//MQTT configuration
const char *serverHostname = "raspberrypi";
const char *topic = "test/temp";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  dht.begin();

  connectWifi();
  client.setServer(serverHostname, 1883);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    connectMQTT();
  }

  float t = dht.readTemperature();
  char result[8];
  
  dtostrf(t, 6, 2, result);
  result = "The temperature is " + result;
  client.publish(topic, result);
  client.loop();
  delay(2000);
}

void connectWifi() {
  delay(10);

  Serial.printf("\nConnecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected on IP address ");
  Serial.println(WiFi.localIP());
}

// connect to MQTT server
void connectMQTT() {
  // Wait until we're connected
  while (!client.connected()) {
    String clientId = "ESP8266-ACCEL";
    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
      client.publish(topic, "hello from ESP8266");
      // ... and resubscribe
      // client.subscribe(topic);
    } else {
      Serial.printf("MQTT failed, state %s, retrying...\n", client.state());
      // Wait before retrying
      delay(2500);
    }
  }
}
