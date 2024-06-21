#include <Arduino.h> // Arduino 헤더 파일 포함
#include <TaskScheduler.h>
#include <WiFi.h>
#include <AsyncUDP.h>

#include <SimpleDHT.h>

#include <tonkey.hpp>

#include "config.hpp"

#ifdef SEED_XIAO_ESP32C3
const int ledPins[] = {D10, D9};
const int analogPins[] = {D0, D1};
const int buttonPins[] = {D8, D2};

#elif defined(LOLIN_D32)

const int ledPins[] = {D10, D9};
const int analogPins[] = {D0, D1};
const int buttonPins[] = {D8, D2};
const int pinDHT11 = 18;

#elif defined(LOLIN32_LITE)
const int ledPins[] = {13, 14};
const int buttonPins[] = {36, 39};
const int analogPins[] = {34, 35};
const int pinDHT11 = 18;
const int batteryPin = 33;
const int builtingLed = BUILTIN_LED;

#endif

std::vector<int> vec;

// for DHT11,
SimpleDHT11 dht11(pinDHT11);

String strBroadCastMsg;
AsyncUDP udp;

unsigned int localUdpPort = 7204;

// WiFi 이벤트 핸들러 함수 정의
void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_STA_CONNECTED:
    Serial.println("Connected to WiFi");
    digitalWrite(builtingLed, LOW);
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("Disconnected from WiFi");
    digitalWrite(builtingLed, HIGH);
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());
    break;
  default:
    break;
  }
}

Scheduler scheduler;
tonkey g_MainParser;
CCongifData configData;

const char *VersionString = "1.0";

Task task_udpBroadCast(
    5000, TASK_FOREVER, []()
    {
      udp.broadcastTo(strBroadCastMsg.c_str(), localUdpPort);
      // Serial.println("udp broadcast");
    });

String processCommand(String _strLine)
{
  String _strResult = "";
  g_MainParser.parse(_strLine);

  if (g_MainParser.getTokenCount() > 0)
  {
    String cmd = g_MainParser.getToken(0);

    if (cmd == "about")
    {
      _strResult = "bolt_t1_v" + String(VersionString) + "\n";
    }
    else if (cmd == "reboot")
    {
      ESP.restart();
    }
    else if (cmd == "save")
    {
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
      _strResult = configData.print();
    }
    else if ((cmd == "scan"))
    {
      int n = WiFi.scanNetworks();
      Serial.println("scan done");
      if (n == 0)
        Serial.println("no networks found");
      else
      {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
          // Print SSID and RSSI for each network found
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")");
          Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
          delay(10);
        }
      }
      Serial.println("");
    }
    else if ((cmd == "connect"))
    {
      if (g_MainParser.getTokenCount() < 3)
      {
        _strResult = "err:invalid parameter\n";
      }
      else
      {
        configData.mStrAp = g_MainParser.getToken(1);
        configData.mStrPassword = g_MainParser.getToken(2);
        WiFi.begin(configData.mStrAp.c_str(), configData.mStrPassword.c_str());
      }
    }
    else if ((cmd == "disconnect"))
    {
      WiFi.disconnect();
    }
    else if (cmd == "target")
    {
      if (g_MainParser.getTokenCount() < 3)
      {
        _strResult = "err:invalid parameter\n";
      }
      else
      {
        configData.mTargetIp = g_MainParser.getToken(1);
        configData.mTargetPort = g_MainParser.getToken(2).toInt();
        _strResult = "target:" + configData.mTargetIp + ":" + String(configData.mTargetPort) + "\n";
      }
    }
    else if (cmd == "led")
    {
      if (g_MainParser.getTokenCount() < 3)
      {
        _strResult = "err:invalid parameter\n";
      }
      else
      {
        int n = g_MainParser.getToken(1).toInt();
        int s = g_MainParser.getToken(2).toInt();
        digitalWrite(ledPins[n], s);
        _strResult = "led:" + String(n) + ":" + String(s) + "\n";
      }
    }
    else if (cmd == "analog")
    {
      if (g_MainParser.getTokenCount() < 2)
      {
        _strResult = "err:invalid parameter\n";
        // return "#RES_" + _strResult + "\nOK\n";
      }
      else
      {
        int n = g_MainParser.getToken(1).toInt();
        int s = analogRead(analogPins[n]);
        _strResult = "analog:" + String(s) + "\n";
      }
    }
    else if (cmd == "button")
    {
      if (g_MainParser.getTokenCount() < 2)
      {
        _strResult = "err:invalid parameter\n";
      }
      else
      {
        int n = g_MainParser.getToken(1).toInt();
        int s = digitalRead(buttonPins[n]);
        _strResult = "button:" + String(s) + "\n";
      }
    }
    else if (cmd == "battery")
    {
      uint32_t Vbatt = 0;
      for (int i = 0; i < 16; i++)
      {
        Vbatt = Vbatt + analogReadMilliVolts(batteryPin); // ADC with correction
      }
      float Vbattf = 2 * Vbatt / 16 / 1000.0; // attenuation ratio 1/2, mV --> V
      // Serial.println(Vbattf, 3);
      // delay(1000);
      _strResult = "battery:" + String(Vbattf, 3) + "\n";
    }
    else if (cmd == "dht11")
    {
      byte temperature = 0;
      byte humidity = 0;

      int err = SimpleDHTErrSuccess;

      int count = 0;
      static const int maxCount = 10;

      // digitalWrite(pinDHT11, LOW);

      while ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
      {
        // Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
        // Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
        // return;
        // _strResult = "err:" + String(SimpleDHTErrCode(err)) + ", " + String(SimpleDHTErrDuration(err)) + "\n";

        delay(500);

        count++;

        if (count > maxCount)
        {
          break;
        }
        // break;
      }

      Serial.println(count);
      if (count > maxCount)
      {
        _strResult = "dht11:err\n";
      }
      else
      {
        _strResult = "dht11:" + String((int)temperature) + "C, " + String((int)humidity) + "H\n";
      }
    }
    else if ((cmd == "status"))
    {
      // Serial.print("WiFi status: ");
      // Serial.println(WiFi.status());
      _strResult = "WiFi status: " + String(WiFi.status()) + "\n";
    }
    else if ((cmd == "ip"))
    {
      // Serial.print("IP address: ");
      // Serial.println(WiFi.localIP());
      _strResult = "IP address: " + WiFi.localIP().toString() + "\n";
    }
    else if ((cmd == "mac"))
    {
      // Serial.print("MAC address: ");
      // Serial.println(WiFi.macAddress());
      _strResult = "MAC address: " + WiFi.macAddress() + "\n";
    }
    else if ((cmd == "rssi"))
    {
      // Serial.print("RSSI: ");
      // Serial.println(WiFi.RSSI());
      _strResult = "RSSI: " + String(WiFi.RSSI()) + "\n";
    }
    else if ((cmd == "gateway"))
    {
      // Serial.print("Gateway: ");
      // Serial.println(WiFi.gatewayIP());
      _strResult = "Gateway: " + WiFi.gatewayIP().toString() + "\n";
    }
    else if ((cmd == "dns"))
    {
      // Serial.print("DNS: ");
      // Serial.println(WiFi.dnsIP());
      _strResult = "DNS: " + WiFi.dnsIP().toString() + "\n";
    }
    else if (cmd == "start_broadcast")
    {
      task_udpBroadCast.enable();
      _strResult = "start_broadcast ok\n";
    }
    else if (cmd == "stop_broadcast")
    {
      task_udpBroadCast.disable();
      _strResult = "stop_broadcast ok\n";
    }
    else if (cmd == "send")
    {
      IPAddress serverIP;
      serverIP.fromString(g_MainParser.getToken(1));
      unsigned int port = g_MainParser.getToken(2).toInt();

      String msg = g_MainParser.getToken(3);

      udp.writeTo((const uint8_t *)msg.c_str(), msg.length(), serverIP, port);

      _strResult = "send ok\n";
    }
    else if (cmd == "help")
    {

      _strResult = "about\nreboot\nsave\nload\nclear\nprint\nscan\nconnect\ndisconnect\ntarget\nled\nanalog\nbutton\nbattery\ndht11\nstatus\nip\nmac\nrssi\ngateway\ndns\nstart_broadcast\nstop_broadcast\nsend\n";
      _strResult += "version " + String(VersionString) + "\n";
    }
    else
    {
      _strResult = "unknown command\n";
      _strResult += "version " + String(VersionString) + "\n";
    }
  }
  return "#RES_" + _strResult + "\nOK\n";
}

Task task_Cmd(
    100, TASK_FOREVER, []()
    {
    if (Serial.available() > 0)
    {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        String _strResult = processCommand(_strLine);
        Serial.print(_strResult);
    } });

void setup()
{

  pinMode(builtingLed, OUTPUT);
  digitalWrite(builtingLed, HIGH);

  // led pin setting
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // button pin setting
  for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++)
  {
    pinMode(buttonPins[i], INPUT);
  }

  // analog pin setting
  for (int i = 0; i < sizeof(analogPins) / sizeof(analogPins[0]); i++)
  {
    pinMode(analogPins[i], INPUT);
  }

  // battery pin setting
  pinMode(batteryPin, INPUT);

  String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
  chipId.toUpperCase(); // 대문자로 변환

  strBroadCastMsg = "#BC_BOLT-" + chipId;

  // AP 모드의 이름을 "BSQ-"와 칩셋 ID의 조합으로 설정
  // String ssid = "BSQ-" + chipId;
  // const char *ssid_ptr = ssid.c_str(); // const char* 타입으로 변환
  // pinMode(LED_PIN, OUTPUT);
  // digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200); // 시리얼 통신 초기화

  configData.load();

  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_STA);

  if (configData.mStrAp.length() > 0)
  {
    WiFi.begin(configData.mStrAp.c_str(), configData.mStrPassword.c_str());
  }
  else
  {
    WiFi.begin();
  }

  // UDP 시작 - 이제 connect 메서드는 사용하지 않습니다.
  if (udp.listen(localUdpPort))
  {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet)
                 {
                   // 여기서 수신된 패킷을 처리합니다.
                   Serial.print("UDP Packet Type: ");
                   Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast"
                                                                                          : "Unicast");
                   Serial.print(", From: ");
                   Serial.print(packet.remoteIP());
                   Serial.print(":");
                   Serial.print(packet.remotePort());
                   Serial.print(", To: ");
                   Serial.print(packet.localIP());
                   Serial.print(":");
                   Serial.print(packet.localPort());
                   Serial.print(", Length: ");
                   Serial.print(packet.length());
                   Serial.print(", Data: ");
                   Serial.write(packet.data(), packet.length());
                   Serial.println();

                   String _strRes = processCommand((const char *)packet.data());

                   // response to the client
                   packet.printf(_strRes.c_str()); });
  }

  Serial.println(":-]");
  Serial.println("Ready");

  scheduler.init();

  scheduler.addTask(task_Cmd);
  task_Cmd.enable();

  scheduler.addTask(task_udpBroadCast);
  task_udpBroadCast.enable();
}

void loop()
{

  scheduler.execute();
}
