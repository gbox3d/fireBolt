#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

MPU9250_asukiaaa mySensor;
float aX, aY, aZ;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("started");

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);
#endif

  mySensor.beginAccel();
}

void loop() {
  int result = mySensor.accelUpdate();
  if (result == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();

    float angleX = atan2(aY, sqrt(aX * aX + aZ * aZ)) * 180 / PI;
    float angleY = atan2(aX, sqrt(aY * aY + aZ * aZ)) * 180 / PI;

    Serial.println("Angle X: " + String(angleX) + ", Angle Y: " + String(angleY));
  } else {
    Serial.println("Cannot read accel values " + String(result));
  }
  delay(100); // Update rate 10Hz
}
