/*
 * Analog Temperature Sensor
 * By Elliot Zuercher
 */

//Include libraries
#include <MCP3008.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

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

//Define wifi credentials
#define NETWORK ""
#define PASSWORD ""

//Define function-like macros
#define CtoF(c) (((9.0 / 5.0) * c) + 32)

//Create an MCP3008 object
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

//Create a webserver object
ESP8266WebServer server(80);

//Store an HTML page in PROGMEM
static const char tempPage[] PROGMEM = R"=====(
 <!DOCTYPE html>
<html>
<head>
 <style>
  body {
    background-color: #1f2227;
  }
  .title {
    background-color: #1a1d21;
    color: white;
    font-size: 60px;
    box-shadow: 0px 0px 8px 0px black;
    margin: 10px;
  }
  .err {
    display: none;
    background-color: #fff794;
    border: 2px solid #888;
    border-radius: 10px;
    text-align: center;
    font-size: 50px;
  }
  .display {
    background-color: #fff794;
    border: 2px solid #888;
    border-radius: 10px;
    text-align: center;
    font-size: 50px;
  }
  .element {
    margin: 10px;
  }
 </style>
</head>
<body>
  <h1 class="title">Laundry Room Temperature</h1>
  <div id="errDisplay" class="err">
    <span id="err"></span>
  </div>
  <div id="tempDisplay" class="display">
    <div class="element">
      <span id="TempC"></span>
      <span> C</span>
    </div> <hr>
    <div class="element">
      <span id="TempF"></span>
      <span> F</span>
    </div>
  </div>
</body>
<script>
  function fetchData() {
    fetch("http://laundryroom.local/readings", { method: "POST"}).then(function(response) {
      if (response.status !== 200) {
        document.getElementById("tempDisplay").style.display = "none";
        document.getElementById("errDisplay").style.display = "block";
        document.getElementById("err").innerText = response.status;
      }
      else {
        document.getElementById("tempDisplay").style.display = "block";
        document.getElementById("errDisplay").style.display = "none";
        console.log(response)
        response.json().then(function(data) {
          document.getElementById("TempC").innerText = data[0];
          document.getElementById("TempF").innerText = data[1];
        });
      }
    });
  };
  //Fetch the data on page load, and every 5 seconds thereafter
  fetchData();
  setInterval(fetchData, 5000);
</script>
</html>
)=====";

//Function stubs
double calculateR(int d);
double calculateTempC(double r);
void handleNewConnection();
void sendNewData();

void setup() {
 
  //Open serial port for debugging
  Serial.begin(9600);

  //Connect to WiFi
  WiFi.begin(NETWORK, PASSWORD);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  //Setup MDNs for convenience
  if (!MDNS.begin("laundryroom", WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }

  //Setup the webserver
  server.on("/", handleNewConnection);
  server.on("/readings", sendNewData);
  server.begin();

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  
}

//Function that converts a digital value into a value of resistance
//Inputs: The value to be converted
double calculateR(int d) {

  double res = (R * d) / (1024 - d);
  return res;
  
}

//Function that converts a resistance value into a temperature value in Celsius
//Inputs: The resistance value to be converted, in Ohms
double calculateTempC(double r) {

  double tempK = (1 / (A + (B * log(r)) + (C * pow(log(r), 3))));
  return tempK - 273.15;
  
}

//Function that sends the HTML page to a client
void handleNewConnection() {
  server.send(200, "text/html", tempPage);
}

//Function that sends updated data to the webpage
void sendNewData() {
  //Using differential signalling, read channel 1
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

  ///Send the data to the client
  String JSONData = "[" + (String)temp + "," + (String)CtoF(temp) + "]";
  server.send(200, "text", JSONData);
}

void loop() {

  //Handle our clients
  server.handleClient();
  MDNS.update();
  
}
