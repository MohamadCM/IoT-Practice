/*
 * Simple LED and LDR control sequence
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/


#define sensor A0    // water_level_sensor input
#define buzzer 16 // Buzzer output

// Scaling values
#define rMin 0
#define rMax 1024
#define tMin 0
#define tMax 100


void setup() {
    Serial.begin(9600);
    pinMode(sensor, INPUT);              // Initializing analog pin sensor as an INPUT.
    pinMode(LED_BUILTIN, OUTPUT);    // Initializing digital pin LED_BUILTIN as an output.
}

void loop() {
  int scaledValue = scaleNumber(analogRead(sensor)); // Reading value and then scaling it
  Serial.println(scaledValue);
  controlBuzzer( (float) scaledValue / 100 );
}

int scaleNumber(int number){
  float m = (float) number;
  int result = ((m - rMin) / (rMax - rMin)) * (tMax - tMin) + tMin;
  return result;
}

void controlBuzzer(float dutyCycle){
    analogWrite(buzzer, 255 - (dutyCycle * 255)); // PWM on Buzzer ouput
    delay(100);
}
