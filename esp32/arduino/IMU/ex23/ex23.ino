#include <Wire.h>

// WHO_AM_I 레지스터의 기본 값
#define MPU6050_WHOAMI_DEFAULT_VALUE 0x68
#define MPU6500_WHOAMI_DEFAULT_VALUE 0x70
#define MPU9250_WHOAMI_DEFAULT_VALUE 0x71
#define MPU9255_WHOAMI_DEFAULT_VALUE 0x73
#define AK8963_WHOAMI_DEFAULT_VALUE 0x48

uint8_t addrs[7] = {0};
uint8_t device_count = 0;

void scan_mpu()
{
    Serial.println("Searching for i2c devices...");
    device_count = 0;
    for (uint8_t i = 0x68; i < 0x70; ++i)
    {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0)
        {
            addrs[device_count++] = i;
            delay(10);
        }
    }
    Serial.print("Found ");
    Serial.print(device_count, DEC);
    Serial.println(" I2C devices");

    Serial.print("I2C addresses are: ");
    for (uint8_t i = 0; i < device_count; ++i)
    {
        Serial.print("0x");
        Serial.print(addrs[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

uint8_t readByte(uint8_t address, uint8_t subAddress)
{
    uint8_t data = 0;
    Wire.beginTransmission(address);
    Wire.write(subAddress);
    Wire.endTransmission(false);
    Wire.requestFrom(address, (size_t)1);
    if (Wire.available())
        data = Wire.read();
    return data;
}

void setup()
{
    Serial.begin(115200);
    Serial.flush();
    Wire.begin();
    delay(2000);

    scan_mpu();

    if (device_count == 0)
    {
        Serial.println("No device found on I2C bus. Please check your hardware connection");
        while (1)
            ;
    }

    for (uint8_t i = 0; i < device_count; ++i)
    {
        Serial.print("I2C address 0x");
        Serial.print(addrs[i], HEX);
        byte ca = readByte(addrs[i], 0x75); // WHO_AM_I register address
        if (ca == MPU6050_WHOAMI_DEFAULT_VALUE)
        {
            Serial.println(" is MPU6050 and ready to use");
        }
        else if (ca == MPU6500_WHOAMI_DEFAULT_VALUE)
        {
            Serial.println(" is MPU6500 and ready to use");
        }
        else if (ca == MPU9250_WHOAMI_DEFAULT_VALUE)
        {
            Serial.println(" is MPU9250 and ready to use");
        }
        else if (ca == MPU9255_WHOAMI_DEFAULT_VALUE)
        {
            Serial.println(" is MPU9255 and ready to use");
        }
        else if (ca == AK8963_WHOAMI_DEFAULT_VALUE)
        {
            Serial.println(" is AK8963 and ready to use");
        }
        else
        {
            Serial.println(" is not a recognized MPU series");
            Serial.print("WHO_AM_I is ");
            Serial.println(ca, HEX);
            Serial.println("Please use a correct device");
        }
    }
}

void loop()
{
    // Your code here
}
