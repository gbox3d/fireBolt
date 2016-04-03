// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 

Servo myservo;
 
void setup() 
{ 
  pinMode(13,OUTPUT);
} 

void loop() 
{ 
  myservo.attach(11);
  delay(1000);
  digitalWrite(13,HIGH);
  myservo.write(90);
  delay(1000);
  myservo.detach();
  digitalWrite(13,LOW);
  delay(1000);
  
}



