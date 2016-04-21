/*
  Blink many

  Used to blink a list of pins
 */

int blinkPins[] = {2,3,4,5,6,7,8,9,10,11,12,13};
int lenPins = 12;
int lowToHigh = 1000; //delay in ms
int highToLow = 1000; //delay in ms

void setup() {
  // initialize digital pin 13 as an output.
  for(int i = 0; i<lenPins; i++){
    pinMode(blinkPins[i], OUTPUT);
  }
}

void loop() {
  for(int i = 0; i<lenPins; i++){
      digitalWrite(blinkPins[i], HIGH);
  }
  delay(lowToHigh);
  for(int i = 0; i<lenPins; i++){
      digitalWrite(blinkPins[i], LOW);
  }
  delay(highToLow);
}
