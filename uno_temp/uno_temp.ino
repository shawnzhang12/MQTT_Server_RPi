
/** RF24Mesh_Example.ino by TMRh20
   This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
   master node.
   The nodes will refresh their network address as soon as a single write fails. This allows the
   nodes to change position in relation to each other and the master node.
*/


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
//#include <printf.h>
#include <DHT.h>

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**
   User Configuration: nodeID - A unique identifier for each radio. Allows addressing
   to change dynamically with physical changes to the mesh.
   In this example, configuration takes place below, prior to uploading the sketch to the device
   A unique value from 1-255 must be configured for each node.
   This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.
 **/
#define nodeID 1

uint32_t displayTimer = 0;
float t = 0;

struct payload_temp {
  float t;
};

struct payload_accel {
  uint16_t xyz;
};

void setup() {

  Serial.begin(115200);
  //printf_begin();
  dht.begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}


void loop() {

  mesh.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();
    t = dht.readTemperature();

    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&t, 'T', sizeof(t))) {

      // If a write fails, check connectivity to the mesh network
      if ( ! mesh.checkConnection() ) {
        //refresh the network address
        Serial.println("Renewing Address");
        if(!mesh.renewAddress()){
          //If address renewal fails, reconfigure the radio and restart the mesh
          //This allows recovery from most if not all radio errors
          mesh.begin();
        }
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: "); Serial.println(t);
    }
  }

  while (network.available()) {
    RF24NetworkHeader header;
     network.peek(header);
      payload_temp payload_t;
      payload_accel payload_a;
      switch(header.type){
      case 'T': network.read(header, &payload_t, sizeof(payload_t));
      case 'X': network.read(header, &payload_a, sizeof(payload_a));
      case 'Y': network.read(header, &payload_a, sizeof(payload_a));
      case 'Z': network.read(header, &payload_a, sizeof(payload_a));
      default: network.read(header,0,0); Serial.println(header.type);break;
      }
  }
}
