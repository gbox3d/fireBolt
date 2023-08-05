#include <WiFi.h>
#include <BluetoothSerial.h>
#include <BTAddress.h>
#include <BTAdvertisedDevice.h>
#include <BTScan.h>

#include <TaskScheduler.h>

#include <esp_chip_info.h>

BluetoothSerial SerialBT;

const int batteryPin = 34; // ADC 핀 번호
// const float voltageDivider = 3.0; // 전압 분배비율

#define LED_BUILTIN 5 // GPIO 핀 번호에 따라 변경할 수 있습니다.
Scheduler g_ts_blinker;
Task task_Blink(300, TASK_FOREVER, []() {
    if (!SerialBT.connected()) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }    
}, &g_ts_blinker, true);

String getChipID()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String macStr = "";
    for (int i = 0; i < 6; i++)
    {
        if (mac[i] < 0x10)
            macStr += "0"; // 한 자리 숫자 앞에 0 추가
        macStr += String(mac[i], HEX);
    }
    macStr.toUpperCase();
    return macStr;
}



void setup()
{
    pinMode(LED_BUILTIN, OUTPUT); // 내장 LED를 출력 모드로 설정
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(115200);

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    String deviceName = "ESP32_" + getChipID();
    Serial.printf("ESP32 Chip model = %d\n", chip_info.model);
    Serial.printf("ESP32 Chip cores = %d\n", chip_info.cores);
    Serial.printf("ESP32 Chip revision = %d\n", chip_info.revision);

    SerialBT.setPin("3117"); // Bluetooth PIN code
    SerialBT.begin(deviceName.c_str());

    //   SerialBT.setPin("1234"); // Bluetooth PIN code
    //   SerialBT.begin("ESP32_mytest"); // Bluetooth device name

    Serial.println("Bluetooth device is ready to pair");

    g_ts_blinker.startNow();
}

int gFSM = 0;
void loop()
{
    switch (gFSM)
    {
    case 0:
    {
        if (SerialBT.connected())
        { // 연결된 기기가 있으면
            Serial.println("Bluetooth Device Connected");
            SerialBT.println("Welcome to ESP32");
            
            digitalWrite(LED_BUILTIN, HIGH); // 내장 LED를 켭니다.
            
            gFSM = 1;
        }
    }
    break;
    case 1:
    {
        if (SerialBT.available())
        {                                  // 연결된 기기로부터 데이터가 들어오면
            // Serial.write(SerialBT.read()); // 시리얼 모니터에 출력
            char c = SerialBT.read();

            if (c == 'a')
            {
                int sensorValue = analogRead(batteryPin);
                float referenceVoltage = 4.2; // 측정된 참조 전압
                float voltage = sensorValue * (referenceVoltage / 4095.0); // 전압 계산
                // 배터리 잔량 계산
                float percentage = (voltage - 3.3) / (4.2 - 3.3) * 100.0;

                // SerialBT.print("sensor raw value: ");
                // SerialBT.println(sensorValue);
                SerialBT.printf("sensor raw value: %d\n", sensorValue);
                SerialBT.printf("sensor voltage: %.2f\n", voltage);
                SerialBT.printf("battery percent: %.2f\n", percentage);
                
            }
            
        }
        if (Serial.available())
        {                                  // 시리얼 모니터로부터 데이터가 들어오면
            SerialBT.write(Serial.read()); // 연결된 기기로 출력
        }
        if (!SerialBT.connected())
        { // 연결된 기기가 없으면
            Serial.println("Bluetooth Device Disconnected");
            digitalWrite(LED_BUILTIN, LOW); // 블루투스 연결 끊기면 LED 끄기
            gFSM = 0;
        }
    }
    }

    g_ts_blinker.execute();

    // delay(20);
}
