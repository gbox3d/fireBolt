#include <Arduino.h>
#include <TaskScheduler.h>
#include <tonkey.hpp>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <WiFi.h>
#include <vector>

#include <TaskScheduler.h>

#include "config.hpp"
#include "etc.hpp"

Config g_config;

Scheduler g_ts;

std::vector<int> ledPins;

void ledOffCallback();
void ledOnCallback();
Task tLedBlinker(500, TASK_FOREVER, &ledOnCallback, &g_ts, false);

int8_t systemMode = 0;

#ifdef ESP32

#if not defined(LED_BUILTIN) // check if the default LED pin is defined
#define LED_BUILTIN 5        // define the default LED pin if it isn't defined
#endif

#endif

void ledOnCallback()
{
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  tLedBlinker.setCallback(&ledOffCallback);
}

void ledOffCallback()
{
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
  tLedBlinker.setCallback(&ledOnCallback);
  // Serial.println("led blink");
  // Serial.println(LED_BUILTIN);
}

extern String parseCmd(String _strLine);

Task task_Cmd(100, TASK_FOREVER, []()
              {

#ifdef MEGA2560

    if(Serial1.available() > 0) {
        String _strLine = Serial1.readStringUntil('\n');
        _strLine.trim();
        Serial1.println(_strLine);
    }
#endif

    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(parseCmd(_strLine));

    } }, &g_ts, true);

void startBlink()
{
  tLedBlinker.enable();
}

void stopBlink()
{
  tLedBlinker.disable();
}

void ledOn(int pinIndex)
{
  if (pinIndex == -1)
  {
    for (int i = 0; i < ledPins.size(); i++)
    {
      int pin = ledPins[i];
      digitalWrite(pin, HIGH); // turn the LED on (HIGH is the voltage level)
    }
  }
  else
  {
    int pin = ledPins[pinIndex];
    digitalWrite(pin, HIGH); // turn the LED on (HIGH is the voltage level)
  }
}

void ledOff(int pinIndex)
{
  if (pinIndex == -1)
  {
    for (int i = 0; i < ledPins.size(); i++)
    {
      int pin = ledPins[i];
      digitalWrite(pin, LOW); // turn the LED on (HIGH is the voltage level)
    }
  }
  else
  {
    int pin = ledPins[pinIndex];
    digitalWrite(pin, LOW); // turn the LED on (HIGH is the voltage level)
  }
}

//------------------------------------------------ ble start
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// UUID for service and characteristic
#define SERVICE_UUID "02cb14bf-42e0-41bd-9d6d-68db02057e42"
#define CHARACTERISTIC_UUID "fef6efb5-fbf8-4d0a-a69f-57049733a87a"

#define CHECK_CODE 230815

struct S_Ble_Header_Req_Packet_V1
{
  uint32_t checkCode;
  uint8_t cmd;
  uint8_t parm[3];
};

struct S_Ble_Header_Res_Packet_V1
{
  uint32_t checkCode;
  uint8_t cmd;
  uint8_t parm[3];
};

class MyCharateristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    Serial.println("Characteristic write event");

    if (systemMode == 0)
    {
      // ascii mode
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
            Serial.println("Received BLE command:");
            Serial.println(value.c_str());

            String response = parseCmd(String(value.c_str()));
            Serial.println("Response:");
            Serial.println(response);

            // printMtuSize(pServer);

            // BLE를 통해 응답 전송
            pCharacteristic->setValue(response.c_str());
            pCharacteristic->notify();
        }
    }
    else
    {
      //binary mode

      std::string value = pCharacteristic->getValue();

      if (value.length() >= sizeof(S_Ble_Header_Req_Packet_V1))
      {
        S_Ble_Header_Req_Packet_V1 *packet = (S_Ble_Header_Req_Packet_V1 *)value.data();

        if (packet->checkCode == CHECK_CODE)
        {
          switch (packet->cmd)
          {
          case 0x01:
          {
            S_Ble_Header_Res_Packet_V1 resPacket;
            resPacket.checkCode = CHECK_CODE;
            resPacket.cmd = 0x01;
            resPacket.parm[0] = packet->parm[0];

            Serial.println("Echoing first parameter:");
            Serial.println(packet->parm[0], HEX);

            pCharacteristic->setValue((uint8_t *)&resPacket, sizeof(resPacket));
          }

          break;
          case 0x02:
          {
            int nPinIndex = packet->parm[0];

            digitalWrite(ledPins[nPinIndex], HIGH);
          }

            // for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
            // {
            //     digitalWrite(ledPins[i], HIGH);
            // }
            Serial.printf("LEDs ON: %d\n", packet->parm[0]);
            break;
          case 0x03:
          {
            int nPinIndex = packet->parm[0];

            digitalWrite(ledPins[nPinIndex], LOW);
          }
            Serial.printf("LEDs OFF: %d\n", packet->parm[0]);

            break;
          case 0x04:
          {
            S_Ble_Header_Res_Packet_V1 resPacket;
            resPacket.checkCode = CHECK_CODE;
            resPacket.cmd = 0x04;
            resPacket.parm[0] = 1;
            resPacket.parm[1] = 2;
            resPacket.parm[2] = 3;
            // notify the client of the new value
            pCharacteristic->setValue((uint8_t *)&resPacket, sizeof(resPacket));
            // delay(500);
            pCharacteristic->notify();
            break;
          }
          default:
            Serial.println("Unknown command");
          }
        }
        else
        {
          Serial.println("Invalid check code");
        }
      }
      else
      {
        Serial.println("Received value too short");
      }
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    // task_Blink.disable();

    stopBlink();

    digitalWrite(ledPins[0], LOW);
    deviceConnected = true;

    Serial.println("Client connected");
    pServer->getAdvertising()->stop(); // 클라이언트가 연결되면 광고 중지
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;

    // task_Blink.enable();
    startBlink();

    Serial.println("Client disconnected");
    pServer->getAdvertising()->start(); // 클라이언트가 연결 해제되면 광고 다시 시작
  }
};

//------------------------------------------------ ble end

void setup()
{

  String strDeviceName = "ESP32_BLE" + String(getChipID().c_str());

  /////////////////////////////////////////////////////////
  // ble setup ---------------------------------------------
  //  Create the BLE Device
  BLEDevice::init(strDeviceName.c_str());

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic->addDescriptor(new BLE2902()); // 알람 표시 기능활성화
  pCharacteristic->setCallbacks(new MyCharateristicCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.printf("Waiting a client connection to notify...%s \n", getChipID().c_str());

  //------------------------------------------------ble end
  /////////////////////////////////////////////////////////

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

  Serial.begin(115200);

  g_config.load();

  delay(500);

  systemMode = g_config.get<int>("mode", 0);

  Serial.println("system mode : " + String(systemMode));

  JsonDocument _doc_ledpins;
  g_config.getArray("ledpin", _doc_ledpins);

  JsonArray ledpin = _doc_ledpins.as<JsonArray>();

  for (JsonVariant v : ledpin)
  {

    int pin = v.as<int>();

    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH); // turn the LED off by making the voltage LOW
    ledPins.push_back(pin);

    Serial.print("led pin : " + String(pin) + "\n");
  }
  Serial.println(":-]");
  Serial.println("Serial connected");
  Serial.println("LED_BUILTIN: " + String(LED_BUILTIN));
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());

  // // print device name and info
  Serial.println("Ble Ready , Device name: " + strDeviceName);

  startBlink();

#ifdef ESP8266
  Serial.println("ESP8266");
#endif

  g_ts.startNow();
}

void loop()
{
  // runner.execute();
  g_ts.execute();
}
