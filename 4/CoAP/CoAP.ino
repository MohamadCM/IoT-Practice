/*
 * CoAP Conection and Client-Server 
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
 */

#include <ESP8266WiFi.h>

#include <coap-simple.h>

#include <NTPClient.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiUdp.h>


#include <string.h>

//Use a WiFi
#define wifi_ssid "Irancell-TD-GP2101PLUS-3209"
#define wifi_password "12345678"

#define interval 2000 // CoAP intervals
IPAddress ip(192, 168, 1, 114);
//ip address and default port of coap server in which your interested in
int port = 5683;
//instance for coapclient
WiFiUDP udp;
Coap coap(udp);
String getValue(String data, char separator, int index);

// RFID module PINs
#define SS_PIN 2 //Signal Input
#define RST_PIN 0 // Reset Input
MFRC522 mfrc522(SS_PIN, RST_PIN);

//LED PINs
#define BLINKING_LED 16
#define TEMP_LED 4
#define WATER_LED 5

// Water sensor constants
#define sensor A0 // water_level_sensor input
// Scaling values
#define rMin 0
#define rMax 1023
#define tMin 0
#define tMax 100
int requested_temprature = 0;
int requested_water_level = 0;

// coap client response callback
void callback_response(CoapPacket & packet, IPAddress ip, int port);
String read_rfid();

void blikng_led(bool);
void modify_temp_led(int);
void modify_water_led(int);
int read_water_level();

void setup_wifi();

// coap client response callback
void callback_response(CoapPacket & packet, IPAddress ip, int port) {
  //receives the payload which subsumes the water and temperature details
  Serial.print("Reponse:   ");
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  Serial.println(p);
  int breakpoint = 0;
  for (int i = 0; i < strlen(p); i++) {
    char c = message[i];
    if (c == ',')
      breakpoint = i;
  }
  String water;
  String temp;
  water = message.substring(0, breakpoint);
  temp = message.substring(breakpoint + 1, strlen(p));
  Serial.println(water);
  Serial.println(temp);
  String temp_value = getValue(temp, '=', 1);
  String water_value = getValue(water, '=', 1);

  requested_water_level = water_value.toInt();
  modify_temp_led(temp_value.toInt());
}

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.begin(9600);
  setup_wifi(); // Connect ot WiFi

  SPI.begin(); // initializing SPI bus
  mfrc522.PCD_Init(); // initializng Proximity Coupling Device for sacanning

  Serial.println("................");
  Serial.println("RFID Scan Ready!");
  Serial.println("................");

  pinMode(BLINKING_LED, OUTPUT);
  pinMode(TEMP_LED, OUTPUT);
  pinMode(WATER_LED, OUTPUT);
  pinMode(sensor, INPUT); // Initializing analog pin sensor as an INPUT.

  coap.response(callback_response);

  // start coap client
  coap.start();
  delay(2000);
}
long last_time = 0;
void loop() {
  // loop
  String tag = "";
  int current_time = millis();
  if (current_time - last_time >= interval) {
    tag = read_rfid();
    if (tag.length() > 0) {
      Serial.println("-- Sending Request --");
      char message[tag.length() + 1];
      strcpy(message, tag.c_str());
      int msgid = coap.put(ip, port, "getData", message, tag.length());

      last_time = current_time;
    }
  }
  blikng_led(requested_water_level > read_water_level());
  modify_water_led(read_water_level());

  coap.loop();
  delay(200);
}
String read_rfid() {
  if (!mfrc522.PICC_IsNewCardPresent()) { // Look for new cards
    return "";
  }
  if (!mfrc522.PICC_ReadCardSerial()) { // Select one of the cards
    return "";
  }
  Serial.print("Tag found:");
  String tag = "";
  byte character;
  //reads the information from rfid tag
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  return tag;
}

void blikng_led(bool should_blink) {
  if (should_blink) {
    digitalWrite(BLINKING_LED, HIGH);
    delay(400);
    digitalWrite(BLINKING_LED, LOW);
  } else {
    digitalWrite(BLINKING_LED, LOW);
  }
}

void modify_temp_led(int value) {
  int scaled = (int) 1023 * (value / 100.0);
  Serial.println(scaled);
  analogWrite(TEMP_LED, scaled);
  requested_temprature = scaled;
}

void modify_water_led(int value) {
  int scaled = (int) 1023 * (value / 100.0);
  analogWrite(WATER_LED, scaled);
}

int read_water_level() {
  int number = analogRead(sensor);
  float m = (float) number;
  int result = ((m - rMin) / (rMax - rMin)) * (tMax - tMin) + tMin;
  return result;
}

void setup_wifi() {
  delay(1000);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {
    0,
    -1
  };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
