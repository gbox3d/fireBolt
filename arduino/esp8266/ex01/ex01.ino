/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

#include <ESP8266WiFi.h>

int pin = 2; //D4
const int output_pin = 16; //D0
const int output_pin2 = 14; //D5

void setup() {
  // initialize GPIO 2 as an output.
  pinMode(pin, OUTPUT);
  pinMode(output_pin, OUTPUT);
  pinMode(output_pin2, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(output_pin, HIGH);
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);               // wait for a second
  digitalWrite(output_pin, LOW);
  digitalWrite(pin, LOW);    // turn the LED off by making the voltage LOW
  delay(500);               // wait for a second

  digitalWrite(output_pin2, HIGH);
  delay(500);               // wait for a second
  digitalWrite(output_pin2, LOW);
  delay(500);               // wait for a second
}
