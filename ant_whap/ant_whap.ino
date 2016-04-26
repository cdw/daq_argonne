/* whap the ant
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  //Serial.begin(115200);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  if(analogRead(A0)>500){
    myservo.write(50);
  }
  else{
    myservo.write(0);
  }
      delayMicroseconds(10);
      //Serial.println(analogRead(A0));    
}

