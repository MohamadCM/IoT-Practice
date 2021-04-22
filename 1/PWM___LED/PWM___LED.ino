/*
 * Simple LED and LDR control sequence
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/


#define ldr A0    // LDR input

// Scaling values
#define rMin 0
#define rMax 1024
#define tMin 0
#define tMax 100

void setup() {
  Serial.begin(9600);
  pinMode(ldr, INPUT);              // Initializing analog pin ldr as an INPUT.
  pinMode(LED_BUILTIN, OUTPUT);    // Initializing digital pin LED_BUILTIN as an output.
}

void loop() {
  int scaledValue = scaleNumber(analogRead(ldr)); // Reading value and then scaling it
  Serial.println(scaledValue);
  controlLight( (float) scaledValue / 100 );
}

int scaleNumber(int number){
  float m = (float) number;
  int result = ((m - rMin) / (rMax - rMin)) * (tMax - tMin) + tMin;
  return result;
}

void controlLight(float dutyCycle){
    analogWrite(LED_BUILTIN, 255 - (dutyCycle * 255)); // PWM on LED output
    delay(100);
}
