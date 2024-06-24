#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

namespace asukiaaa
{
  uint8_t sensorId;
  MPU9250_asukiaaa mySensor;
  float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

  void init()
  {

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
    Wire.begin(SDA_PIN, SCL_PIN);
    mySensor.setWire(&Wire);
#endif

    mySensor.beginAccel();
    mySensor.beginGyro();
    mySensor.beginMag();

    int result = mySensor.readId(&sensorId);
    if (result == 0) {
      Serial.println("sensorId: " + String(sensorId));
    } else {
      Serial.println("Cannot read sensorId " + String(result));
    }
  }

  void update()
  { 
    int result = mySensor.accelUpdate();
    if (result == 0)
    {
      aX = mySensor.accelX();
      aY = mySensor.accelY();
      aZ = mySensor.accelZ();

      aSqrt = mySensor.accelSqrt();
    }
    else
    {
      aX = 0;
      aY = 0;
      aZ = 0;
      aSqrt = 0;
      // Serial.println("Cannod read accel values " + String(result));
    }

    result = mySensor.gyroUpdate();
    if (result == 0)
    {
      gX = mySensor.gyroX();
      gY = mySensor.gyroY();
      gZ = mySensor.gyroZ();
    }
    else
    {
      // Serial.println("Cannot read gyro values " + String(result));
      gX = 0;
      gY = 0;
      gZ = 0;
    }

    result = mySensor.magUpdate();
    if (result != 0)
    {
      Serial.println("cannot read mag so call begin again");
      mySensor.beginMag();
      result = mySensor.magUpdate();
    }
    if (result == 0)
    {
      mX = mySensor.magX();
      mY = mySensor.magY();
      mZ = mySensor.magZ();
      mDirection = mySensor.magHorizDirection();
    }
    else
    {
      // Serial.println("Cannot read mag values " + String(result));
      mX = 0;
      mY = 0;
      mZ = 0;
      mDirection = 0;
    }
  }

  void print() {
    Serial.printf("id %d,aX: %7.2f,aY: %7.2f,aZ: %7.2f,gX: %7.2f,gY: %7.2f,gZ: %7.2f,mX: %7.2f,mY: %7.2f,mZ: %7.2f,mDirection: %7.2f\n",
                  sensorId, aX, aY, aZ, gX, gY, gZ, mX, mY, mZ, mDirection);
  }


}
