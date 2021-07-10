/*
 * HTTP Conection and Client-Server 
 * @author Mohamad ChamanMotlagh 9631018
 */
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <MFRC522.h>
#include <SPI.h>
#include <string.h>

//Use a WiFi
#define wifi_ssid "Irancell-TD-GP2101PLUS-3209"
#define wifi_password "mocm1999"

#define interval 2000 // HTTP intervals
String serverName = "http://192.168.1.114:3000/";
String getValue(String data, char separator, int index);
int requested_temprature = 0;
int requested_water_level = 0;

// RFID module PINs
#define SS_PIN 2 //Signal Input
#define RST_PIN 0 // Reset Input

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

void callback(String payload);
void reconnect();
String read_rfid();

void blikng_led(bool);
void modify_temp_led(int);
void modify_water_led(int);

int read_water_level();

WiFiClient espClient;
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
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
  delay(2000);
}

long last_time = 0;
void loop() {

  long current_time = millis();
  if (current_time - last_time >= interval) {
    int sensorValue = analogRead(A0); // read the input on analog pin 0
    String id = read_rfid(); // Read RFID Value
    if (id.length() > 0) {
      HTTPClient http;
      id.replace(" ", "%20"); // Make query param encoded
      String serverPath = serverName + "?RFID=" + id;

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        callback(payload);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();

      last_time = current_time;
    }
  }
  blikng_led(requested_water_level > read_water_level());
  modify_water_led(read_water_level());
}

void callback(String payload) {
  String temprature = getValue(payload, ',', 0);
  String water = getValue(payload, ',', 1);

  Serial.print("Message arrived [");
  Serial.println(temprature);
  Serial.println(water);
  Serial.print("] ");
  String temp_value = getValue(temprature, '=', 1);
  String water_value = getValue(water, '=', 1);

  requested_water_level = water_value.toInt();
  modify_temp_led(temp_value.toInt());
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
