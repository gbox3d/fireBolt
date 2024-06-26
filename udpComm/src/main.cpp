#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#include <vector>
#endif

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "tonkey.hpp"

#include "config.hpp"

Scheduler g_ts;
tonkey g_MainParser;

Config g_config;

#ifdef ESP8266
#include <ESPAsyncUDP.h>
#elif ESP32
#include <AsyncUDP.h>
#define LED_BUILTIN 4
#endif

AsyncUDP udp;

IPAddress targetIP;
uint16_t udp_port = 7204;

// #ifdef ESP32
// #define LED_BUILTIN 4
// #endif

String ParseCmd(String _strLine)
{
  g_MainParser.parse(_strLine);

  if (g_MainParser.getTokenCount() > 0)
  {
    String cmd = g_MainParser.getToken(0);
    JsonDocument _res_doc;
    if (cmd == "about")
    {
      /* code */
      _res_doc["result"] = "ok";
      _res_doc["os"] = "cronos-v1";
      _res_doc["app"] = "udpComm";
      _res_doc["version"] = "1.0.0";
      _res_doc["author"] = "gbox3d";
// esp8266 chip id
#ifdef ESP8266
      _res_doc["chipid"] = ESP.getChipId();
#elif ESP32
      _res_doc["chipid"] = ESP.getEfuseMac();
#endif
    }
    else if (cmd == "reboot")
    {
#ifdef ESP8266
      ESP.reset();
#elif ESP32
      ESP.restart();
#endif
    }
    else if (cmd == "config")
    {
      if (g_MainParser.getTokenCount() > 1)
      {
        String subCmd = g_MainParser.getToken(1);
        if (subCmd == "load")
        {
          g_config.load();
          _res_doc["result"] = "ok";
          _res_doc["ms"] = "config loaded";
        }
        else if (subCmd == "save")
        {
          g_config.save();
          _res_doc["result"] = "ok";
          _res_doc["ms"] = "config saved";
        }
        else if (subCmd == "dump")
        {

          // parse json g_config.dump()
          String jsonStr = g_config.dump();
          DeserializationError error = deserializeJson(_res_doc["ms"], jsonStr);
          if (error)
          {
            // Serial.print(F("deserializeJson() failed: "));
            // Serial.println(error.f_str());
            // return;
            _res_doc["result"] = "fail";
            _res_doc["ms"] = "json parse error";
          }
          else
          {
            _res_doc["result"] = "ok";
          }
        }
        else if (subCmd == "clear")
        {
          g_config.clear();
          _res_doc["result"] = "ok";
          _res_doc["ms"] = "config cleared";
        }
        else if (subCmd == "set")
        {
          if (g_MainParser.getTokenCount() > 2)
          {
            String key = g_MainParser.getToken(2);
            String value = g_MainParser.getToken(3);
            g_config.set(key.c_str(), value);
            _res_doc["result"] = "ok";
            _res_doc["ms"] = "config set";
          }
          else
          {
            _res_doc["result"] = "fail";
            _res_doc["ms"] = "need key and value";
          }
        }
        else if (subCmd == "setA")
        { // set json array
          if (g_MainParser.getTokenCount() > 2)
          {
            String key = g_MainParser.getToken(2);
            String value = g_MainParser.getToken(3);
            // parse json value
            //  JSON 문자열 파싱을 위한 임시 객체
            JsonDocument tempDoc; // 임시 JSON 문서

            // JSON 문자열 파싱
            DeserializationError error = deserializeJson(tempDoc, value);
            // DeserializationError error = deserializeJson(g_config[key.c_str()], value);
            if (error)
            {
              // Serial.print(F("deserializeJson() failed: "));
              // Serial.println(error.f_str());
              // return;
              _res_doc["result"] = "fail";
              _res_doc["ms"] = "json parse error";
            }
            else
            {
              // JsonArray array = tempDoc[key].as<JsonArray>();

              g_config.set(key.c_str(), tempDoc);
              _res_doc["result"] = "ok";
              _res_doc["ms"] = tempDoc;
            }
            // g_config.set(key.c_str(), value);
            // _res_doc["result"] = "ok";
            // _res_doc["ms"] = "config set";
          }
          else
          {
            _res_doc["result"] = "fail";
            _res_doc["ms"] = "need key and value";
          }
        }
        else if (subCmd == "get")
        {
          if (g_MainParser.getTokenCount() > 2)
          {
            String key = g_MainParser.getToken(2);

            // check key exist
            if (!g_config.hasKey(key.c_str()))
            {
              _res_doc["result"] = "fail";
              _res_doc["ms"] = "key not exist";
              // serializeJson(_res_doc, Serial);
              // Serial.println();
              // return;
            }
            else
            {
              _res_doc["result"] = "ok";
              _res_doc["value"] = g_config.get<String>(key.c_str());
            }
          }
          else
          {
            _res_doc["result"] = "fail";
            _res_doc["ms"] = "need key";
          }
        }
        else
        {
          _res_doc["result"] = "fail";
          _res_doc["ms"] = "unknown sub command";
        }
      }
      else
      {
        _res_doc["result"] = "fail";
        _res_doc["ms"] = "need sub command";
      }
    }

    else
    {
      _res_doc["result"] = "fail";
      _res_doc["ms"] = "unknown command";
    }

    // serializeJson(_res_doc, Serial);
    // Serial.println();

    return _res_doc.as<String>();
    // String
  }
  else
  {
    return "";
  }
}

Task task_Cmd(100, TASK_FOREVER, []()
              {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();

        String _res = ParseCmd(_strLine);
        Serial.println(_res);


        Serial.println("Sent to target IP: " + targetIP.toString() + ":" + udp_port);


        udp.writeTo((const uint8_t*)_strLine.c_str(), _strLine.length(), targetIP, udp_port);

        // //json 형식문자열인지 판단
        // if(_strLine.startsWith("{")) {
        //   String _res = ParseCmd(_strLine);
        //   Serial.println(_res);
        //   // udp.broadcastTo(_strLine.c_str(), udp_port);
        //   // return;
        // }
        // else {
        //   //target ip 로 전송
        //   // udp.writeTo(_strLine.c_str(), _strLine.length(), targetIP, udp_port);
        //   udp.writeTo((const uint8_t*)_strLine.c_str(), _strLine.length(), targetIP, udp_port);
        //   // Serial.println("Sent to target IP: " + targetIP.toString());

        // }
        // Serial.println(_strLine);
        
    } }, &g_ts, true);

// blink task
Task task_Blink(150, TASK_FOREVER, []()
                { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); }, &g_ts, true);


// udp broadcast task
Task task_Broadcast(5000, TASK_FOREVER, []() {

  if (WiFi.status() == WL_CONNECTED) {
    JsonDocument broadcastDoc;

    String chipid = "udpcm_";
    // broadcastDoc["chipid"] = "udpcm_";
    
#ifdef ESP8266
    chipid += String(ESP.getChipId());
    // broadcastDoc["chipid"] += String(ESP.getChipId());
    
#elif ESP32
    chipid += String(ESP.getEfuseMac());
    // broadcastDoc["chipid"] += String(ESP.getEfuseMac());
#endif

    broadcastDoc["chipid"] = chipid;
    broadcastDoc["type"] = "broadcast";
    
    String broadcastMessage;
    serializeJson(broadcastDoc, broadcastMessage);

    udp.broadcastTo(broadcastMessage.c_str(), udp_port);
  } 
}, &g_ts, false);

// WiFi events
WiFiEventHandler staConnectedHandler;
WiFiEventHandler staGotIPHandler;
WiFiEventHandler staDisconnectedHandler;


// UDP 수신 콜백 함수
void onPacketReceived(AsyncUDPPacket packet) {

  //target ip 등록
  // IPAddress targetIP = packet.remoteIP();
  targetIP = packet.remoteIP();

  Serial.write(packet.data(), packet.length());
}

// the setup function runs once when you press reset or power the board
void setup()
{
  udp_port = g_config.get<int>("port");
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


    // WiFi event handlers
    // on connected
    staConnectedHandler = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &event) { 

      Serial.println( "WiFi connected : " + event.ssid); 
    });

    staGotIPHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
      Serial.println("Get IP address");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      //stop blink task
      task_Blink.disable();

      digitalWrite(LED_BUILTIN, LOW); 

          // UDP 수신 시작
      if(udp.listen(udp_port)) {  // 8888 포트로 UDP 수신 대기
        // Serial.println("UDP Listening on port 8888");
        udp.onPacket(onPacketReceived);
      }

      // UDP 브로드캐스트 시작
      task_Broadcast.enable();

    });

    staDisconnectedHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event) {
      Serial.println("WiFi lost connection");
      digitalWrite(LED_BUILTIN, HIGH); 
      //start blink task
      task_Blink.enable();

      task_Broadcast.enable();

    });

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
}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}