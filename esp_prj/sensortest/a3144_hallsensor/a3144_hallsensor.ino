#include <Arduino.h>

const int ledPin = 5; // D1
const int sensorPin = 14; // D5
volatile int sensorState = LOW;
volatile unsigned long interruptTime = 0;
volatile bool on_state = false;

ICACHE_RAM_ATTR void handleInterrupt()
{
  sensorState = digitalRead(sensorPin);
  if (sensorState == LOW && on_state == false)
  {
      on_state = true;
      interruptTime = millis();    
  }  
}

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, CHANGE);

  digitalWrite(ledPin, HIGH); // turn the LED off by making the voltage LOW
  Serial.begin(115200);

  while (!Serial); // wait for serial attach
  delay(500);

  digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)

  Serial.println(":-]");
  Serial.println("Serial connected");
}

void loop()
{
  if (on_state && millis() - interruptTime <= 500)
  {
    digitalWrite(ledPin, HIGH); // turn the LED on
  }
  else
  {
    on_state = false;
    digitalWrite(ledPin, LOW); // turn the LED off
  }
}
