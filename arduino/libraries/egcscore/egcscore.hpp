
#ifndef EGCSCORE_HPP
#define EGCSCORE_HPP

// the #include statment and code go here...

#include "Arduino.h"

class CEGCSCore
{

    int m_nFireCount;
    int m_nMaxFireCount;
    
    int m_nPwmFirePowerControl;

    int m_coinCount;
    int m_nCoinPerPlayCount;
    int m_nPlayCount;

    int m_nSemiAutoCounter; //반자동 연사횟수 계산변수
    int m_nMaxSemiAuto;     //반자동 연사횟수

    unsigned long m_nCutOffThresHold;

    byte m_FlagValue;
    
  public:
    bool m_bAutoStart = false;
    bool m_bMokpoSystem = false;

    int m_nToTalCounter;
    
    int m_pulseUpMill;    
    int m_pulseDownMill;
        
    int m_nRecoilPluse;

    CEGCSCore()
    {
        m_nFireCount = 0;
        m_nMaxFireCount = 25;
        m_nToTalCounter = 0;

        m_pulseUpMill = 120;
        //m_pulseUpMicro = 0;

        m_pulseDownMill = 50;
        // m_pulseDownMicro = 0;

        m_nRecoilPluse = 0;

        m_nPwmFirePowerControl = 255;

        m_coinCount = 0;
        m_nCoinPerPlayCount = 0;
        m_nPlayCount = 0;

        m_nSemiAutoCounter = 0; //반자동 연사횟수 계산변수
        m_nMaxSemiAuto = 1;     //반자동 연사횟수

        m_nCutOffThresHold = 15;

        m_FlagValue = 0;
        m_bAutoStart = false;
    }
    //---rom
    void saveConfigData();
    void clearConfigData();
    void systemBootUp();
    void processCoinIn();
    void cmdConfig(String p1, int p2, int p3);

    //utils
    void SerialOutputResponse(String szOperation, int p1, int p2);
    void dumpConfig();

    //-------- parser
    //String parseCmd(const char *_szBuf);
    inline void addCoinCount(int nAdd)
    {

        m_coinCount += nAdd;

        if (m_coinCount >= m_nCoinPerPlayCount)
        {
            m_coinCount = 0;
            m_nPlayCount++;
            SerialOutputResponse("playCount", m_nPlayCount, 0);
        }
        Serial.print("{\"r\":\"coin\",\"p1\":\"");
        Serial.print(m_coinCount);
        Serial.println("\"}");
    }
    inline void addMaxFireCount(int nAdd)
    {
        m_nMaxFireCount += nAdd;
        //Serial.println( String("{\"r\":\"dp\",\"p1\":") + m_nMaxFireCount + String("}") );
    }
    inline void setMaxFireCount(int nVal)
    {
        m_nMaxFireCount = nVal;
        //Serial.println( String("{\"r\":\"dp\",\"p1\":") + m_nMaxFireCount + String("}") );
    }

    
    inline int getCoinPerPlayCount()
    {
        return m_nCoinPerPlayCount;
    }
    inline int setCoinPerPlayCount(int nVal)
    {
        m_nCoinPerPlayCount = nVal;
    }

    inline int getCurrentPlayCount()
    {
        return m_nPlayCount;
    }
    inline int setCurrentPlayCount(int nVal)
    {
        m_nPlayCount = nVal;
    }

    inline void addCurrentPlayCount(int nVal)
    {
        m_nPlayCount += nVal;
        if(m_nPlayCount < 0) {
            m_nPlayCount = 0;
        }
    }

    inline bool isAutoStart()
    {
        return m_bAutoStart;
    }

    inline bool isMokpoSystem()
    {
        return m_bMokpoSystem;
    }

    inline void initPlayGun()
    {
        m_nFireCount = m_nMaxFireCount;                            
        m_nSemiAutoCounter = m_nMaxSemiAuto;
    }
    inline void restSemiAuto()
    {
        //m_nFireCount = m_nMaxFireCount;                            
        m_nSemiAutoCounter = m_nMaxSemiAuto;
    }

    inline int getFireCount()
    {
        return m_nFireCount;
    }

    inline void addFireCount(int nVal)
    {
        m_nFireCount += nVal;
    }

    inline int getSemiAutoCounter()
    {
        return m_nSemiAutoCounter;
    }
    inline void addSemiAutoCounter(int nVal)
    {
        m_nSemiAutoCounter += nVal;
    }

    inline void doFirePinHighPluse(int nPin)
    {
        analogWrite(nPin,m_nPwmFirePowerControl); //다시 켜기 
    }
/*
    inline int getPluseUpValue()
    {
        return m_pulseUpMill;
    }
    inline int getPluseDownValue()
    {
        return m_pulseDownMill;
    }
    */
    inline int getCutOffThresHold()
    {
        return m_nCutOffThresHold;
    }

};

#endif
