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

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "packet.hpp"
#include "tonkey.hpp"

#include "config.hpp"

#include "sampling_module.hpp"

extern String ParseCmd(String _strLine);

Scheduler g_ts;
tonkey g_MainParser;

Config g_config;
AsyncUDP udp;

IPAddress targetIP;
uint16_t udp_port_data;
uint16_t udp_port_broadcast;

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
Task task_Broadcast(3000, TASK_FOREVER, []()
                    {

  if (WiFi.status() == WL_CONNECTED) {
    JsonDocument broadcastDoc;
    // broadcastDoc["chipid"] = "udpcm_";
    broadcastDoc["chipid"] = chipid;
    broadcastDoc["type"] = "broadcast";
    
    String broadcastMessage;
    serializeJson(broadcastDoc, broadcastMessage);

    udp.broadcastTo(broadcastMessage.c_str(), udp_port_broadcast);
  } }, &g_ts, false);

Task task_SendBuffer(50, TASK_FOREVER, []() {

    // sampling_module::sendUdpData(udp, targetIP, udp_port_data);
  
}, &g_ts, true);


#ifdef ESP8266
// WiFi events
WiFiEventHandler staConnectedHandler;
WiFiEventHandler staGotIPHandler;
WiFiEventHandler staDisconnectedHandler;
#endif


void sendResponse(AsyncUDP& udp, const IPAddress& remoteIP, uint16_t remotePort, uint8_t resultCode) {
    S_PACKET_RES response;
    response.header.header = MAGIC_NUMBER;
    response.header.chipId = ESP.getEfuseMac(); // ESP32의 고유 ID
    response.header.type = PacketType::RES;
    response.header.packet_size = sizeof(S_PACKET_RES);
    response.result_code = resultCode;

    udp.writeTo((uint8_t*)&response, sizeof(S_PACKET_RES), remoteIP, remotePort);
}

void handleRequest( const S_PACKET_REQ* req, const IPAddress& remoteIP, uint16_t remotePort) {
    uint8_t resultCode = 0; // 성공을 의미하는 기본값

    switch(req->cmd) {
        case Command::CMD_PING:

            targetIP = remoteIP;
            // Serial.print("Ping received from ");
            // Serial.print(remoteIP.toString());
            // Serial.print(":");
            // Serial.println(remotePort);
            // Serial.println("Ping received");
            // 상태 정보 처리

            break;
        case Command::CMD_START_SAMPLING:
            break;
        case Command::CMD_STOP_SAMPLING:
            break;
        default:
            resultCode = 1; // 알 수 없는 명령어
            break;
    }

    sendResponse(udp, remoteIP, remotePort, resultCode);
}

// UDP 수신 콜백 함수
void onPacketReceived(AsyncUDPPacket packet)
{

  Serial.print("UDP Packet Type: ");
  //브로드 캐스팅
  if(packet.isBroadcast()) {
    Serial.println("broadcast");
  }
  else if(packet.isMulticast()) {
    Serial.println("multicast");
  }
  else {

    Serial.println("unicast");

    S_PACKET_HEADER* header = (S_PACKET_HEADER*)packet.data();

    Serial.print("magic number: ");
    Serial.println(header->header);

    Serial.print("type: ");
    Serial.println(header->type);

    switch(header->type) {
        case PacketType::REQ:
            handleRequest( (S_PACKET_REQ*)packet.data(), packet.remoteIP(), packet.remotePort());
            
            break;
        case PacketType::RES:
            // 응답 패킷 처리 (필요한 경우)
            break;
        case PacketType::SYS:
            // 시스템 패킷 처리
            break;
        default:
            Serial.println("Unknown packet type");
            break;
    }



    
  }
}

// the setup function runs once when you press reset or power the board
void setup()
{
#ifdef ESP8266
  chipid += String(ESP.getChipId());
  // broadcastDoc["chipid"] += String(ESP.getChipId());
#elif ESP32
  chipid += String(ESP.getEfuseMac());
  // broadcastDoc["chipid"] += String(ESP.getEfuseMac());
#endif
  
  udp_port_data = g_config.get<int>("port",8284);
  udp_port_broadcast = g_config.get<int>("broadcast_port",7204);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

  Serial.begin(115200);
  // g_config.load();

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

                        // UDP 수신 시작
                        if (udp.listen(udp_port_data))
                        { 
                          udp.onPacket(onPacketReceived);

                          Serial.println("UDP Listening on port " + String(udp_port_data));
                        }




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

  // sampling_module::setup();

}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}