void setup() {
  // put your setup code here, to run once:

  pinMode(13,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);

  digitalWrite(13,0);
  digitalWrite(11,0);
  digitalWrite(10,0);

  

}

void loop() {
  // put your main code here, to run repeatedly:

  delay(1500);
  digitalWrite(13,HIGH);
  digitalWrite(11,HIGH);
  digitalWrite(10,LOW);
  delay(1500);
  digitalWrite(13,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,HIGH);
  delay(1500);
  digitalWrite(13,HIGH);
  digitalWrite(11,HIGH);
  digitalWrite(10,HIGH);
  delay(1500);
  digitalWrite(13,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);

}
