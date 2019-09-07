#include <Wire.h>

const int g_nVersion = 100;
const int g_DeviceID = 0x1b;

byte RegPort_Mode[2] = {0x33, 0xcc};
byte RegPort_Status[2] = {0x00, 0x00};


void receiveEvent(int howMany)
{
  int _index = 0;
  static int _opBuf[8];
  while (0 < Wire.available())
  {
    int c = Wire.read();
    _opBuf[_index++] = c;
    Serial.print(c);
    Serial.print(",");
  }
  Serial.println(howMany);

  switch(_opBuf[0]) {
    case 0x00: //nop
    Serial.println("nop");
    break;
    case 0x01:
    Serial.println("pin mode");
    break;
    case 0x02:
    Serial.println("pin set");
    {
      int _bnk = _opBuf[1];
      int _val = _opBuf[2];
      if(howMany == 3) {
        RegPort_Status[_bnk] = _val;
      }
    }
    break;
    case 0xf1:
    {
      Serial.print(RegPort_Status[0]);
      Serial.print(" ");
      Serial.println(RegPort_Status[1]);
    }
    break;
  }

}

void requestEvent()
{
  Wire.write(RegPort_Status,2);
  // byte test[] = {60,61};
  // Wire.write(test,2);
}

//0011 1100 1100 1100 => 0x3c  0xcc


void printReg(byte reg)
{
  for (int i = 0; i < 8; i++)
  {
    if(reg & 0x80)
    {
      // pinMode(i,OUTPUT);
      Serial.print("1");
    }
    else {
      // pinMode(i,INPUT);
      Serial.print("0");
    }
    reg  <<= 1;
  }
  //Serial.println();
}

void pinModeReg(int bank)
{
  byte reg = RegPort_Mode[bank];
  for (int i = 0; i < 8; i++)
  {
    if(reg & 0x80)
    {
      pinMode(i + (bank*8),OUTPUT);
    }
    else {
      pinMode(i+(bank*8),INPUT_PULLUP);
    }
    reg  <<= 1;
  }
  //Serial.println();
}

void setup()
{
  Wire.begin(g_DeviceID);       // 자신의 주소를  설정하고 i2c 버스에 참여
  Wire.onReceive(receiveEvent); // 수신 이벤트 함수 등록
  Wire.onRequest(requestEvent);

  Serial.begin(115200);

  Serial.println(">regPM-0#");
  printReg(RegPort_Mode[0]);
  pinModeReg(0);

  Serial.println(">regPM-1#");
  printReg(RegPort_Mode[1]);
  pinModeReg(1);

  Serial.println(">regPS-0#");
  printReg(RegPort_Status[0]);
  Serial.println(">regPS-1#");
  printReg(RegPort_Status[1]);

  Serial.println(">ready#");
}

void loop()
{
  byte _mode_reg = RegPort_Mode[0];
  byte _status_reg = RegPort_Status[0];


//  Serial.println(_mode_reg);
  for (int i = 0; i < 8; i++)
  {
    if( (_mode_reg & 0x80)   )
    {
//      Serial.print(i);
      if((_status_reg & 0x80))
      {
        digitalWrite(i,HIGH);
        //Serial.print(i);
        //Serial.print(" ");
      }
      else {
        digitalWrite(i,LOW);
      }
        
    }
    else {
      if(digitalRead(i)) {
        RegPort_Status[0] |= (0x0080 >> i);
      }
      else {
        RegPort_Status[0] &= (0xff7f >> i);
      }

     // Serial.print(" in");
    }
    _mode_reg  <<= 1;
    _status_reg  <<= 1;
  }
//  Serial.println();
  
  //delay(1000);

  _mode_reg = RegPort_Mode[1];
  _status_reg = RegPort_Status[1];
  // Serial.print(_status_reg);
  for (int i = 0; i < 8; i++)
  {
    if( (_mode_reg & 0x80)   )
    { 
      if((_status_reg & 0x80))
      {
        digitalWrite(i+8,HIGH);
      }
      else 
        digitalWrite(i+8,LOW);
    }
    else {
      if(digitalRead(i+8)) {
        RegPort_Status[1] |= (0x0080 >> i);
      }
      else {
        RegPort_Status[1] &= (0xff7f >> i);
      }

     // Serial.print(" in");
    }
    
    _mode_reg  <<= 1;
    _status_reg  <<= 1;
    //Serial.println();
  }

  delay(5);
}
