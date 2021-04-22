/*
 * Simple Binary Counter using LEDs
 * @author Mohamad ChamanMotlagh 9631018
 * @author Mohamad Jabbari Zadehgan 9631015
*/

// Defining GPIO pins
#define led2 16 // (D0) = MSB
#define led1 5 // (D1)
#define led0 4 // (D2) = LSB

void setup() {
  Serial.begin(9600);
  // Initializing pins
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

int count = 0;
void loop() {
  int binaryArray[3] = {0, 0, 0};
  convertNumberIntoArray(decimalToBinary(count), binaryArray); // Converting Number into binary, then putting it inside and array
  // Writing Array bits to LEDs
  digitalWrite(led2, binaryArray[2] ? HIGH : LOW);
  digitalWrite(led1, binaryArray[1] ? HIGH : LOW);
  digitalWrite(led0, binaryArray[0] ? HIGH : LOW);
  Serial.print("\n");
  delay(1000);
  count = count < 7 ? (count + 1) : 0; // Increamenting counter between 0 to 7
}

int decimalToBinary(int decimalnum) // Convert a decimal number to binary
{
    int binarynum = 0;
    int rem, temp = 1;

    while (decimalnum != 0)
    {
        rem = decimalnum % 2;
        decimalnum = decimalnum / 2;
        binarynum = binarynum + rem * temp;
        temp = temp * 10;
    }
    return binarynum;
}

void convertNumberIntoArray(int number, int arr[]) // Converting binary number into an array
{
    int len = 3;
    for(int i = 0; i < len; i++){
      arr[i] = number % 10;
      number /= 10;
    }
}
