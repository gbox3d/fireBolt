void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  digitalWrite(13,0);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  Serial.println(analogRead(A0));
  delay(50);

}
