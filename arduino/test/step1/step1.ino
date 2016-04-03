
/*
 Stepper Motor Control - one step at a time

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor will step one step at a time, very slowly.  You can use this to
 test that you've got the four wires of your stepper wired to the correct
 pins. If wired correctly, all steps should be in the same direction.

 Use this also to count the number of steps per revolution of your motor,
 if you don't know it.  Then plug that number into the oneRevolution
 example to see if you got it right.

 Created 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

int in1Pin = 8;
int in2Pin = 9;
int in3Pin = 10;
int in4Pin = 11;

Stepper motor(64,in1Pin,in2Pin,in3Pin,in4Pin);

void setup()
{
  pinMode(in1Pin,OUTPUT);
  pinMode(in2Pin,OUTPUT);
  pinMode(in3Pin,OUTPUT);
  pinMode(in4Pin,OUTPUT);
  
  Serial.begin(9600);
  motor.setSpeed(255);
  
}

void loop() 
{
  if(Serial.available()) 
  {
    int steps = Serial.parseInt();
    Serial.println(steps);
    motor.step(steps);
  }

}




