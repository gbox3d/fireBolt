void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  //float temper = (float)random(1000,2000) / 100.0;
  float temper = (float)random(1000,7000) / 1000.0;
  float huminidty = (float)random(5000,8000) / 100.0;
  
  Serial.print( temper );
  Serial.print(",");
  Serial.println(huminidty);
  delay(500);

}
