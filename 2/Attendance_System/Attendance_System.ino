/*
 * RFID Serial Monitor Write With NTP
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 4 //Signal Input
#define RST_PIN 5 // Reset Input
#define buzzer 16 // Buzzer output

const char *ssid     = "Irancell-TD-GP2101PLUS-3209";
const char *password = "12345678";

//for converting NTP into local time zone
const long utcOffsetInSeconds = (3600 * 4) + 1800;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ir.pool.ntp.org", utcOffsetInSeconds);

//sets up the rfid
MFRC522 mfrc522(SS_PIN, RST_PIN); 

char st[20];

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);     // initializing digital pin LED_BUILTIN as an output.
  pinMode(buzzer, OUTPUT);          // initializing digital pin buzzer as an output.
  SPI.begin();                      // initializing SPI bus
  mfrc522.PCD_Init();               // initializng Proximity Coupling Device for sacanning

  Serial.println("RFID Scan Ready!");
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(3000);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    Serial.print ( "." );
    delay ( 500 );
  }
  Serial.println("WiFi connected!");
  timeClient.begin();
}

void loop() {
  timeClient.update();
  if ( ! mfrc522.PICC_IsNewCardPresent()) { // Look for new cards
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { // Select one of the cards
    return;
  }
  Serial.print("Tag :");
  String tag= "";
  byte character;
  //reads the information from rfid tag
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     tag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.println("Time : ");
  tag.toUpperCase();
  //showing time on the serial monitor
  timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  

  //checking if the rfid is valid based on scan time
  if (timeClient.getHours()< 15 && timeClient.getHours() > 8) 
  {
    Serial.println("TAG checked - Access allow !");
    Serial.println();
    digitalWrite(LED_BUILTIN, LOW); //Turns led on for 3 seconds
    delay(3000);
    }
  else
  {
    Serial.println("Unknown tag - Access refused !!!");
    Serial.println();
    digitalWrite(buzzer, HIGH);     // Turn-on the buzzer for 3 seconds
    delay(3000);
  }
  delay(1000);
}
