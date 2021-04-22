/*
 * WiFi controll using HTML
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/


#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 

// Constans
#define ldr A0    // LDR input
const String ssid = "Irancell-TD-GP2101PLUS-3209"; // WiFi connection username
const String password = "12345678"; // WiFi connection password
const int buff = 4000; // HTML buffer size
char temp[buff]; // HTML buffer
ESP8266WebServer server(80); // Server object

void setup(void){
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);     // initializing LED_BUILTIN pin as an digital output.
  pinMode(ldr, INPUT);              // initializing ldr pin as an analog input.
  Serial.println("Connecting to network");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);   // Connecting to WiFi network

  while (WiFi.status() != WL_CONNECTED){   // Trying to connect
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // IP address is needed for connection
  Serial.println("--------------------------------------------------------------");

  // Setup server routes
  server.on("/", sendHTML);
  server.on("/1", showLDR);
  server.on("/2", turnOnLED);
  server.on("/3", turnOffLED);

  // Initialize server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}


void turnOnLED() { // Turning LED on
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  server.send(200, "text/html", temp);
}

void turnOffLED() { // Turning LED off
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  server.send(200, "text/html", temp);
}

void showLDR() {
  int ldrVal = analogRead(ldr); // Reading value and then scaling it
  Serial.print("LDR value is: ");
  Serial.println(ldrVal);
  delay(500);
}

void sendHTML() { // Sending HTML page
  snprintf(temp, buff,
"<html>\
  <head>\
  <style>\
  </style>\
  </head>\
    <body>\ 
       <h1 style=\"/margin: 35px;\">IoT homework</h1>\
       <a href =\"/1\"> <button style=\"/font-family:century gothic;background: none;box-shadow: 0 1px 3px rgba(0,0,0,0.12), 0 1px 2px rgba(0,0,0,0.24);font-size: 16px;border-radius: 24px;border: 0 solid;background: #FFC80C;padding: 13px 40px;margin: 35px;\">Show LDR input!</button></a>\
       <a href =\"/2\"> <button style=\"/font-family:century gothic;background: none;box-shadow: 0 1px 3px rgba(0,0,0,0.12), 0 1px 2px rgba(0,0,0,0.24);font-size: 16px;border-radius: 24px;border: 0 solid;background: #304FFE;padding: 13px 40px;margin: 35px;\">Turn On the LED!</button></a>\
       <a href =\"/3\"> <button style=\"/font-family:century gothic;background: none;box-shadow: 0 1px 3px rgba(0,0,0,0.12), 0 1px 2px rgba(0,0,0,0.24);font-size: 16px;border-radius: 24px;border: 0 solid;background: #D50000;padding: 13px 40px;margin: 35px;\">Turn Off the LED!</button></a>\
       </body>\
</html>");
  server.send(200, "text/html", temp);
}
