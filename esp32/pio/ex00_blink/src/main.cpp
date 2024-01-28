#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  pinMode(D10, OUTPUT);
  pinMode(D9, OUTPUT);
  digitalWrite(D10, LOW);
  digitalWrite(D9, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D10, LOW);
  digitalWrite(D9, LOW);
  delay(1000);
  digitalWrite(D10, HIGH);
  digitalWrite(D9, HIGH);
  delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}