#ifndef GUNPOWER_TARGET_SLAVE
#define GUNPOWER_TARGET_SLAVE

class CGunpowerTargetSlave {
  private:
  
  byte mUUID[16];
  int mTargetIndex;
  int mServo_pin;
  int mSensorDown_pin;
  int mSensorUp_pin;
  int mLED_pin;
  
  Servo mServo;
  //servo control
  int mAngle_base; //
  int mAngle_stand;
  int mAngle_down;
  
  int m_nFSM;
  int m_nStep;
  
  enum {
    FSM_READY = 0,
    FSM_STAND_UP,
    FSM_DOWN,
    FSM_HIT
  };
  

  public:
  
  
    CGunpowerTargetSlave(int servo_pin=9,int sensor_down=6,int sensor_up=7,int led_pin = 12,
    int angle_base = 130,
    int angle_stand = 30 ,
    int angle_down = 160
    ) {
      
      //mID = 0;
      mTargetIndex = 0;
      
      mServo_pin = servo_pin;
      mSensorDown_pin = sensor_down;
      mSensorUp_pin = sensor_up;
      mLED_pin = led_pin;
      
      mAngle_base = angle_base;
      mAngle_stand = angle_stand;
      mAngle_down = angle_down;
      
      pinMode(mSensorDown_pin,INPUT);
      pinMode(mLED_pin,OUTPUT);
      
    }
    ~CGunpowerTargetSlave() {
      
      mServo.detach();
    }
    
    
    void changeStatus(int newFsm)
    {
      m_nStep = 0;
      m_nFSM = newFsm;
    }
    
    void startUp() {
      
      delay(100);
      mServo.attach(mServo_pin);
      delay(100);
      mServo.write(mAngle_down);
      delay(300);
      mServo.write(mAngle_base);
      delay(500);
      mServo.detach();
      
    }
    
    
    void standUp() {
      
      mServo.attach(mServo_pin);
      delay(10);
      mServo.write(mAngle_stand);
      delay(500);
      mServo.write(mAngle_base);
      delay(300);
      mServo.detach();
      Serial.println("stand up test");
    }
    
    void fallDown() {
      
      mServo.attach(mServo_pin);
      delay(10);
      mServo.write(mAngle_down);
      delay(300);
      mServo.write(mAngle_base);
      Serial.println("fall down test");
      delay(300);
      mServo.detach();
    }
    
    void ledOn() {
      digitalWrite(mLED_pin,HIGH);
      Serial.println("led on");
    }
    
    void ledOff() {
      digitalWrite(mLED_pin,LOW);
      Serial.println("led on");
    }
    
    void update() {
      
      int sensor_status = digitalRead(mSensorDown_pin);
  
  //Serial.println(sensor_status);
  
      switch(m_nFSM) {
        case FSM_READY:
          Serial.println("ready status!");
          
          if(sensor_status)
            changeStatus(FSM_STAND_UP);
          else 
            changeStatus(FSM_DOWN);
          break;
          
        case FSM_STAND_UP:
          if(m_nStep) {
            m_nStep++;
            Serial.println("stand up status");
          }
          else {
            if(!sensor_status) {
              changeStatus(FSM_HIT);
              Serial.println("down!");
            }
          }
          break;
        case FSM_DOWN:
          if(m_nStep) {
            m_nStep++;
            Serial.println("down status");
          }
          else {
            if(sensor_status) {
              changeStatus(FSM_STAND_UP);
              Serial.println("up!");
            }
          }
          break;
        case FSM_HIT:
          if(m_nStep) {
            m_nStep++;
            Serial.println("hit !");
          }
          else {
            changeStatus(FSM_DOWN);
          }
          break;
      }
    }


    //////////////////
    
    
};

#endif


