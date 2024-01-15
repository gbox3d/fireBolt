#include <Wire.h>
#include <MPU6050_tockn.h>
#include <MadgwickAHRS.h>

MPU6050 mpu6050(Wire);
Madgwick filter;
unsigned long microsPerReading, microsPrevious;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  filter.begin(25); // 25Hz sample rate

  // initialize variables to pace updates to correct rate
  microsPerReading = 1000000 / 25; //40000 microseconds/reading = 25Hz
  microsPrevious = micros();
}

void loop() {
  unsigned long microsNow = micros();

  //25Hz 로 센서값 읽기
  if (microsNow - microsPrevious >= microsPerReading) {
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

    // Print angles with Serial.printf, right-aligned and 6 digits (3 decimal places)
    Serial.printf("Roll: %6.3f\tPitch: %6.3f\tYaw: %6.3f\n", roll, pitch, yaw);

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
  }
}
