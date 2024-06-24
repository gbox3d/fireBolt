#include <Arduino.h>

#include <MPU9250.h> // hideakitai/MPU9250

namespace hideaki
{
    MPU9250 mpu;
    void init()
    {

        Wire.begin();
        delay(2000);

        if (!mpu.setup(0x68))
        { // change to your own address
            while (1)
            {
                Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
                delay(5000);
            }
        }
        else
        {
            Serial.println("MPU9250 connected");
        }
    }
    
    void update()
    {
        mpu.update();
    }

    void print() {
        Serial.print("Yaw, Pitch, Roll: ");
        Serial.print(mpu.getYaw(), 2);
        Serial.print(", ");
        Serial.print(mpu.getPitch(), 2);
        Serial.print(", ");
        Serial.println(mpu.getRoll(), 2);
        
    }
}