
#ifndef TEST_h
#define TEST_h

// the #include statment and code go here...

#include "Arduino.h"

class CTest
{
public :
    CTest() {
        Serial.println("just CTest");

    }
    int sum(int a,int b);
};

#endif

