void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13,OUTPUT);

}

void loop() {
  
  if(Serial.available()) {
    
    byte code = Serial.read();
    
    if(code == 97) {
      
      digitalWrite(13,HIGH);
      Serial.println("OK port13 on");
      
    }
    else if(code == 98) {
      digitalWrite(13,LOW);
      Serial.println("OK port13 off");
    }
    
  }
  
  // put your main code here, to run repeatedly:

}
