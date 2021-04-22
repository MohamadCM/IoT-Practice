/*
 * WiFi access point mode
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/

#include <ESP8266WiFi.h>

IPAddress local_IP(192, 168, 15, 20);
IPAddress gateway(192, 168, 15, 20);
IPAddress subnet(255, 255, 255, 0);

void setup()
{
  Serial.begin(9600);
  Serial.println();
  delay(3000);
  
  Serial.print("Setting Access Point ... ");
  boolean result = WiFi.softAP("NODEMCU", "12345678");
  if(result == true) {
    Serial.println("Ready");
    Serial.print("Soft-AP initial IP address = ");
    Serial.println(WiFi.softAPIP()); // Initial IP
    Serial.println("---------------------------");
    delay(3000);

    // Changin configuration
    Serial.print("Setting soft-AP configuration ... ");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Setting soft-AP ... ");
    Serial.println(WiFi.softAP("NODEMCU_Changed") ? "Ready" : "Failed!");

    Serial.print("Soft-AP new IP address = ");
    Serial.println(WiFi.softAPIP());
  }
  else{
    Serial.println("Failed!");
  }
}

void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
}
