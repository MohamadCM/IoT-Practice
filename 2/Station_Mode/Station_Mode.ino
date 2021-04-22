/*
 * WiFi scan in station mode
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/

#include <ESP8266WiFi.h>

void setup()
{
  Serial.begin(9600);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(3000);

  Serial.print("Scanning... ");
  int n = WiFi.scanNetworks(); // Get number of found WiFis
  Serial.print(n);
  Serial.println(" network(s) found");
  for (int i = 0; i < n; i++) {
    Serial.println(WiFi.SSID(i)); // Print found WiFis
  }
  delay(3000);
  
  WiFi.begin("Irancell-TD-GP2101PLUS-3209", "12345678");
  while (WiFi.status() != WL_CONNECTED) { // Trying again until it can connect
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{ 
// No loop statement
}
