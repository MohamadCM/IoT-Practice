/*
 * Simple LDR and buzzer control sequence
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/

#define ldr A0    // LDR input
#define buzzer 16 // Buzzer output

// Scaling values
#define rMin 0
#define rMax 1024
#define tMin 0
#define tMax 100

void setup() {
  Serial.begin(9600);
  pinMode(ldr, INPUT);              // initializing ldr pin as an analog input.
  pinMode(LED_BUILTIN, OUTPUT);     // initializing digital pin LED_BUILTIN as an output.
  pinMode(buzzer, OUTPUT);          // initializing digital pin buzzer as an output.
}

void loop() {
  int scaledValue = scaleNumber(analogRead(ldr)); // Reading value and then scaling it
  Serial.println(scaledValue);

  if(scaledValue > 50){               //  Built-in LEDs operate in “inverted” power mode
      digitalWrite(LED_BUILTIN, LOW); // Turn-on the LED
      digitalWrite(buzzer, HIGH);     // Turn-on the buzzer
      delay(500);
  } else if (scaledValue < 50){
      digitalWrite(LED_BUILTIN, HIGH); // Turn-off the LED
      digitalWrite(buzzer, LOW);     // Turn-off the buzzer
      delay(500);
  }
}

int scaleNumber(int number){
  float m = (float) number;
  int result = ((m - rMin) / (rMax - rMin)) * (tMax - tMin) + tMin;
  return result;
}
