/*
 * Analog Temperature Sensor
 * Sketch to analyze data collection
 * By Elliot Zuercher
 */

//Include libraries
#include <MCP3008.h>
#include <math.h>

//Define pin connections
#define CS_PIN D8
#define CLOCK_PIN D5
#define MOSI_PIN D7
#define MISO_PIN D6

//Set this to Vref
#define V 1.0
//Set this to the sum of the other resistances in the resistor ladder
#define R 10440
//Define the constants for this thermistor
#define A 0.00091233
#define B 0.00028452
#define C -0.00000020344
//Define the B value of the resistor, if desired to do so this way
#define BVALUE 3950000

//Define function-like macros
#define CtoF(c) (((9.0 / 5.0) * c) + 32)

//Create an MCP3008 object
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

//Function stubs
double calculateR(int d);
double calculateTempC(double r);

void setup() {
 
  //Open serial port for debugging
  Serial.begin(9600);
  
}

//Function that converts a digital value into a value of resistance
//Inputs: The value to be converted
double calculateR(int d) {

  double res = (R * d) / (1024 - d);
  return res;
  
}

//Function that converts a resistance value into a temperature value in Celsius
double calculateTempC(double r) {

  double tempK = (1 / (A + (B * log(r)) + (C * pow(log(r), 3))));
  return tempK - 273.15;
  
}

void loop() {

  //Using differential signalling, read channel 0
  int val = adc.readADC(1, false);
  //Convert the reading into a resistance value
  double res = calculateR(val);
  //Convert the resistance into a temperature value
  double temp = calculateTempC(res);

  //Print data to the serial monitor
  Serial.print(temp);
  Serial.print(" C\t");
  Serial.print(CtoF(temp));
  Serial.println(" F");

  delay(100);
  
}
