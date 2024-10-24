#include <Arduino.h>
#include <Wire.h>
#include <math.h>

// gy521  example

const int MPU6050_addr = 0x68;
float AcX, AcY, AcZ, GyX, GyY, GyZ;
float Roll, Pitch;

float gyroScaleFactor = 131; // MPU6050의 경우, 131로 나누면 도/초 단위로 변환됩니다.

void setup()
{
    Wire.begin();
    Wire.beginTransmission(MPU6050_addr);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
    Serial.begin(115200);
}

void loop()
{
    Wire.beginTransmission(MPU6050_addr);
    Wire.write(0x3B); // Starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU6050_addr, (size_t)14, true); // request a total of 14 registers

    //   Wire.requestFrom((uint8_t)MPU6050_addr, (size_t)6, (bool)true);

    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

    // 온도 값을 읽어옵니다.
    int16_t Temp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

    GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

    Roll = atan2(AcY, sqrt(AcX * AcX + AcZ * AcZ)) * 180 / M_PI;
    Pitch = atan2(AcX, sqrt(AcY * AcY + AcZ * AcZ)) * 180 / M_PI;

    // 자이로스코프 값을 도/초 단위로 변환
    float gyroRateX = GyX / gyroScaleFactor;
    float gyroRateY = GyY / gyroScaleFactor;
    float gyroRateZ = GyZ / gyroScaleFactor;

    Serial.printf("AcX = %6.2f | AcY = %6.2f | AcZ = %6.2f | GyX = %6.2f | GyY = %6.2f | GyZ = %6.2f | Temp = %6.2f \n",
                  AcX, AcY, AcZ, GyX, GyY, GyZ, Temp / 340.00 + 36.53);
                  
    // Serial.printf("pitch = %f, roll = %f, gyroRateX = %f, gyroRateY = %f, gyroRateZ = %f, temperature = %f \n",
    //               Pitch, Roll, gyroRateX, gyroRateY, gyroRateZ,
    //               Temp / 340.00 + 36.53);

    //   Serial.print("Roll: "); Serial.print(Roll);
    //   Serial.print(" | Pitch: "); Serial.print(Pitch);
    //   Serial.print(" | Gyro - X: "); Serial.print(GyX);
    //   Serial.print(" | Y: "); Serial.print(GyY);
    //   Serial.print(" | Z: "); Serial.print(GyZ);
    // Serial.print(" | temperature: ");
    // Serial.println(Temp / 340.00 + 36.53);

    delay(100);
}