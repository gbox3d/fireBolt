
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX,TX

void setup()  
{
 
  mySerial.begin(9600);
  Serial.begin(9600);
  Serial1.begin(9600);
  
}



void loop() // run over and over
{
  while(mySerial.available() > 0) {
    byte data = mySerial.read(); 
    Serial.write(data);
  }
  
  while(Serial1.available() > 0) {
    byte data = Serial1.read(); 
    Serial.write(data);
  }
  
  
  while(Serial.available() > 0) {
    byte data = Serial.read();
    mySerial.write(data);
    Serial1.write(data);
  }
  
}

