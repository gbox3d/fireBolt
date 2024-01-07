#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);


  Serial.begin(115200);
  Serial.println("Hello World!");
  Serial.println(result);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World!");
  digitalWrite(D0, HIGH);
  delay(500);
  digitalWrite(D0, LOW);
  delay(500);

  digitalWrite(D1, HIGH);
  delay(500);
  digitalWrite(D1, LOW);
  delay(500);

  digitalWrite(D2, HIGH);
  delay(500);
  digitalWrite(D2, LOW);
  delay(500);

  digitalWrite(D3, HIGH);
  delay(500);
  digitalWrite(D3, LOW);
  delay(500);
  
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}