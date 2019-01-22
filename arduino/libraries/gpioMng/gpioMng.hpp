
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
    unsigned long m_nOnDelay;
    unsigned long m_nOffDelay;
    int m_nRepeat; //반복횟수 -1 이면 무한반복 
    int m_nRepeatCounter;

    unsigned long m_nPluseTerm;    
    byte m_pBuf[8];
    byte m_nBufSize;

public :
    CGpioMng() 
    {
        m_accTick = 0;
    }
    CGpioMng(int port) 
    {
        /*pinMode(port,OUTPUT);
        m_nPort = port;
        m_accTick = 0;*/
        setup(port);
    }

    inline void setup(int port)
    {
        pinMode(port,OUTPUT);
        m_nPort = port;
        m_accTick = 0;
    }

    inline void startBlink(int nRepeat,unsigned long nOnDelay,unsigned long nOffDelay) 
    {
        m_nRepeat = nRepeat;
        m_nOnDelay = nOnDelay;
        m_nOffDelay = nOffDelay;
        m_nFSM = 10;
    }

//low -> high
    inline void makePluseUp(unsigned long nTerm) 
    {
        //m_nRepeat = nRepeat;
        //m_nOnDelay = nOnDelay;
        //m_nOffDelay = nOffDelay;
        m_nPluseTerm = nTerm;
        m_nFSM = 20;
    }

//high -> low
    inline void makePluseDown(unsigned long nTerm) 
    {
        //m_nRepeat = nRepeat;
        //m_nOnDelay = nOnDelay;
        //m_nOffDelay = nOffDelay;
        m_nPluseTerm = nTerm;
        m_nFSM = 30;
    }

    inline void makePluse(byte *pBuf,int count,unsigned long nTerm)
    {
        m_nFSM = 40;
        m_nStep = 0;
        memcpy(m_pBuf,pBuf,count);
        m_nPluseTerm = nTerm;
        m_nBufSize = count;
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

                // Serial.println(m_nStep);
                // Serial.println(m_accTick);
                // Serial.println(m_nOnDelay);
                
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
            //low -> high
            case 20:
            {
                switch(m_nStep) {
                    case 0:
                    {
                        digitalWrite(m_nPort,LOW);
                        m_nStep++;
                        m_accTick = 0;
                        m_nRepeatCounter=0;
                    }
                    break;
                    case 1:
                    {
                        if(m_accTick > m_nPluseTerm) {
                            
                            m_accTick = 0;
                            digitalWrite(m_nPort,HIGH);
                            m_nFSM = 0;
                            m_nStep = 0;
                        }
                    }
                    break;
                    
                }

            }
            break;

            //high -> low
            case 30:
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
                        if(m_accTick > m_nPluseTerm) {
                            
                            m_accTick = 0;
                            digitalWrite(m_nPort,LOW);
                            m_nFSM = 0;
                            m_nStep = 0;
                        }
                    }
                    break;
                    
                }

            }
            break;

            case 40:
            {
                static byte nIndex = 0;
                switch(m_nStep) {
                    case 0:
                    {
                        nIndex = 0;
                        m_nStep = 10;
                        m_accTick = 0;
                        digitalWrite(m_nPort,m_pBuf[nIndex]);                    
                    }
                    break;
                    case 10:
                    {
                        //Serial.println(m_accTick);
                        if(m_accTick > m_nPluseTerm) {
                            m_accTick = 0;
                            nIndex++;
                            if(nIndex < m_nBufSize) {
                                digitalWrite(m_nPort,m_pBuf[nIndex]);
                            }
                            else {
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