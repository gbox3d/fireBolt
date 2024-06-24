#include <Arduino.h>

#include "I2Cdev.h"
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif



void setup()
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // initialize serial communication
  // (115200 chosen because it is required for Teapot Demo output, but it's
  // really up to you depending on your project)
  Serial.begin(115200);
  while (!Serial)
    ; // wait for Leonardo enumeration, others continue immediately

  Serial.println("MPU6050 DMP6 Example ... ready to go!");
}

//zero
void doZero();
int16_t *GetActiveOffset();

//dmp
void initDmp(int16_t *pOffset);

String _strLine;
void loop()
{
  if (Serial.available() > 0)
  {
    // 하나씩 읽어서 버퍼에 쌓는다.
    char _ch = Serial.read();
    if (_ch == '.')
    {
      _strLine.trim();
      // Serial.println(_strLine);
      if (_strLine == "help")
      {
        Serial.println("it is mp6050 dmp sample");
        Serial.println("command list");
        Serial.println("help : show this message");
        Serial.println("init : init mpu6050");
        Serial.println("start : start dmp");
        Serial.println("stop : stop dmp");
        Serial.println("reboot : reboot mpu6050");
      }
      else if (_strLine == "init")
      {
        int16_t offset[6]= {
          56,-27,50,-1614,1255,1666
        };

        initDmp(offset);
      }
      else if (_strLine == "start")
      {
        // initDmp();
      }
      else if (_strLine == "stop")
      {
        // dmpReady = false;
      }
      else if (_strLine == "reboot")
      {
        ESP.restart();
        // mpu.reset();
      }
      else if (_strLine == "zero")
      {
        doZero();
      }
      else if (_strLine == "offset")
      {
        int16_t *pOffset = GetActiveOffset();

        Serial.print("acc x:");
        Serial.println(pOffset[0]);
        Serial.print("acc y:");
        Serial.println(pOffset[1]);
        Serial.print("acc z:");
        Serial.println(pOffset[2]);

        Serial.print("gyro x:");
        Serial.println(pOffset[3]);
        Serial.print("gyro y:");
        Serial.println(pOffset[4]);
        Serial.print("gyro z:");
        Serial.println(pOffset[5]);
      }
      else
      {
        Serial.println("unknown command");
      }
      _strLine = "";
    }

    else
    {
      _strLine += _ch;
    }
  }
}
