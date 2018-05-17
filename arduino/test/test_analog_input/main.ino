
#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  digitalWrite(13,0);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  int nValue = analogRead(A0);
  
  if(nValue > 10) {
    Serial.println(nValue);
  }

  delay(5);

}
