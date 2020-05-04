
const int pinTriger = 3;
const int pinStatus = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinTriger, INPUT);
  pinMode(pinStatus, OUTPUT);
  //digitalWrite(pinStatus, 0);
  //delay(500)
  digitalWrite(pinStatus, 1);
  delay(500);
  digitalWrite(pinStatus, 0);


}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(pinTriger) == LOW) 
  {
    digitalWrite(pinStatus, 1);
    
  }
  else {
    digitalWrite(pinStatus, 0);
  }

}
