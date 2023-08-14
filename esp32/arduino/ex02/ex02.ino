#include <WiFi.h>
#include <BluetoothSerial.h>
#include <BTAddress.h>
#include <BTAdvertisedDevice.h>
#include <BTScan.h>

BluetoothSerial SerialBT;

String getChipID() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String macStr = "";
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 0x10) macStr += "0"; // 한 자리 숫자 앞에 0 추가
    macStr += String(mac[i], HEX);
  }
  macStr.toUpperCase();
  return macStr;
}


void setup() {
  Serial.begin(115200);

  // Bluetooth 장치 이름으로 Chip ID 사용
  String deviceName = "ESP32_" + getChipID();
  SerialBT.setPin("3117"); // Bluetooth PIN code
  SerialBT.begin(deviceName.c_str());
  Serial.println("Bluetooth device is ready to pair with name: " + deviceName);  
  Serial.println("Bluetooth device is ready to pair");
}

int gFSM = 0;
void loop() {
    switch(gFSM) {
        case 0: 
        {
            if (SerialBT.connected()) { // 연결된 기기가 있으면
                Serial.println("Bluetooth Device Connected");
                delay(1000);
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
