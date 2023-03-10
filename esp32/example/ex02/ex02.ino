#include <BluetoothSerial.h>
#include <BTAddress.h>
#include <BTAdvertisedDevice.h>
#include <BTScan.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  
  SerialBT.setPin("1234"); // Bluetooth PIN code
  SerialBT.begin("ESP32_mytest"); // Bluetooth device name
  
  Serial.println("Bluetooth device is ready to pair");
  
}

int gFSM = 0;
void loop() {
    switch(gFSM) {
        case 0: 
        {
            if (SerialBT.connected()) { // 연결된 기기가 있으면
                Serial.println("Bluetooth Device Connected");
                SerialBT.println("Welcome to ESP32");
                gFSM = 1;
            }
        }
        break;
        case 1: 
        {
            if (SerialBT.available()) { // 연결된 기기로부터 데이터가 들어오면
                Serial.write(SerialBT.read()); // 시리얼 모니터에 출력
            }
            if (Serial.available()) { // 시리얼 모니터로부터 데이터가 들어오면
                SerialBT.write(Serial.read()); // 연결된 기기로 출력
            }
            if (!SerialBT.connected()) { // 연결된 기기가 없으면
                Serial.println("Bluetooth Device Disconnected");
                gFSM = 0;
            }
        }
    }

  delay(20);
}
