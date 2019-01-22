

#include "Arduino.h"
#include "rm_object.h"


void CRMObject::UpdateSensor()
{
    long randNumber = random(0,10);
    //Serial.println(randNumber);

    char buf[12];
    mTemper[0] = 20.4 + randNumber;

    long temp = long(mTemper[0]*10);

    itoa(temp, buf, 10);


    Serial.println(buf);



}

void CRMObject::OutputData()
{




    mStrData = "+246+231+23114522200002000000000030211130300252500008091011120809101112182626262619262626261630000012218413060700005";
    Serial.print(mStrData);
}

