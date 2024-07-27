#include <Arduino.h>
#include <math.h>


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

// int pinDHT11 = D5;
// SimpleDHT11 dht11(pinDHT11);

#elif ESP32
#include <AsyncUDP.h>
#define LED_BUILTIN 4
#endif

AsyncUDP udp;

IPAddress targetIP;
uint16_t udp_port = 8284;
uint16_t udp_port_broadcast = 7204;


IPAddress g_remoteIp;
uint16_t g_remotePort = 0;

bool dnsResolved = false;
String remoteHostName;

float g_Temperature = 0.0f;

String chipid = "boltt2-";

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
      _res_doc["app"] = "iceDay";
      _res_doc["version"] = g_config.version;
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
    else if(cmd == "temperature")
    {
      _res_doc["result"] = "ok";
      _res_doc["temperature"] = g_Temperature;
    }
    else
    {
      _res_doc["result"] = "fail";
      _res_doc["ms"] = "unknown command " + _strLine;
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
        
    } }, &g_ts, true);

// blink task
Task task_Blink(150, TASK_FOREVER, []()
                { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); }, &g_ts, true);

// udp broadcast task
Task task_Broadcast(3000, TASK_FOREVER, []()
                    {

  if (WiFi.status() == WL_CONNECTED) {
    JsonDocument broadcastDoc;
    broadcastDoc["chipid"] = chipid;
    broadcastDoc["type"] = "broadcast";
    
    String broadcastMessage;
    serializeJson(broadcastDoc, broadcastMessage);
    udp.broadcastTo(broadcastMessage.c_str(), udp_port_broadcast);

  } }, &g_ts, false);



const int analogPin = A0;
const int seriesResistor = 10000; // 10kΩ 저항

// NTC 서미스터의 B 파라미터와 T0 (서미스터의 참조 온도)
const float B_CONSTANT = 3435; // 서미스터의 B 파라미터 (3435K)
const float T0 = 298.15; // 참조 온도 25도씨 (켈빈 값: 25 + 273.15)
const float R0 = 10000; // 서미스터의 참조 저항 (10kΩ)

// 측정하려는 온도 범위에 대한 전압-저항 변환
float readThermistor(int pin) {
  int analogValue = 1024 - analogRead(pin);
  float voltage = analogValue * (3.3 / 1023.0);
  float resistance = seriesResistor * (3.3 / voltage - 1.0);
  return resistance;
}

// 서미스터의 저항 값을 온도로 변환 (켈빈, 섭씨 변환)
float thermistorToTemperature(float resistance) {
  float lnR = log(resistance / R0);
  float temperatureK = 1.0 / (1.0 / T0 + lnR / B_CONSTANT);
  float temperatureC = temperatureK - 273.15;
  return temperatureC;
}


Task task_Temperature(5000, TASK_FOREVER, []() {

  float resistance = readThermistor(analogPin);
  float temperatureC = thermistorToTemperature(resistance);
  
  g_Temperature = temperatureC;

  String _res = "#POST_" + chipid + "_TempSensor_" + String(temperatureC);

  // Serial.println(_res);

  udp.writeTo((byte *)_res.c_str(), _res.length(), g_remoteIp, g_remotePort);
  
    
},&g_ts,false);




// UDP 수신 콜백 함수
void onPacketReceived(AsyncUDPPacket packet)
{
  //브로드 캐스팅
  if(packet.isBroadcast()) {
    // Serial.println("broadcast");
  }
  else if(packet.isMulticast()) {
    // Serial.println("multicast");
  }
  else {
    // target ip 등록
    //  IPAddress targetIP = packet.remoteIP();
    // targetIP = packet.remoteIP();

    // Serial.write(packet.data(), packet.length());
    // // Serial.print(" from " + packet.remoteIP().toString() + ":" + packet.localPort());
    // Serial.println();

  }
}

// DNS resolution task
Task task_Dns(0, TASK_ONCE, []() {

    Serial.println("Resolving DNS...");
    IPAddress resolvedIP;
    //   Serial.println("Resolving DNS...");
    if (WiFi.hostByName(remoteHostName.c_str(), resolvedIP)) {
        g_remoteIp = resolvedIP;
        dnsResolved = true;
        Serial.println("DNS resolved: " + g_remoteIp.toString());

        // UDP 수신 시작
        if (udp.listen(udp_port))
        { 
          udp.onPacket(onPacketReceived);
        }

        // UDP 브로드캐스트 시작
        task_Broadcast.enable(); 
        task_Temperature.enable();

        // if (udp.listen(udp_port)) {
        //   Serial.println("UDP Listening on port " + String(udp_port));
        //   udp.onPacket(onPacketReceived);
        // }
        
    } else {
        Serial.println("DNS resolution failed. Retrying...");
        task_Dns.restartDelayed(TASK_SECOND * 5);  // 5초 후 재시도
    }
},&g_ts,false);

// WiFi events
WiFiEventHandler staConnectedHandler;
WiFiEventHandler staGotIPHandler;
WiFiEventHandler staDisconnectedHandler;


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

      if (g_config.hasKey("remoteHost") && g_config.hasKey("remotePort"))
      {
          remoteHostName = g_config.get<String>("remoteHost");
          g_remotePort = g_config.get<int>("remotePort");

          Serial.print("Remote host: ");
          Serial.println(remoteHostName);
          Serial.print("Remote port: ");
          Serial.println(g_remotePort);

          // runner.addTask(dnsTask);
          task_Dns.enable();     
      }
                         
    });

    staDisconnectedHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event)
                                                            {
                                                              Serial.println("WiFi lost connection");
                                                              digitalWrite(LED_BUILTIN, HIGH);
                                                              // start blink task
                                                              task_Blink.enable();

                                                              task_Broadcast.enable(); });

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

  task_Temperature.enable();

  g_ts.startNow();
}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}