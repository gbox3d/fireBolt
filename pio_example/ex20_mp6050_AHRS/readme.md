# mp6050 imu sample

사용된 라이브러리  

MPU6050_tockn : https://github.com/tockn/MPU6050_tockn    
MadgwickAHRS : https://github.com/arduino-libraries/MadgwickAHRS  


```c
#include <MPU6050_tockn.h>
#include <MadgwickAHRS.h>

...

MPU6050 mpu6050(Wire);
Madgwick filter;

...

// 데이터 업데이트 
mpu6050.update();

// 가속도, 자이로 데이터 읽기
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

```
MPU6050_tockn 라이브러리로 가속도, 자이로 데이터를 읽어서 MadgwickAHRS 라이브러리로 오일러 각도를 구한다.  

## upload binary to esp32

```bash
pio run -t upload --upload-port /dev/cu.usbmodem21301 --upload-protocol esptool --upload-file .pio/build/seeed_xiao_esp32c3/firmware.bin
```
