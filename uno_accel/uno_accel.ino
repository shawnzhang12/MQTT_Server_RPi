#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <ADXL362.h>

/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

ADXL362 xl;
#define nodeID 2 //accel ID

uint32_t displayTimer = 0;
int16_t XValue, YValue, ZValue, Temperature;

struct payload_temp {
  float t;
};

struct payload_accel {
  uint16_t xyz;
};

void setup() {
  Serial.begin(115200);
  xl.begin(10);                   // Setup SPI protocol, issue device soft reset
  xl.beginMeasure();              // Switch ADXL362 to measure mode  
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

    xl.readXYZTData(XValue, YValue, ZValue, Temperature); 


    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&XValue, 'X', sizeof(XValue)) || !mesh.write(&YValue, 'Y', sizeof(YValue)) || !mesh.write(&ZValue, 'Z', sizeof(ZValue))) {

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
      Serial.print("Send OK: "); Serial.println(XValue); Serial.println(YValue); Serial.println(ZValue);
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
