#include <ArduinoJson.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    // wait serial port initialization
  }
  
  //{"pi":3.14,"name":"gbox"}


}

String strJson;
void loop() {
  
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0) {
    strJson = Serial.readStringUntil('\n');
    Serial.println(strJson);
    StaticJsonBuffer<200> jsonBuffer; //반드시 지역변수로 해야함 
    JsonObject& root = jsonBuffer.parseObject(strJson);
    if(root.success() == true) {
      Serial.println("parse ok");
      Serial.println(String("pi :") + (double)root["pi"]);
    
    }
    else {
      Serial.println("parse error");
    }
    
  }

  

}
