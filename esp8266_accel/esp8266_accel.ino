#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <ADXL362.h>
#include <stdlib.h>
#include <stdio.h>

ADXL362 xl;

int16_t temp;
int16_t XValue, YValue, ZValue, Temperature;
char *XString, *YString, *ZString;
String X, Y, Z;

// max received message length
#define MAX_MSG_LEN (128)

//Wifi configuration
const char* ssid = "ssid";
const char* password = "password";


//MQTT configuration
const char *serverHostname = "raspberrypi";
const char *topic = "test/accel";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  xl.begin(10);                   // Setup SPI protocol, issue device soft reset
  xl.beginMeasure();              // Switch ADXL362 to measure mode  
  Serial.println("Start Demo: Simple Read");
  
  connectWifi();
  client.setServer(serverHostname, 1883);
  //client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    connectMQTT();
  }
  xl.readXYZTData(XValue, YValue, ZValue, Temperature); 

  X = String(XValue);
  X.toCharArray(XString, 10);
  Y = String(YValue);
  Y.toCharArray(YString, 10);  
  Z = String(ZValue);
  Z.toCharArray(ZString, 10);


  client.publish(topic, XString);
  client.publish(topic, YString);  
  client.publish(topic, XString);
  client.loop();
  delay(500);
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
