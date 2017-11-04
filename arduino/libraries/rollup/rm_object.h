
#ifndef RM_OBJECT_h
#define RM_OBJECT_h

// the #include statment and code go here...

#include "Arduino.h"

class CRMTime {
    int mHour;
    int mMin;
};

class CRMObject
{
    float mTemper[3];

    //열림시간
    CRMTime mOpenTime[2];
    CRMTime mCloseTime[2];

    byte mWarnHighTemper[2]; //고온경보
    byte mWarnTemperSensor[2]; //온도계 이상

    byte mStatus[2]; //모터동작 상태
    byte mSleep[2];
    byte mMode[2];

    byte mRainySensor;


    byte mTemperTime_Mode[2]; //온도/시간 제어 모드
    byte mAutoManual_Mode[2]; //자동/수동 모드

    float mTemperRange[2];

    byte mTemperMode[2];

    CRMTime mActiveTime[2];
    CRMTime mWaitTime[2];

    float mHighTemperLimit[2];

    byte mSensormode;

    byte mControlSolver[2]; //작동방식 0 : 스텝 , 1 : 퍼지

    CRMTime mTimer[2][5]; //타이머
    float mTemperTriger[2][5]; //온도 트리거

    byte mTemperStep[2];

    byte mWind;

    CRMTime mLockTime[2]; //작동 제한 시간

    int mStepNumber[2]; //스텝 횟수

    String mStrData;

public :
    CRMObject() {
        mTemper[0] = 26.3;
        mTemper[1] = 27.3;
        mTemper[2] = 28.3;

        randomSeed(analogRead(0));
        Serial.println("create CRMObject instance");

    }

    void UpdateSensor();
    void OutputData();
};

#endif