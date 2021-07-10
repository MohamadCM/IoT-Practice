/*
 * MQTT Conection and Pub-Sub 
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MFRC522.h>
#include <SPI.h>
#include <string.h>

//Use a WiFi
#define wifi_ssid "Irancell-TD-GP2101PLUS-3209"
#define wifi_password "12345678"

#define interval 2000 // MQTT intervals
#define server "192.168.1.114"
#define mqtt_user "Mohamad"
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

void callback(char * topic, byte * payload, unsigned int length);
void reconnect();
String read_rfid();

void blikng_led(bool);
void modify_temp_led(int);
void modify_water_led(int);

int read_water_level();

WiFiClient espClient;
PubSubClient client(espClient);
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  delay(2000);

  Serial.begin(9600);
  client.setServer(server, 1883); // initialize connection to MQTT Broker
  client.setCallback(callback);
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
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long current_time = millis();
  if (current_time - last_time >= interval) {
    int sensorValue = analogRead(A0); // read the input on analog pin 0
    String id = read_rfid(); // Read RFID Value
    if (id.length() > 0) {
      float t = 1.2;
      String temp = String(t);
      client.publish("RFID", id.c_str());
      Serial.print("RFID: ");
      Serial.print(id);
      Serial.println("Message published");
      last_time = current_time;
    }
  }
  blikng_led(requested_water_level > read_water_level());
  modify_water_led(read_water_level());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_user)) {
      Serial.println("connected, Subsribing to topics!");

      client.subscribe("temperature");
      client.subscribe("water");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char * topic, byte * payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String complete_payload = "";
  for (int i = 0; i < length; i++) {
    complete_payload.concat((char) payload[i]);
  }
  Serial.println(complete_payload);
  if (strcmp(topic, "water") == 0)
    requested_water_level = complete_payload.toInt();
  if (strcmp(topic, "temperature") == 0)
    modify_temp_led(complete_payload.toInt());
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
