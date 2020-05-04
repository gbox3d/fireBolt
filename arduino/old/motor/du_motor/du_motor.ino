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
  
  Serial.begin(9600);
  Serial.println("start Motor system v0.1.0");
  
  Serial1.begin(9600);
  
  
  
}


int index = 0;
char messageBuffer[12];

/*

!80[chenel(1)][speed(3)][dir(1)]

chenel : 0~1
speed : 0~255
dir : 0 ->stop ,1 forword, 2 backword

example
!8001281.
*/

void processMotorShiled() {
  
  char cmd[3];
  char ch[2];
  char velocity[4];
  char dir[2];
  
  
  strncpy(cmd, messageBuffer, 2);
  cmd[2] = '\0';
  strncpy(ch, messageBuffer + 2, 1);
  ch[2] = '\0';
  strncpy(velocity, messageBuffer + 3, 3);
  velocity[3] = '\0';
  strncpy(dir, messageBuffer + 6, 1);

  int cmdid = atoi(cmd);
  int nCh = atoi(ch);
  int nSpeed = atoi(velocity);
  int nDir = atoi(dir);
  
  Serial.println(cmdid);
  Serial.println(nCh);
  Serial.println(nSpeed);
  Serial.println(nDir);
  
  //motor_ch[nCh].Forward(Speed);
  
  switch(nDir) {
    case 0:
    motor_ch[nCh].Stop();
    break;
    case 1:
    motor_ch[nCh].Forward(nSpeed);
    Serial.println("run forward");
    break;
    case 2:
    motor_ch[nCh].Back(nSpeed);
     Serial.println("run backward");
    break;
  }
  
}

void loop() {
  
  while(Serial1.available() > 0) {
    char x = Serial1.read();
    if (x == '!') index = 0;      // start
    else if (x == '.') {
      processMotorShiled(); // end
    }
    else messageBuffer[index++] = x;
  }
  
  /*
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
  */
  
}


