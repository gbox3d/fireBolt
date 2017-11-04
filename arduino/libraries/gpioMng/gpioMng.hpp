
#ifndef GPIOMNG_HPP
#define GPIOMNG_HPP

// the #include statment and code go here...

#include "Arduino.h"

class CGpioMng
{
    int m_nPort;
    unsigned long m_accTick;
    int m_nFSM;
    int m_nStep;
    int m_nOnDelay;
    int m_nOffDelay;
    int m_nRepeat; //반복횟수 -1 이면 무한반복 
    int m_nRepeatCounter;

public :
    CGpioMng(int port) 
    {

        pinMode(port,OUTPUT);
        m_nPort = port;
        m_accTick = 0;

        
    }

    inline void startBlink(int nRepeat,int nOnDelay,int nOffDelay) 
    {
        m_nRepeat = nRepeat;
        m_nOnDelay = nOnDelay;
        m_nOffDelay = nOffDelay;
        m_nFSM = 10;
    }

    inline void stopBlink(int nPinStatus) 
    {
        digitalWrite(m_nPort,nPinStatus);
        m_nFSM = 0;
    }

    inline void doApply(unsigned long deltaTick)
    {
        m_accTick += deltaTick;
        /*
        Serial.print(m_accTick);
        Serial.print(",");
        Serial.print(m_nFSM);
        Serial.print(",");
        Serial.print(m_nStep);
        Serial.print(",");
        Serial.println(m_nPort);
        */
        switch(m_nFSM) {
            case 0: //stop 
            {
            }
            break;
            case 10:
            {
                
                switch(m_nStep) {
                    case 0:
                    {
                        digitalWrite(m_nPort,HIGH);
                        m_nStep++;
                        m_accTick = 0;
                        m_nRepeatCounter=0;
                    }
                    break;
                    case 1:
                    {
                        if(m_accTick > m_nOnDelay) {
                            m_nStep++;
                            m_accTick = 0;
                            digitalWrite(m_nPort,LOW);
                        }

                    }
                    break;
                    case 2:
                    {
                        if(m_accTick > m_nOffDelay) {
                            m_nRepeatCounter++;
                            m_accTick = 0;
                            if(m_nRepeat < 0) // -1 이면 무한반복
                            {
                                m_nStep = 0;
                            }
                            else if( m_nRepeatCounter >= m_nRepeat) {
                                m_nFSM = 0;
                                m_nStep = 0;
                            }
                        }
                    }
                    break;
                }

            }
            break;
        }

    }
    
};

#endif