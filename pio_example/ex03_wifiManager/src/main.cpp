#include <Arduino.h>
#include <TaskScheduler.h>
#include <WiFiManager.h> // WiFiManager 라이브러리를 포함
#include <tonkey.hpp>

#include "config.hpp"

#if defined(SEED_XIAO_ESP32C3)
#define LED1_PIN D0
#define LED2_PIN D1
#define RESET_PIN D3
#else
#define LED_PIN 4
#endif

Scheduler g_ts;
tonkey g_MainParser;
CCongifData g_Config;

Task task_Cmd(
    100, TASK_FOREVER, []()
    {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);

          if(cmd=="help") {
            Serial.println("it is wifiManager example revision 1.0.1");
          }
          else if(cmd=="reboot") {
            ESP.restart();
          }
          else if(cmd=="print") {
            g_Config.print();
          }
        }
    }
    },&g_ts, true);


// WiFi 이벤트 핸들러 함수
void WiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case SYSTEM_EVENT_STA_DISCONNECTED:
            digitalWrite(LED1_PIN, LOW);  // WiFi 연결 해제 시 LED 끄기
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
        case SYSTEM_EVENT_STA_GOT_IP:
            digitalWrite(LED1_PIN, HIGH); // WiFi 연결 시 LED 켜기
            break;
        default:
            break;
    }
}

void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("AP 모드로 전환됨. 설정을 입력해주세요.");
  // AP 모드로 전환 시 LED 점멸 시작
  digitalWrite(LED1_PIN, HIGH);
}

void setup()
{

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(RESET_PIN, INPUT_PULLUP);

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  WiFi.onEvent(WiFiEvent);    // WiFi 이벤트 핸들러 등록

  Serial.begin(115200); // 시리얼 통신을 시작

  while (!Serial)
    ; // wait for serial attach


  g_Config.load(); // load config

  WiFiManager wifiManager;

  if (digitalRead(RESET_PIN) == LOW)
  {
    // 사용자 정의 매개변수 (장치 번호를 위한)
    WiFiManagerParameter custom_device_number("device_number", "장치 번호", String(g_Config.mDeviceNumber).c_str(), 10);
    WiFiManagerParameter custom_target_ip("target_ip", "타겟 IP", g_Config.mTargetIp.c_str(), 16);
    WiFiManagerParameter custom_target_port("target_port", "타겟 포트", String(g_Config.mTargetPort).c_str(), 6);

    WiFiManagerParameter custom_offset0("offset0", "Gyro X offset", String(g_Config.mOffsets[0]).c_str(), 6);
    WiFiManagerParameter custom_offset1("offset1", "Gyro Y offset", String(g_Config.mOffsets[1]).c_str(), 6);
    WiFiManagerParameter custom_offset2("offset2", "Gyro Z offset", String(g_Config.mOffsets[2]).c_str(), 6);
    WiFiManagerParameter custom_offset3("offset3", "Accel X offset", String(g_Config.mOffsets[3]).c_str(), 6);
    WiFiManagerParameter custom_offset4("offset4", "Accel Y offset", String(g_Config.mOffsets[4]).c_str(), 6);
    WiFiManagerParameter custom_offset5("offset5", "Accel Z offset", String(g_Config.mOffsets[5]).c_str(), 6);


    // 'Reset Settings'라는 이름의 추가 메뉴 항목을 만들어서 WiFi 설정을 리셋할 수 있게 함
    // WiFiManager에 장치 번호 매개변수 추가
    wifiManager.addParameter(&custom_device_number);
    wifiManager.addParameter(&custom_target_ip);
    wifiManager.addParameter(&custom_target_port);

    wifiManager.addParameter(&custom_offset0);
    wifiManager.addParameter(&custom_offset1);
    wifiManager.addParameter(&custom_offset2);
    wifiManager.addParameter(&custom_offset3);
    wifiManager.addParameter(&custom_offset4);
    wifiManager.addParameter(&custom_offset5);


    // WiFiManager에 AP 모드 콜백 설정
    wifiManager.setAPCallback(configModeCallback);
    Serial.println("Resetting WiFi settings...");
    wifiManager.resetSettings();

    // ESP32 칩셋 ID를 얻어서 문자열로 변환
    String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
    chipId.toUpperCase(); // 대문자로 변환

    // AP 모드의 이름을 "BSQ-"와 칩셋 ID의 조합으로 설정
    String ssid = "BSQ-" + chipId;
    const char *ssid_ptr = ssid.c_str(); // const char* 타입으로 변환

    // 여기서 "AutoConnectAP"는 ESP32가 AP 모드로 전환했을 때 나타나는 네트워크의 이름입니다.
    // "password"는 AP 모드의 비밀번호입니다 (최소 8자 필요).
    // 이 부분은 필요에 따라 수정하면 됩니다.
    wifiManager.autoConnect(ssid_ptr, "123456789");

    g_Config.mDeviceNumber = String(custom_device_number.getValue()).toInt();
    g_Config.mTargetIp = custom_target_ip.getValue();
    g_Config.mTargetPort = String(custom_target_port.getValue()).toInt();

    g_Config.mOffsets[0] = String(custom_offset0.getValue()).toInt();
    g_Config.mOffsets[1] = String(custom_offset1.getValue()).toInt();
    g_Config.mOffsets[2] = String(custom_offset2.getValue()).toInt();
    g_Config.mOffsets[3] = String(custom_offset3.getValue()).toInt();
    g_Config.mOffsets[4] = String(custom_offset4.getValue()).toInt();
    g_Config.mOffsets[5] = String(custom_offset5.getValue()).toInt();

    g_Config.print();
    g_Config.save();

    // 사용자 입력 값 확인 및 정수로 변환
    // Serial.println("사용자가 입력한 장치 번호: ");
    // String deviceNumberStr = custom_device_number.getValue();
    // long deviceNumber = deviceNumberStr.toInt(); // 문자열을 long 타입으로 변환
    // Serial.println(deviceNumber);

    // 연결된 WiFi 네트워크의 이름과 IP 주소를 표시
    Serial.print("연결된 네트워크 이름: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP 주소: ");
    Serial.println(WiFi.localIP());

    // digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
  }
  else
  {
    wifiManager.autoConnect();
    Serial.println("\nConnected to WiFi network!");
    // digitalWrite(LED1_PIN, HIGH);
  }

  digitalWrite(LED1_PIN, HIGH);
  g_ts.startNow();
}

void loop()
{
  g_ts.execute();
}