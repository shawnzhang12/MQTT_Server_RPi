#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <ADXL362.h>

ADXL362 xl;


int16_t XValue, YValue, ZValue, Temperature;

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

  char X[50];
  char Y[50];
  char Z[50];
  String xval = String(XValue, 3);   
  String yval = String(YValue, 3);   
  String zval = String(ZValue, 3);   

  String xresult = String("The x_accel value is " + xval);  
  String yresult = String("The y_accel value is " + yval);  
  String zresult = String("The z_accel value is " + zval);  
  xresult.toCharArray(X, 30);
  yresult.toCharArray(Y, 30);
  zresult.toCharArray(Z, 30);
  client.publish(topic, X);
  client.publish(topic, Y);
  client.publish(topic, Z);
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
      client.publish(topic, "hello from ESP8266, accelerometer data");
      // ... and resubscribe
      // client.subscribe(topic);
    } else {
      Serial.printf("MQTT failed, state %s, retrying...\n", client.state());
      // Wait before retrying
      delay(2500);
    }
  }
}
