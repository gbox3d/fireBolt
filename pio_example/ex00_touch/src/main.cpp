#include <Arduino.h>

const int touchPin = 4;
const int ledPin = 5;

void setup() {

  Serial.begin(115200);

  pinMode(touchPin, INPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println(touchRead(touchPin));

  if(touchRead(touchPin) < 10)
    digitalWrite(ledPin, HIGH);
  else
    digitalWrite(ledPin, LOW);

  delay(50);
}
