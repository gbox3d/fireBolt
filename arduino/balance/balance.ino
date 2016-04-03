
int sensorPin_1 = A0;
int sensorPin_2 = A1;



void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200); 

}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue1 = analogRead(sensorPin_1);
  int sensorValue2 = analogRead(sensorPin_2);
  
  Serial.print(sensorValue1);
  Serial.print(",");
  Serial.print(sensorValue2);
  Serial.write(0x81);
  
  delay(50);
  
  //Serial.write(0x81);

}
