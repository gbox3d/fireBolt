#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESPAsyncUDP.h>

#elif ESP32
#include <WiFi.h>
#include <vector>
#include <AsyncUDP.h>

#define LED_BUILTIN 5
#endif

#include <SPIFFS.h>

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "packet.hpp"
#include "tonkey.hpp"

#include "config.hpp"

#include "sampling_module.hpp"
#include "tcp_server.hpp"

extern String ParseCmd(String _strLine);

Scheduler g_ts;
Scheduler g_ts2;  // 두 번째 스케줄러 추가

tonkey g_MainParser;

Config g_config;
AsyncUDP udp;

IPAddress targetIP;

uint16_t data_port;
uint16_t udp_port_broadcast;

const int MIC_PINS[] = {32, 33, 25, 26};
const int LED_PINS[] = {2,12,15};
const int NUM_CHANNELS = 4;
const int SAMPLE_RATE = 8000;
const int BUFFER_SIZE = 8000;  // 각 채널당 1000 샘플

// SamplingModule sampler(MIC_PINS, NUM_CHANNELS, SAMPLE_RATE, BUFFER_SIZE);
SamplingModule sampler(MIC_PINS, NUM_CHANNELS, SAMPLE_RATE, BUFFER_SIZE);
TcpServer *g_pTcpServer = nullptr;

// #if defined(ESP8266)
// #elif defined(ESP32)
// int g_analog_pins[] = {34, 35, 36, 39};
// int g_digital_pins[] = {32, 33, 25, 26};//, 27}; // , 14, 12, 13, 23, 22, 21, 19, 18, 5, 17, 16, 4, 0, 2, 15};
// #endif

String chipid = "DAQ_";

Task task_test(1000, TASK_FOREVER, []()
{
}, &g_ts, false);

void start_task_test()
{
  task_test.enable();
}
void stop_task_test()
{
  task_test.disable();
}


Task task_Cmd(100, TASK_FOREVER, []()
              {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');

        _strLine.trim();
        String _res = ParseCmd(_strLine);
        Serial.println(_res);
    } }, &g_ts, true);

// blink task
Task task_Blink(150, TASK_FOREVER, []()
                { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); }, &g_ts, true);

// udp broadcast task
Task task_Broadcast(5000, TASK_FOREVER, []()
                    {

  if (WiFi.status() == WL_CONNECTED) {
    JsonDocument broadcastDoc;
    broadcastDoc["chipid"] = chipid;
    broadcastDoc["type"] = "broadcast";
    
    String broadcastMessage;
    serializeJson(broadcastDoc, broadcastMessage);

    udp.broadcastTo(broadcastMessage.c_str(), udp_port_broadcast);
  } }, &g_ts, false);

// Task task_SendBuffer(50, TASK_FOREVER, []() {

//     // sampling_module::sendUdpData(udp, targetIP, udp_port_data);
  
// }, &g_ts, true);


// Task2를 위한 FreeRTOS 태스크 함수
void Task2(void * pvParameters) {
    for(;;) {
        g_ts2.execute();
        vTaskDelay(1);
    }
}

Task task_SendData(250, TASK_FOREVER, []() {
  
  g_pTcpServer->sendDataByEvent();

}, &g_ts2, true);


#ifdef ESP8266
// WiFi events
WiFiEventHandler staConnectedHandler;
WiFiEventHandler staGotIPHandler;
WiFiEventHandler staDisconnectedHandler;
#endif

// the setup function runs once when you press reset or power the board
void setup()
{

  sampler.setup();
  

#ifdef ESP8266
  chipid += String(ESP.getChipId());
  // broadcastDoc["chipid"] += String(ESP.getChipId());
#elif ESP32
  chipid += String(ESP.getEfuseMac());
  // broadcastDoc["chipid"] += String(ESP.getEfuseMac());
#endif
  
  data_port = g_config.get<int>("port",8284);
  udp_port_broadcast = g_config.get<int>("broadcast_port",7204);

  g_pTcpServer = new TcpServer(data_port,&sampler,LED_PINS);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

  for(int i=0;i< sizeof(LED_PINS)/sizeof(int);i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }


  Serial.begin(115200);
  // g_config.load();

  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS Mount Failed");
    if(SPIFFS.format()){
        Serial.println("SPIFFS Formatted");
        if(SPIFFS.begin(true)){
            Serial.println("SPIFFS Mounted");
        } else {
            Serial.println("SPIFFS Mount Failed after format");
        }
    } else {
        Serial.println("SPIFFS Format Failed");
    }
  }
  else {
    Serial.println("SPIFFS Mounted");
  }

  // ssid, password 가 졵재하면 wifi 연결
  if (g_config.hasKey("ssid") && g_config.hasKey("password"))
  {

    Serial.println("WiFi connecting");
    Serial.print("ssid: ");
    Serial.println(g_config.get<String>("ssid"));
    Serial.print("password: ");
    Serial.println(g_config.get<String>("password"));

#if defined(ESP8266)
    // WiFi event handlers
    // on connected
    staConnectedHandler = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &event)
                                                      { Serial.println("WiFi connected : " + event.ssid); });

    staGotIPHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event)
                                              {
                                                Serial.println("Get IP address");
                                                Serial.print("IP address: ");
                                                Serial.println(WiFi.localIP());
                                                // stop blink task
                                                task_Blink.disable();

                                                digitalWrite(LED_BUILTIN, LOW);

                                                Serial.println("UDP Listening on port " + String(udp_port));

                                                // UDP 수신 시작
                                                if (udp.listen(udp_port))
                                                { 
                                                  udp.onPacket(onPacketReceived);
                                                }

                                                // UDP 브로드캐스트 시작
                                                task_Broadcast.enable(); });

    staDisconnectedHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event)
                                                            {
                                                              Serial.println("WiFi lost connection");
                                                              digitalWrite(LED_BUILTIN, HIGH);
                                                              // start blink task
                                                              task_Blink.enable();

                                                              task_Broadcast.enable(); });
#elif defined(ESP32)

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      Serial.printf("[WiFi-event] event: %d\n", event);

      switch(event) {
        case SYSTEM_EVENT_STA_GOT_IP:
          Serial.println("WiFi connected");
          Serial.print("IP address: ");
          Serial.println(WiFi.localIP());

          task_Blink.disable();
          digitalWrite(LED_BUILTIN, HIGH);

          task_Broadcast.enable();

          if(g_pTcpServer) g_pTcpServer->begin();
          
          break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          task_Blink.enable();
          // digitalWrite(LED_PIN, LOW);
          break;
        default: break;
      }
    });

#endif

    String _ssid = g_config.get<String>("ssid");
    String _password = g_config.get<String>("password");

    WiFi.begin(_ssid.c_str(), _password.c_str());
  }
  else
  {
    Serial.println("WiFi not connected");
  }

  Serial.println(":-]");
  Serial.println("Serial connected");

  g_ts.startNow();
  g_ts2.startNow();  // 두 번째 스케줄러 시작

  // Task2를 코어 1에 할당
  xTaskCreatePinnedToCore(
      Task2,
      "Task2",
      10000,
      NULL,
      1,
      NULL,
      1  // 코어 1에 할당
  );

  // sampling_module::setup();

}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}