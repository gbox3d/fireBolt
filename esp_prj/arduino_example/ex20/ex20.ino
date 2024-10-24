// 디지털 센서입력예쩨 on off

#include <Arduino.h>

const int sensor_pin = 14; // D5;
const int relay_pin = 5; //D1;

volatile boolean enableTriger = true;
int nFSM = 0;

ICACHE_RAM_ATTR void detect_sensor()
{
    if (enableTriger)
    {
        enableTriger = false;
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(sensor_pin, INPUT);
    // 인터럽트 셋업
    attachInterrupt(digitalPinToInterrupt(sensor_pin), detect_sensor, FALLING);

    pinMode(relay_pin, OUTPUT);
    digitalWrite(relay_pin, HIGH);

    Serial.printf("\n****\nsetup done\n");
}

int nCount = 0;
void loop()
{
    int sensor_value = digitalRead(sensor_pin);

    switch (nFSM)
    {
    case 0:
        if(sensor_value == LOW)
        {
            nFSM = 1;
            delay(100);
        }
        break;
    case 1:
        if(sensor_value == HIGH)
        {
            nFSM = 0;
            enableTriger = true;
            Serial.printf("sensor is off %d\n", nCount++);
        }
        break;
    
    default:
        break;
    }

    // if (sensor_value == LOW)
    // {
    //     Serial.println("sensor is on");
    // }

    delay(20);
}