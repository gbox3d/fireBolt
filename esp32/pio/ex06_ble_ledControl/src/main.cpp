#include <Arduino.h>
#include <TaskScheduler.h>
#include <tonkey.hpp>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// #include "config.hpp"

#if defined(LOLIN_D32)
// const int builtInLed = LED_BUILTIN;
// #define LED_BUILTIN 5
const int ledPins[] = {4, 5};
#elif defined(SEED_XIAO_ESP32C3)
// #define LED_BUILTIN -1
// const int builtInLed = D5; //실재론 없다
const int ledPins[] = {D10, D9};
const int analogPins[] = {D0, D1};
const int buttonPins[] = {D8, D2, D3};
#elif defined(LOLIN_D32_LITE)
// const int builtInLed = 22;
const int ledPins[] = {
    22, 27, 26, 25, 23, 19, 18, 13, 14};
const int analogPins[] = {34, 35, 36, 39};
const int buttonPins[] = {32, 33};
#endif

String strTitleMsg = "FireBolt ESP32 Exam Ble\n";

String strHelpMsg = "\
command list\n\
help : show this message\n\
reboot : reboot esp32\n\
";

// CCongifData g_Config;

tonkey g_MainParser;
Scheduler runner;

Task task_Blink(
    250, TASK_FOREVER, []()
    {
      static bool _state = false;
      _state = !_state;
      digitalWrite(ledPins[0], _state); });

Task task_Cmd(
    100, TASK_FOREVER, []()
    {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        // Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
          String _result = "";

          if(cmd=="help") {
            _result = strTitleMsg + "\n";
            _result += strHelpMsg;
          }
          else if(cmd=="reboot") {
            Serial.println("rebooting...");
            delay(1000);
            ESP.restart();
          }
          else if(cmd=="stop_blink") {
            task_Blink.disable();
            digitalWrite(ledPins[0], LOW);
          }
          else if(cmd=="start_blink") {
            task_Blink.enable();
          }
          else {
            _result = "unknown command";
          }

          Serial.println(_result + "\nOK\n");
        }
    } });

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
};

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    task_Blink.disable();
    digitalWrite(ledPins[0], LOW);
    deviceConnected = true;

    Serial.println("Client connected");
    pServer->getAdvertising()->stop(); // 클라이언트가 연결되면 광고 중지
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;

    task_Blink.enable();

    Serial.println("Client disconnected");
    pServer->getAdvertising()->start(); // 클라이언트가 연결 해제되면 광고 다시 시작
  }
};

//------------------------------------------------ ble end

void setup()
{

  Serial.begin(115200);
  delay(500);

  String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
  chipId.toUpperCase(); // 대문자로 변환

  Serial.printf("now start , chipId:%s\n", chipId.c_str());

  // io setup
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP); // LOW: button pressed
  }

  for (int i = 0; i < sizeof(analogPins) / sizeof(analogPins[0]); i++)
  {
    pinMode(analogPins[i], INPUT);
  }

  /////////////////////////////////////////////////////////
  // ble setup ---------------------------------------------
  //  Create the BLE Device
  BLEDevice::init("ESP32_BLE" + std::string(getChipID().c_str()));
  // BLEDevice::init("MyESP32");

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

  // task setup
  runner.init();
  runner.addTask(task_Cmd);
  runner.addTask(task_Blink);

  task_Cmd.enable();
  task_Blink.enable();

  delay(1000);
  Serial.println(strTitleMsg);
}

void loop()
{
  runner.execute();
}
