#include <Arduino.h>
#include <math.h>
#include <WiFi.h>
#include <Wire.h>
#include "AsyncUDP.h"

#include "MPU6050_tockn.h"

#include <MadgwickAHRS.h>

// MPU6050 mpu6050(Wire);
Madgwick filter;
unsigned long microsPerReading, microsPrevious;

MPU6050 mpu6050(Wire);

const char *ssid = "redstar0427";
const char *password = "123456789a";
const char *udpAddress = "192.168.4.232";
const int udpPort = 9250;

//gpio 5
const int ledPin = LED_BUILTIN;

AsyncUDP udp;

struct S_Udp_IMU_RawData_Packet
{
    uint32_t checkCode;
    uint8_t cmd;
    uint8_t parm[3];
    float aX;
    float aY;
    float aZ;
    float gX;
    float gY;
    float gZ;
    float mX;
    float mY;
    float mZ;
    float extra;
    float battery;

    float pitch;
    float roll;
    float yaw;
};

void setup()
{
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);

    Serial.begin(115200);
    Wire.begin();

    mpu6050.begin();
    // DO NOT MOVE MPU6050 during calculating
    mpu6050.calcGyroOffsets(true);

    filter.begin(100); // Madgwick filter update rate in Hz

    // initialize variables to pace updates to correct rate
    microsPerReading = 1000000 / 100;
    microsPrevious = micros();

    // wifi setup
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");

    }
    Serial.println("Connected to WiFi");

    //print connection status & info
    
    Serial.printf("Connected to %s\n", ssid);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    

    if (udp.listen(udpPort))
    {
        udp.onPacket([](AsyncUDPPacket packet)
                     {
                         // Handle incoming packets here
                     });
    }

    digitalWrite(ledPin, LOW);
}

S_Udp_IMU_RawData_Packet packet;

void loop()
{   
    unsigned long microsNow = micros();
    if (microsNow - microsPrevious >= microsPerReading)
    {
        packet.checkCode = 20230903;
        packet.cmd = 0x10;

        mpu6050.update();

        float ax = mpu6050.getAccX();
        float ay = mpu6050.getAccY();
        float az = mpu6050.getAccZ();

        float gx = mpu6050.getGyroX();
        float gy = mpu6050.getGyroY();
        float gz = mpu6050.getGyroZ();

        // Update Madgwick filter
        filter.updateIMU(gx, gy, gz, ax, ay, az);

        // Get Euler angles (roll, pitch, yaw)
        float roll = filter.getRoll();
        float pitch = filter.getPitch();
        float yaw = filter.getYaw();

        packet.pitch = pitch;
        packet.roll = roll;
        packet.yaw = yaw;

        // Print angles with Serial.printf, right-aligned and 6 digits (3 decimal places)
        // Serial.printf("Roll: %6.3f\tPitch: %6.3f\tYaw: %6.3f\n", roll, pitch, yaw);
        sendDataToServer(packet);

        // increment previous time, so we keep proper pace
        microsPrevious = microsPrevious + microsPerReading;
    }
}

void sendDataToServer(S_Udp_IMU_RawData_Packet &packet)
{
    IPAddress serverIP;
    serverIP.fromString(udpAddress); // Convert the C string to an IPAddress
    udp.writeTo((uint8_t *)&packet, sizeof(S_Udp_IMU_RawData_Packet), serverIP, udpPort);
}
