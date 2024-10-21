#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <vector>
#endif

#include <TaskScheduler.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// #include "tonkey.hpp"

#include "config.hpp"
#include "etc.hpp"

Scheduler g_ts;
Config g_config;

String g_ApiUrl;
int g_restCallTerm; // REST API 호출 주기
String g_LogID;

extern String parseCmd(String _strLine);

// #ifdef ESP32
// #define LED_BUILTIN 4
// #endif

#if defined(LOLIN_D32) | defined(LOLIN_D32_PRO) | defined(WROVER_KIT)
// this device aready defined LED_BUILTIN 4 -> D5 

#elif defined(SEED_XIAO_ESP32C3)
#define LED_BUILTIN D10

#endif

Task task_LedBlink(500, TASK_FOREVER, []()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}, &g_ts, true);

Task task_Cmd(100, TASK_FOREVER, []()
{
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(parseCmd(_strLine));
    }
}, &g_ts, true);

#if defined(LOLIN_D32) | defined(LOLIN_D32_PRO) | defined(LOLIN_D32_LITE)
int ds18b20Pin = 27;
#elif defined(ESP8266)
int ds18b20Pin = D5;
#endif

OneWire oneWire(ds18b20Pin);
DallasTemperature sensor_dallas(&oneWire);


WiFiClient wifiClient; // WiFiClient 인스턴스 생성

Task task_RestCall(10000, TASK_FOREVER, []()
{
    sensor_dallas.requestTemperatures();
    float temperature = sensor_dallas.getTempCByIndex(0);
    
    // HTTP POST
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(wifiClient, g_ApiUrl + "/api/logs/update"); // WiFiClient 사용

        // JSON 데이터 생성 (StaticJsonDocument 또는 DynamicJsonDocument 사용)
        // DynamicJsonDocument doc(1024); // 동적 메모리 할당
        JsonDocument doc;

        // JSON 데이터 추가
        // doc["id"] = "67136f184d4d7e4e6371a943"; //log id
        doc["id"] = g_LogID;
        JsonObject update = doc["update"].to<JsonObject>(); // 명시적으로 JsonObject로 변환
        update["devid"] = getChipID();
        update["temperature"] = temperature;

        // JSON 문자열 생성
        String jsonData;
        serializeJson(doc, jsonData);

        // HTTP 요청 보내기
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0)
        {
            Serial.printf("HTTP Response code: %d\n", httpResponseCode);
            String response = http.getString();
            Serial.println("Response: " + response);
        }
        else
        {
            Serial.printf("Error on sending POST: %s\n", http.errorToString(httpResponseCode).c_str());
        }

        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected. Skipping REST call.");
    }

}, &g_ts, false);


#ifdef ESP32

void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("WiFi Connected");
        Serial.printf("SSID: %s\n", (const char *)WiFi.SSID().c_str());
        stopBlink();
        break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println("WiFi Got IP Address");
        Serial.println(WiFi.localIP());

        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("UDP Listening on port " + String(udp_port_data));

        task_Broadcast.enable();

        // 온도 센서 작업 활성화
        startDS18B20();
        task_RestCall.enable();

        Serial.println("start broadcast on port  " + String(udp_port_broadcast));
        Serial.println("device name: " + strDeviceName);
        break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("WiFi Disconnected");
        Serial.printf("Reason: %d\n", info.wifi_sta_disconnected.reason);
        WiFi.begin(); // 자동 재연결
        startBlink();
        task_RestCall.disable();
        break;

    default:
        break;
    }
}

#elif ESP8266

WiFiEventHandler staConnectedHandler;
WiFiEventHandler staGotIPHandler;
WiFiEventHandler staDisconnectedHandler;
#endif

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    Serial.begin(115200);

    g_config.load();

    delay(250);

    Serial.println(":-]");
    Serial.println("Serial connected");
    
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    Serial.printf("Chip ID: %s\n", getChipID().c_str());
    Serial.printf("Board: %s\n", ARDUINO_BOARD);
    Serial.println("LED_BUILTIN: " + String(LED_BUILTIN));
    Serial.printf("Config System Revision: %d\n", Config::SystemVersion);

    String ssid = g_config.get<String>("ssid", "redstar0427");
    String password = g_config.get<String>("password", "123456789a");

    g_ApiUrl = g_config.get<String>("api_url", "http://gears001.iptime.org:21037");
    g_restCallTerm = g_config.get<int>("rest_call_term", 10000);
    g_LogID = g_config.get<String>("log_id", "67136f184d4d7e4e6371a943");

    Serial.println("try to connect WiFi.....");
    Serial.printf("ssid: %s\n", ssid.c_str());
    Serial.printf("password: %s\n", password.c_str());

    Serial.printf("API URL: %s\n", g_ApiUrl.c_str());
    Serial.printf("REST Call Term: %d\n", g_restCallTerm);
    Serial.printf("Log ID: %s\n", g_LogID.c_str());

    #ifdef ESP32
    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid.c_str(), password.c_str());
    #elif ESP8266
    staConnectedHandler = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &event)
    {
        Serial.println("WiFi connected : " + event.ssid);
    });

    staGotIPHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event)
    {
        Serial.println("Get IP address");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        task_LedBlink.disable();
        digitalWrite(LED_BUILTIN, LOW);

         // 초기 설정 로드
        task_RestCall.setInterval(g_restCallTerm); // 태스크 주기 설정

        task_RestCall.enable();
    });

    staDisconnectedHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event)
    {
        Serial.println("WiFi lost connection");
        digitalWrite(LED_BUILTIN, HIGH);
        task_LedBlink.enable();
        task_RestCall.disable();
    });

    WiFi.begin(ssid.c_str(), password.c_str());
    #endif

    g_ts.startNow();
}

void loop()
{
    g_ts.execute();
}
