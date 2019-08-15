/*
 arduino repl framework by json 
 */

#include <Wire.h>


//#define __DEBUG__

const int g_nVersion = 10000;
const int g_DeviceID = 0x30; 
const char *g_strOkResponse = "{\"r\":\"ok\"}";

//const int g_nMaxpinNumber = 14;
//0: en ,1: pinmode, 2: value
unsigned int g_pinEnStatus;
unsigned int g_pinModeStatus;
unsigned int g_pinDataStatus;

int g_nFlagValue = 0;

String outputDeviceInfo()
{
  /*Serial.print("{\"r\":\"version\",\"p1\":");
  Serial.print(g_nVersion);
  Serial.print(",\"did\":");
  Serial.print(g_DeviceID);
  Serial.println("}");*/

  return String("{\"r\":\"version\",\"p1\":") + g_nVersion +
    String(",\"did\":") + g_DeviceID +
    String("}");
}


//-------- parser
String parseCmd(const char *_szBuf)
{
  //Serial.println(szbufJson);
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(_szBuf);
  String szOperation;
  if (root.success())
  {
    const char *cmd = root["c"];
    if (cmd != NULL)
    {
      //example : {"c":"rqdi"}
      //{c:"dprom"} {c:"pm",p:[13,1]} {c:"dw",p:[13,1]}

      JsonArray &param = root["p"];
      
      if (!strcmp(cmd, "rqdi")) // device information
      {
        Serial.println(outputDeviceInfo());
        
      }      
      else if (!strcmp(cmd, "dw")) //digital write
      {
        digitalWrite((int)param[0],(int)param[1]);
        bitWrite(g_pinDataStatus,(int)param[0],(int)param[1]);
        Serial.println(g_strOkResponse);
      }   
      else if (!strcmp(cmd, "dr")) //digital write
      {
        Serial.println(  digitalRead(12) );
        
        //Serial.println(  (int) param[0] );
        Serial.println(String("{\"r\":\"dr\",\"p\":") + digitalRead((int)param[0]) +
          String("}") );
      }      
      else if (!strcmp(cmd, "pm")) //pin mode
      {
        pinMode((int)param[0],(int)param[1]);
        //g_pinStatus[(int)param[0]] = (int)param[1];
        bitWrite(g_pinEnStatus,(int)param[0],1);
        bitWrite(g_pinModeStatus,(int)param[0],(int)param[1]);
        Serial.println(g_strOkResponse);

        /*
        g_pinEnStatus |= (0x01 << (int)param[0] );
        if((int)param[1]) {
          g_pinModeStatus |= (0x01 << (int)param[0] );
        }
        */

      }
      else if( !strcmp(cmd,"clrom")) {
        // for (int i = 0; i < EEPROM.length(); i++)
        // {
        //     EEPROM.write(i, 0);
        // }
        EEPROM.write(0, '{');
        EEPROM.write(1, '}');     
        Serial.println(g_strOkResponse);   
      }
      else if( !strcmp(cmd,"svcfg")) {
        saveConfigData();
        Serial.println(g_strOkResponse);
      }
      else if(!strcmp(cmd,"dprom")) {

        for (int i = 0; i < 128; i++)
        {
            char _c = EEPROM.read(i);
            if (_c == '}' || _c == 0x00 )
            {
                Serial.println(_c);
                break;
            }
            else {
              Serial.print(_c);
            }
        }

      }
      else {
        //에러 출력 
        Serial.println(String("{\"r\":\"erc\",\"p\":") + cmd +
          String("}") );
      }
    }
  }
  else
  {
    Serial.println("{\"r\":\"erp\"}");
  }

  return szOperation;
}
//-------
// 다음 함수는 마스터가 데이터를 전송해 올 때마다 호출됨
bool bI2CBufReady = false;
char szbufJsonI2C[128];
void receiveEvent(int howMany)
{

  static int nBufIndex = 0;

  while (0 < Wire.available() && !bI2CBufReady)
  {
    char c = Wire.read();
    szbufJsonI2C[nBufIndex++] = c;
    //Serial.print(c);
    if (c == '}')
    {
      szbufJsonI2C[nBufIndex] = 0x00;
      //Serial.println(szbufJsonI2C);
      nBufIndex = 0;
      bI2CBufReady = true;
    }
  }
}

void requestEvent()
{
  Wire.write("{\"v\":100}");
}

//-----------------------------------------------

String checkI2CCmd()
{
  if (bI2CBufReady)
  {
    bI2CBufReady = false;
    return parseCmd(szbufJsonI2C);
  }

  return "";
}

String checkUartCmd()
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

//----------------------------------------

void saveConfigData()
{
  //Serial.println("parse failed");
        //saveConfigData();
  char _buf[128];
  StaticJsonBuffer<128> jsonBuffer;    
  JsonObject &_config = jsonBuffer.createObject();
  _config["flag"] = g_nFlagValue;
  _config["cs"] = "ok";
  _config["pe"] = g_pinEnStatus;
  _config["pm"] = g_pinModeStatus;
  _config["pd"] = g_pinDataStatus;
  
  _config.printTo(_buf);

  for (int i = 0; i < 128; i++)
  {
      EEPROM.write(i, _buf[i]);
      //delay(50);
      if (_buf[i] == '}')
          break;
  }
  delay(100);
  Serial.println(_buf);

}
void systemBootup()
{
  char buf[128];
  for (int i = 0; i < 128; i++)
  {
      buf[i] = EEPROM.read(i);
      if (buf[i] == '}')
      {
          buf[++i] = 0x00;
          //Serial.println(buf);
          break;
      }
  }

  StaticJsonBuffer<128> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(buf);
  if (root.success())
  {
    if( (char *)root["cs"] != NULL ) {

      //JsonArray &param = root["ps"];

      unsigned int _pinEnable = root["pe"];
      unsigned int _pinMode = root["pm"];
      unsigned int _pinData = root["pd"];


      for(int i=0; i<32;i++) {
        if ( bitRead(_pinEnable,i) ) {
          if( bitRead(_pinMode,i )) {
            pinMode(i,1);
            digitalWrite(i,bitRead(_pinData,i));
          } 
          else {
            pinMode(i,0);

          }
        }
      }


/*
      for(int i=0; i<32;i++) {
        unsigned int _pinBit = (0x1 << i);
        if( _pinEnable & _pinBit) {
          if(_pinMode & _pinBit) {
            pinMode(i,1);
            digitalWrite(i, _pinData & _pinBit);
          }
          else {
            pinMode(i,0);
          }
        }
      }
      */

      // for(int i=0;i<g_nMaxpinNumber;i++) {
      //   g_pinStatus[i] = (int)param[i];
      //   if(g_pinStatus[i] > 0) {
      //     pinMode(i,g_pinStatus[i]-1);
      //     Serial.println(
      //       String("{\"r\":\"pm\",\"p\":") + i + 
      //       String(",") + (g_pinStatus[i]-1) + 
      //       String("}")
      //       );
      //   }
      // }


    }
    else {
      Serial.println("{\"r\",\"er\":\"not found cs\"}");
      //Serial.println((char *)root["cs"]);
      saveConfigData();
    } 
  }
  else
  {
    saveConfigData();
  }
}

void setup()
{
    
  Wire.begin(g_DeviceID);       // 자신의 주소를 4번으로 설정하고 i2c 버스에 참여
  Wire.onReceive(receiveEvent); // 수신 이벤트 함수 등록
  Wire.onRequest(requestEvent); //요청이밴트

  Serial.begin(115200);

  Serial.println(outputDeviceInfo());

  systemBootup();

  //pinMode(12,0);
  
  //롬읽기  
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
  //Serial.println(_deltaTick);

  String szOperation = checkUartCmd();
  if (szOperation == "")
  {
    szOperation = checkI2CCmd();
  }

  //Serial.println(  digitalRead(12) );
  
}
