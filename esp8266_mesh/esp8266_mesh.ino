#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

// max received message length
#define MAX_MSG_LEN (128)

//Wifi configuration
const char* ssid = "CIK1000M_AC-3bd4";
const char* password = "3c9066b63bd4";


//MQTT configuration
const char *serverHostname = "raspberrypi";
const char *topic_temp = "test/temp";
const char *topic_accel = "test/accel";
/***** Configure the chosen CE,CS pins *****/

RF24 radio(2,15);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

WiFiClient espClient;
PubSubClient client(espClient);

uint32_t displayTimer = 0;
int16_t Xval = 0;
int16_t Yval = 0;
int16_t Zval = 0;
float t = 0.0;
  
void setup() {
  Serial.begin(115200);

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  
  connectWifi();
  client.setServer(serverHostname, 1883);
  // Connect to the mesh
  mesh.begin();
}

void loop() {    
   if (!client.connected()) {
    connectMQTT();
  }
  
  // Call mesh.update to keep the network updated
  mesh.update();
  
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();

  // Check for incoming data from the sensors
  if(network.available()){
    RF24NetworkHeader header;
    network.peek(header);
    
    switch(header.type){
      case 'T': network.read(header,&t,sizeof(t)); //Serial.println(t); break;
      case 'X': network.read(header,&Xval,sizeof(Xval)); //Serial.println(X); break;
      case 'Y': network.read(header,&Yval,sizeof(Yval)); //Serial.println(Y); break;
      case 'Z': network.read(header,&Zval,sizeof(Zval)); //Serial.println(Z); break;
      default: network.read(header,0,0); Serial.println(header.type);break; Serial.println("why");
    }
  }
  char X[50];
  char Y[50];
  char Z[50];
  char T[50];

  String num = String(t, 2);  
  String xresult = String("The x_accel value is " + String(Xval, 3));  
  String yresult = String("The y_accel value is " + String(Yval, 3));  
  String zresult = String("The z_accel value is " + String(Zval, 3));
  String tresult = String("The temperature is " + num);
        
  xresult.toCharArray(X, 50);
  yresult.toCharArray(Y, 50);
  zresult.toCharArray(Z, 50);
  tresult.toCharArray(T, 50);
    
  client.publish(topic_accel, X);
  client.publish(topic_accel, Y);
  client.publish(topic_accel, Z);
  client.publish(topic_temp, T);
  
  client.loop();
  delay(200);
  
  if(millis() - displayTimer > 5000){
    displayTimer = millis();
    Serial.println(" ");
    Serial.println(F("********Assigned Addresses********"));
     for(int i=0; i<mesh.addrListTop; i++){
       Serial.print("NodeID: ");
       Serial.print(mesh.addrList[i].nodeID);
       Serial.print(" RF24Network Address: 0");
       Serial.println(mesh.addrList[i].address,OCT);
     }
    Serial.println(F("**********************************"));
  }
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

void connectMQTT() {
  // Wait until we're connected
  while (!client.connected()) {
    String clientId = "ESP8266-MASTER";
    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
      client.publish(topic_accel, "hello from ESP8266, accelerometer data");
      client.publish(topic_temp, "hello from ESP8266, temperature data");
      // ... and resubscribe
      // client.subscribe(topic);
    } else {
      Serial.printf("MQTT failed, state %s, retrying...\n", client.state());
      // Wait before retrying
      delay(2500);
    }
  }
}
