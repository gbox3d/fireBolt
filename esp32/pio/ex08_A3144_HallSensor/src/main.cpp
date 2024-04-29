#include <Arduino.h> // Arduino 헤더 파일 포함
#include <WiFi.h>
#include <AsyncUDP.h>
#include <TaskScheduler.h>

// #include <ArduinoJson.h>

#include <tonkey.hpp>

#include "config.hpp"

#ifdef SEED_XIAO_ESP32C3
#define LED_PIN D0
#define ANALOG_PIN A0
#elif defined(LOLIN_D32)
#define LED_PIN 4
#elif defined(BEETLE_ESP32C3)
#define LED_PIN 10
#define ANALOG_PIN A0
#define BTN_PIN 7
#elif defined(WROVER_KIT)
#define LED_PIN 5
#define ANALOG_PIN 34
#endif


struct S_Packet
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

  uint16_t dev_id;
  uint16_t fire_count;

  // quaternion
  float qw;
  float qx;
  float qy;
  float qz;
};

static S_Packet packet;

Scheduler g_runner;
tonkey g_MainParser;
CCongifData configData;
AsyncUDP udp;

String target_ip;
uint16_t target_port;

// 인터럽트 서비스 루틴
volatile int cnt = 0;
void IRAM_ATTR detectHallSensor() {
  // udp.connect(IPAddress(192, 168, 1, 100), 12345); // IP와 포트 연결
  // udp.print("Detection!"); // 메시지 전송
  // int sensorState = digitalRead(7); // 7번 핀의 상태를 읽음
  // Serial.println(sensorState); // 상태 출력
  cnt++;
}


Task task_Cmd(
    100, TASK_FOREVER, []()
    {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        String _result = "OK";

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
            // DynamicJsonDocument _res_doc(1024);

            if (cmd == "help")
            {
              _result = "example a3144 sensor \n";
              _result += "save, load, clear, print \n";
              _result += "OK";
            }
            else if(cmd == "save") {
              configData.save();
            }
            else if ((cmd == "load")) 
            {
              configData.load();
            }
            else if ((cmd == "clear")) 
            {
              configData.clear();
              
            }
            else if ((cmd == "print")) 
            {
              configData.print();
            }
            else if (cmd == "set") {
              String key = g_MainParser.getToken(1);
              String value = g_MainParser.getToken(2);
              if(key == "ap") {
                configData.mStrAp = value;
              }
              else if(key == "password") {
                configData.mStrPassword = value;
              }
              else if(key == "target_ip") {
                configData.mTargetIp = value;
              }
              else if(key == "target_port") {
                configData.mTargetPort = value.toInt();
              }
              else {
                _result = "FAILD";
              }
            }
            else if( cmd == "reboot") {
              ESP.restart();
            }
            else if(cmd == "wifi") {
              String value = g_MainParser.getToken(1);
              if(value == "connect") {

                digitalWrite(LED_PIN, LOW);
                
                WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
                  Serial.printf("[WiFi-event] event: %d\n", event);

                  switch(event) {
                    case SYSTEM_EVENT_STA_GOT_IP:
                      Serial.println("WiFi connected");
                      digitalWrite(LED_PIN, HIGH);
                      break;
                    case SYSTEM_EVENT_STA_DISCONNECTED:
                      Serial.println("WiFi lost connection");
                      digitalWrite(LED_PIN, LOW);
                      break;
                    default: break;
                  }
                  
                });

                WiFi.mode(WIFI_STA);
                WiFi.begin(configData.mStrAp.c_str(), configData.mStrPassword.c_str());

                
              }
              else if(value == "disconnect") {
                WiFi.disconnect();
                // _res_doc["result"] = "ok";
              }
              else {
                // _res_doc["result"] = "fail";
                // _res_doc["ms"] = "unknown value";
                _result = "FAIL";
              }
            }
            else {
              _result = "FAIL";
            }
            // serializeJson(_res_doc, Serial);
            Serial.println(_result);
            Serial.println();
        }
    } });

// UDP 메시지 전송을 위한 작업
int prev_cnt = 0;
Task task_HallSensor(
    500, TASK_FOREVER, []() {
        IPAddress serverIP;
        serverIP.fromString(target_ip); // IP 주소로 변환
        // String message = "cnt: " + String(cnt);
        // udp.writeTo((uint8_t*)message.c_str(), message.length(), serverIP, target_port); // 메시지 전송
        packet.checkCode = 20230903;
        packet.cmd = 0x10;
        packet.dev_id = (uint16_t)0;

        packet.fire_count = (uint16_t)cnt;

        udp.writeTo((uint8_t *)&packet, sizeof(S_Packet), serverIP, target_port);

        if(prev_cnt != cnt) {
          Serial.printf("cnt: %d\n", cnt);
          prev_cnt = cnt;
        }
        // Serial.printf("cnt: %d\n", cnt);
        // cnt++;  // 카운터 증가
    });

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // pinMode(BTN_PIN, INPUT);
  pinMode(ANALOG_PIN, INPUT);

  pinMode(BTN_PIN, INPUT_PULLUP); // A3144 홀 센서를 입력으로 설정
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), detectHallSensor, FALLING); // 인터럽트 설정


  Serial.begin(115200); // 시리얼 통신 초기화

  Serial.println(":-]");
  Serial.println("Serial connected");

  configData.load();

  if(configData.mStrAp.length() == 0) {
    Serial.println("config data is empty");
    Serial.println("please set config data");
  }
  else {
    Serial.println("config data is loaded");
    Serial.printf("try to connect %s\n", configData.mStrAp.c_str());

    target_ip = configData.mTargetIp;
    target_port = configData.mTargetPort;
  }

  delay(500);

  // wifi
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info)
  {
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event)
    {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      digitalWrite(LED_PIN, HIGH);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      digitalWrite(LED_PIN, LOW);
      break;
    default:
      break;
    }
  });

  WiFi.mode(WIFI_STA);
  WiFi.begin(configData.mStrAp.c_str(), configData.mStrPassword.c_str());

  // g_ts.startNow();
  g_runner.addTask(task_Cmd);
  g_runner.addTask(task_HallSensor);

  task_Cmd.enable();
  task_HallSensor.enable();
}

void loop()
{
  g_runner.execute();
}
