/*
Blink just once

Adapted from blink
 */

int pin = 8;
int single_delay = 1000;

void setup() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); 
  delayMicroseconds(single_delay);
  digitalWrite(pin, HIGH);
}

// the loop function runs over and over again forever
void loop() {
delay(1000);
}
