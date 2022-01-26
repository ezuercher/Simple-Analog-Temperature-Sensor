/*
 * Analog Temperature Sensor
 * Sketch to analyze data collection
 * By Elliot Zuercher
 */

//Include libraries
#include <MCP3008.h>

//Define pin connections
#define CS_PIN D8
#define CLOCK_PIN D5
#define MOSI_PIN D7
#define MISO_PIN D6

//Create an MCP3008 object
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

void setup() {
 
  //Open serial port for debugging
  Serial.begin(9600);
  
}

void loop() {

  delay(3000);
  
  for (int i = 0; i < 500; i++) {
    Serial.println(adc.readADC(1, false));
    delay(1);
  }

  delay(1000000);
  
}
