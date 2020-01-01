#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
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
  char result[256];

  String num = String(t, 2);   
  String head = String("The temperature is ");  
  String sresult = String(head + num);  // concatenating two strings
  
  sresult.toCharArray(result, 30);
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
    String clientId = "ESP8266-TEMP";
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
