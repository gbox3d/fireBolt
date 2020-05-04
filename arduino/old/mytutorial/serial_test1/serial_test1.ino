
void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  
}

void loop()
{
  digitalWrite(13,HIGH);
  //Serial.println("hello");
  Serial.write(61);
  Serial.write(62);
  Serial.write(63);
  Serial.write(64);
  Serial.write(65);
  Serial.write(66);
  Serial.write(67);
  delay(500);
  digitalWrite(13,LOW);
  delay(500);
}
