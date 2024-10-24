//
//    FILE: GY521_angle.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: read angleX, angleY, angleZ
//    DATE: 2022-06-06

#include <Arduino.h>
#include <Wire.h>
#include "GY521.h"

GY521 sensor(0x68);

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println(__FILE__);
    Serial.print("GY521_LIB_VERSION: ");
    Serial.println(GY521_LIB_VERSION);

    Wire.begin();

    delay(100);
    while (sensor.wakeup() == false)
    {
        Serial.print(millis());
        Serial.println("\tCould not connect to GY521");
        delay(1000);
    }
    sensor.setAccelSensitivity(2); //  8g
    sensor.setGyroSensitivity(1);  //  500 degrees/s

    sensor.setThrottle();
    Serial.println("start...");

    //  set calibration values from calibration sketch.
    sensor.axe = 0.574;
    sensor.aye = -0.002;
    sensor.aze = -1.043;
    sensor.gxe = 10.702;
    sensor.gye = -6.436;
    sensor.gze = -0.676;
}

void loop()
{
    sensor.read();

    float ax = sensor.getAccelX() * 9.8;
    float ay = sensor.getAccelY() * 9.8;
    float az = sensor.getAccelZ() * 9.8;

    Serial.printf("Accel: %6.2f %6.2f %6.2f\n", ax, ay, az);

    delay(100);
}

//  -- END OF FILE --
