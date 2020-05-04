#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <SimpleTimer.h>

SoftwareSerial mySerial(2, 3); // RX,TX



void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600);  
  delay(500);
  mySerial.println("start~");
}



void loop() {
  // put your main code here, to run repeatedly:
  while(mySerial.available() > 0) 
  {
    char c = mySerial.read();
    mySerial.println(c);
    
  }
}
