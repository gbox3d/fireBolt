

class CArduino_MotorShild {
  public :
  byte mDirection_pin;
  byte mSpeed_pin;
  byte mBrake_pin;
  
  CArduino_MotorShild(byte dir_pin,byte speed_pin,byte brake_pin ) :
  mDirection_pin(dir_pin),
  mSpeed_pin(speed_pin),
  mBrake_pin(brake_pin)
  {
    pinMode(mDirection_pin,OUTPUT);
    pinMode(mBrake_pin,OUTPUT);
    
    Stop();
  }
  
  void Forward(byte speed)
  {
    digitalWrite(mDirection_pin,HIGH);
    digitalWrite(mBrake_pin,LOW);
    analogWrite(mSpeed_pin,speed);
  }
  
  void Back(byte speed)
  {
    digitalWrite(mDirection_pin,LOW);
    digitalWrite(mBrake_pin,LOW);
    analogWrite(mSpeed_pin,speed);
  }
  
  void Stop()
  {
    digitalWrite(mBrake_pin,HIGH);
    analogWrite(mSpeed_pin,0);
  }
  
  
};

CArduino_MotorShild motor_ch[2] = {
  CArduino_MotorShild(12,3,9),
  CArduino_MotorShild(13,11,8)
};

void setup() {
  
  //sparkfun : 115200
  
  //btshild
  Serial.begin(9600);
  
  Serial.println("start Motor system");
  
  
  
  
}

void loop() {
 
  if(Serial.available() >0) {
  
    int ch = Serial.parseInt();
    Serial.print("chennle : ");
    Serial.println(ch);
    int Speed = Serial.parseInt();
    int dir = Serial.parseInt();
    
    
    
    if(ch == 1 || ch == 2) {
      Serial.print(ch);
      Serial.print(':');
      
      ch -= 1;
      
      if(dir == 0) {
        motor_ch[ch].Stop();
        
        Serial.println(" stop");
        
      }
      else if(dir==1) {
        motor_ch[ch].Forward(Speed);
        Serial.println("run forward");
        
      }
      else if(dir==2) {
        motor_ch[ch].Back(Speed);
        Serial.println("run backward");
      }
    }
    else {
      Serial.println("chennel not exist");
    }
    
  }
  
}
