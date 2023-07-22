#include <Arduino.h>
const int ledPin = 5; // D1
const int sensorPin = 14; // D5

volatile boolean enableTriger = true;

ICACHE_RAM_ATTR void detect_sensor()
{
    if (enableTriger)
    {
        enableTriger = false;
    }
}

void setup()
{  
  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  //인터럽트 셋업
  attachInterrupt(digitalPinToInterrupt(sensorPin), detect_sensor, FALLING);

  digitalWrite(ledPin, HIGH); // turn the LED off by making the voltage LOW
  Serial.begin(115200);

  while (!Serial); // wait for serial attach
  delay(500);

  digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)

  Serial.println(":-]");
  Serial.println("Serial connected");  

}

int count = 0;
void loop()
{
    int sensorState = digitalRead(sensorPin);

    if (!enableTriger) //발사 신호 감지 
    {

      digitalWrite(ledPin, HIGH); // turn the LED off by making the voltage LOW
      
        
      delay(150);
      count++;

        Serial.println("detect sensor : " + String(count));
      digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)
      enableTriger = true;

    }
}