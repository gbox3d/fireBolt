

#include "Arduino.h"
#include <ArduinoJson.h>
#include "egcscore.hpp"
#include <EEPROM.h>

void CEGCSCore::saveConfigData()
{
    //Serial.println("\"r\":\"save\",\"p1\":\"start\"}");
    char buf[128];
    StaticJsonBuffer<128> jsonBuffer;
    //StaticJsonBuffer<128> jsonBuffer;
    JsonObject &_config = jsonBuffer.createObject();
    _config["pu"] = m_pulseUpMill; //펄스업
    // _config["pumc"] = m_pulseUpMicro;
    _config["pd"] = m_pulseDownMill; //펄스다운
    // _config["pdmc"] = m_pulseDownMicro;
    _config["rc"] = m_nRecoilPluse;

    _config["mxct"] = m_nMaxFireCount;     //장전발수
    _config["cppc"] = m_nCoinPerPlayCount; //장전발수

    _config["cth"] = m_nCutOffThresHold;     //컷오프 대기시간
    _config["fpw"] = m_nPwmFirePowerControl; //발사 전압조절계수

    _config["msa"] = m_nMaxSemiAuto; //최대 연사발수

    _config["flag"] = m_FlagValue;
    _config["tc"] = m_nToTalCounter;
    //_config["msa"] = g_nMaxSemiAuto;

    _config.printTo(buf);

    for (int i = 0; i < 128; i++)
    {
        EEPROM.write(i, buf[i]);
        //delay(50);
        if (buf[i] == '}')
            break;
    }
    delay(100);
    Serial.println(buf);
}

void CEGCSCore::systemBootUp()
{

    char buf[128];
    for (int i = 0; i < 128; i++)
    {
        buf[i] = EEPROM.read(i);
        if (buf[i] == '}')
        {
            buf[++i] = 0x00;
            Serial.println(buf);
            break;
        }
    }

    StaticJsonBuffer<128> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(buf);
    if (root.success())
    {
        //Serial.println("parse success");
        m_pulseUpMill = root["pu"];
        m_pulseUpMicro = m_pulseUpMill * 1000;
        // m_pulseUpMicro = root["pumc"];
        m_pulseDownMill = root["pd"];
        m_pulseDownMicro = m_pulseDownMill * 1000;
        // m_pulseDownMicro = root["pdmc"];
        m_nRecoilPluse = root["rc"];

        m_nCutOffThresHold = root["cth"];

        m_nMaxFireCount = root["mxct"];

        m_nCoinPerPlayCount = root["cppc"];
        m_nPwmFirePowerControl = root["fpw"];
        m_nMaxSemiAuto = root["msa"];

        m_nToTalCounter = root["tc"];

        //플래그 값 읽기
        m_FlagValue = root["flag"];

        //마이크로 초 값 재설정
        updateMicroValue();

        //자동시작 여부

        if (m_FlagValue & 0x01)
        {
            m_bAutoStart = true;
        }
        else
        {
            m_bAutoStart = false;
        }

        //스타트버튼 선택
        if (m_FlagValue & 0x02)
        {
            m_bSwapStartBtn = true;
        }
        else
        {
            m_bSwapStartBtn = false;
        }

        if (m_FlagValue & 0x04)
        {
            m_bMokpoSystem = true;
        }
        else
        {
            m_bMokpoSystem = false;
        }

        if (m_FlagValue & 0x08)
        {
            m_bSuDalSystem = true;
        }
        else
        {
            m_bSuDalSystem = false;
        }
    }
    else
    {
        //Serial.println("parse failed");
        saveConfigData();
    }
}

void CEGCSCore::SerialOutputResponse(String szOperation, int p1, int p2)
{
    Serial.print("{\"r\":");
    Serial.print(szOperation);
    Serial.print(",\"p1\":");
    Serial.print(p1);
    Serial.print(",\"p2\":");
    Serial.print(p2);
    Serial.println("}");
}

void CEGCSCore::dumpConfig()
{
    StaticJsonBuffer<128> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["r"] = "cr";
    JsonArray &data = root.createNestedArray("data");
    data.add(m_pulseUpMill);
    // data.add(m_pulseUpMicro);
    data.add(m_pulseDownMill);
    // data.add(m_pulseDownMicro);
    data.add(m_nRecoilPluse);

    data.add(m_nMaxFireCount);
    data.add(m_nCoinPerPlayCount);
    data.add(m_nCutOffThresHold);
    data.add(m_nPwmFirePowerControl);
    data.add(m_nMaxSemiAuto);
    data.add(m_nToTalCounter);
    data.add(m_FlagValue);

    root.printTo(Serial);
    Serial.println();
}

void CEGCSCore::clearConfigData()
{
    for (int i = 0; i < EEPROM.length(); i++)
    {
        EEPROM.write(i, 0);
    }
    Serial.println("{\"r\":\"ok\"}");
}

void CEGCSCore::cmdConfig(String p1, int p2, int p3)
{
    if (p1 == "mxup")
    {
        //g_nMaxFireCount++;

        addMaxFireCount(1);
        Serial.println(String("{\"r\":\"dp\",\"p1\":") + m_nMaxFireCount + String("}"));
        //dumpConfig();
    }
    else if (p1 == "mxdn")
    {
        //g_nMaxFireCount--;

        addMaxFireCount(-1);
        Serial.println(String("{\"r\":\"dp\",\"p1\":") + m_nMaxFireCount + String("}"));
        //g_egcsCore.dumpConfig();
    }
    else if (p1 == "stmx")
    {
        //g_nMaxFireCount = root["p2"];

        //addMaxFireCount(1);
        m_nMaxFireCount = p2;
        Serial.println(String("{\"r\":\"dp\",\"p1\":") + m_nMaxFireCount + String("}"));
        //dumpConfig();
    }
    else if (p1 == "pu")
    {
        m_pulseUpMill = p2;
        // m_pulseUpMicro = p3;
        //SerialOutputResponse(p1, pulseUpMill, pulseUpMicro);
        /*Serial.println( String("{\"r\":\"dp\"") + 
          String(",\"p1\":") + pulseUpMill + 
          String(",\"p2\":") + pulseUpMicro + 
          String("}") );*/
    }
    else if (p1 == "pd")
    {
        m_pulseDownMill = p2;
        // m_pulseDownMicro = p3;
        //SerialOutputResponse(p1, pulseDownMill, pulseDownMicro);
        /*Serial.println( String("{\"r\":\"dp\"") + 
          String(",\"p1\":") + pulseDownMill + 
          String(",\"p2\":") + pulseDownMicro + 
          String("}") );*/
    }

    else if (p1 == "cppc")
    { //coin per play count
        //{"c":"cs","p1":"cppc","p2":2}
        //{"c":"cs","p1":"cppc","p2":0} 프리 플레이
        m_nCoinPerPlayCount = p2;
        //SerialOutputResponse(p1, g_nCoinPerPlayCount, 0);
        /*Serial.println("{\"r\":\"ok\",");
        Serial.println("{\"p1\":");
        Serial.println(g_nCoinPerPlayCount);
        Serial.println("}");*/
    }
    else if (p1 == "cth")
    { //컷오프 대기시산
        //{c:"cs",p1:"cth",p2:100}
        m_nCutOffThresHold = p2;
        SerialOutputResponse(p1, m_nCutOffThresHold, 0);
        //Serial.println("{\"r\":\"ok\"}");
    }
    else if (p1 == "fpw")
    { //발사 전압 계수
        //{c:"cs",p1:"fpw",p2:128}
        m_nPwmFirePowerControl = p2;
        //SerialOutputResponse(p1, g_nPwmFirePowerControl, 0);
        //Serial.println("{\"r\":\"ok\"}");
    }
    else if (p1 == "msa")
    { //발사 전압 계수
        //{c:"cs",p1:"msa",p2:3}
        m_nMaxSemiAuto = p2;
        //SerialOutputResponse(p1, g_nMaxSemiAuto, 0);

        /*Serial.print("{\"r\":\"ok\",");
        Serial.print("{\"p1\":");
        Serial.print(g_nMaxSemiAuto);
        Serial.println("}");*/
    }
    else if (p1 == "fv") //플레그값
    {
        m_FlagValue = p2;
    }
    else if (p1 == "rc")
    {
        m_nRecoilPluse = p2;
    }
    else
    {
        Serial.print("{\"r\":\"uk\",");
        Serial.print("{\"p1\":\"");
        Serial.print(p1);
        Serial.println("\"}");
    }

    updateMicroValue();

    dumpConfig();
}

void CEGCSCore::processCoinIn()
{
    m_coinCount++;
    SerialOutputResponse("coin", m_coinCount, 0);

    if (m_coinCount >= m_nCoinPerPlayCount)
    {
        m_coinCount = 0;
        m_nPlayCount++;
        SerialOutputResponse("playCount", m_nPlayCount, 0);
        /*Serial.print("{\"r\":\"playCount\"");
      Serial.print(",\"p1\":"); 
      Serial.print(g_nPlayCount);
      Serial.println("}"); */
        //return "readyToPlay";
        //szOperation = "ready";
    }
}