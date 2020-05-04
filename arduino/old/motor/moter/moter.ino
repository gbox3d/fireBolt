void setup() {
  pinMode(12,OUTPUT);
  //digitalWrite(12,HIGH);
  digitalWrite(9,HIGH);
  
}

void loop() {
  
  digitalWrite(12,HIGH);
  digitalWrite(9,LOW);
  analogWrite(3,100);
  
  delay(1000);
  
  digitalWrite(9,HIGH);
  //analogWrite(3,0);
  
  //analogWrite(3,100);
  
  delay(1000);
  
  digitalWrite(12,LOW);
  digitalWrite(9,LOW);
  analogWrite(3,100);
  
}
