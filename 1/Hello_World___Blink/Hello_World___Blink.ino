/*
 * Simple Blink and Serial Monitor Hello World
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/

// the setup function runs once when you press reset or power the board
void setup() {
  delay(2000);                     // wait for two seconds before starting
  Serial.begin(9600);              // initialize serial monitor with baud_rate = 9600
  Serial.print("Hello, World!");   // Print the statement
  pinMode(LED_BUILTIN, OUTPUT);    // initialize digital pin LED_BUILTIN as an output.
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // wait for a second
}
