/*
 gpiomng test
 */

#include <ArduinoJson.h>
#include <gpioMng.hpp>

CGpioMng g_test_pin(13);

//-------- parser
int parseCmd(const char *_szBuf)
{
  //Serial.println(szbufJson);""
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(_szBuf);
  
  if (root.success())
  {
    const char *cmd = root["c"];
    if (cmd != NULL)
    {
      //example : {"c":"rqdi"}
      //{c:"dprom"} {c:"pm",p:[13,1]} {c:"dw",p:[13,1]}
      Serial.println(cmd);
      JsonArray &param = root["p"];      
      if ( !strcmp(cmd,"test1") ) // device information
      {
          static byte pBuf[] = {1,0,1,0};
          g_test_pin.makePluse(pBuf,4,500000);

      }   
    }
  }
  return 0;
}
//-------

int checkUartCmd()
{
  static char szbufJson[128];
  static int nBufIndex = 0;

  while (Serial.available() > 0)
  {
    char x = Serial.read();
    if (x == '}')
    {
      szbufJson[nBufIndex++] = x;
      szbufJson[nBufIndex] = 0x00;
      nBufIndex = 0;
      return parseCmd(szbufJson);
    }
    else
    {
      szbufJson[nBufIndex++] = x;
    }
  }

  return "";
}

void setup()
{
  Serial.begin(115200);  
  g_test_pin.stopBlink(0);
}

void loop()
{  
  static int nStep = 0;
  static int nFSM = 0;

  static unsigned long _deltaTick =0;
  static unsigned long _curTick = micros();
  static unsigned long _prevTick = micros();
//get deltatick

  _curTick = micros();
  if(_curTick > _prevTick) {
    _deltaTick = _curTick - _prevTick;
  }
  else {
    //_deltaTick = 0;
  }
  _prevTick = _curTick;

  checkUartCmd();
  g_test_pin.doApply(_deltaTick);
  
}
