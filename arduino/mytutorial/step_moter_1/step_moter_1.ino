#include <ArduinoJson.h>

/*
    BYJ48 Stepper motor code
    Connect :
    IN1 -> D8
    IN2 -> D9
    IN3 -> D10
    IN4 -> D11
    VCC . 5V Prefer to use external 5V Source
    Gnd
*/

 #define IN1  8
 #define IN2  9
 #define IN3  10
 #define IN4  11
 int Steps = 0;
 boolean Direction = true; //
 unsigned long last_time;
 unsigned long currentMillis,clk_delay ;
 int steps_left=4095;
 long time;
 void setup()
 {
     Serial.begin(9600);
     pinMode(IN1, OUTPUT); 
     pinMode(IN2, OUTPUT); 
     pinMode(IN3, OUTPUT); 
     pinMode(IN4, OUTPUT); 
     // delay(1000);
 }

 //{"step":4095,"dir":true,"clk_delay":1000}
 void loop()
 { 
    String strJson;
    if(Serial.available() > 0) {
      strJson = Serial.readStringUntil('\n');
      Serial.println(strJson);

      StaticJsonBuffer<200> jsonBuffer;
      
      JsonObject& root = jsonBuffer.parseObject(strJson);
      if(root.success() == true) {
        Serial.println("parse ok");
        //Serial.println(String("pi :") + (double)root["pi"]);


        steps_left = (long)root["step"];
        Direction = (boolean)root["dir"];
        clk_delay = (long)root["clk_delay"];
        while(steps_left>0)
        {
            currentMillis = micros();
            if(currentMillis-last_time>=clk_delay)
            {
                stepper(1); 
                time=time+micros()-last_time;
                last_time=micros();
                steps_left--;
            }
        } // while
        Serial.println("step all done!");
        
      
      }
      else {
        Serial.println("parse error");
      }
    
    }
   
 }

 void stepper(int xw) 
 {
    for (int x=0;x<xw;x++) 
    {
        switch(Steps)
        {
            case 0:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, HIGH);
                break; 
            case 1:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, HIGH);
                break; 
            case 2:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
                break; 
            case 3:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
                break; 
            case 4:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                break; 
            case 5:
                digitalWrite(IN1, HIGH); 
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                break; 
            case 6:
                digitalWrite(IN1, HIGH); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                break; 
            case 7:
                digitalWrite(IN1, HIGH); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, HIGH);
                break; 
        default:
                digitalWrite(IN1, LOW); 
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                break; 
        }
        
        SetDirection();
    }
 } 

 void SetDirection()
 {
    if(Direction==1) { Steps++; }
    if(Direction==0) { Steps--; }
    if(Steps>7) { Steps=0; }
    if(Steps<0) { Steps=7; }
 }

