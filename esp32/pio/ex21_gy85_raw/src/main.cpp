
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>
#include <TaskScheduler.h>

#include "I2Cdev.h"
#include "ADXL345.h"
#include "HMC5883L.h"
#include "ITG3200.h"

#include <tonkey.hpp>
#include "config.hpp"

#ifdef SEED_XIAO_ESP32C3
const int ledPins[] = {D10, D9};
const int analogPins[] = {D0, D1};
const int buttonPins[] = {D8, D2};

#elif defined(LOLIN_D32)

const int ledPins[] = {13,14};
const int buttonPins[] = {36, 39};
const int analogPins[] = {34, 35};

const int batteryPin = 33;
const int builtingLed = BUILTIN_LED;

#elif defined(LOLIN32_LITE)
const int ledPins[] = {13, 14};
const int buttonPins[] = {36, 39};
const int analogPins[] = {34, 35};
const int batteryPin = 33;
const int builtingLed = BUILTIN_LED;

#endif

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
AsyncUDP udp;

const char *VersionString = "1.0";

String processCommand(String _strLine)
{
  String _strResult = "";
  g_MainParser.parse(_strLine);

  if (g_MainParser.getTokenCount() > 0)
  {
    String cmd = g_MainParser.getToken(0);

    if (cmd == "about")
    {
      _strResult = "gy85_raw_v" + String(VersionString) + "\n";
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
      _strResult = "DNS: " + WiFi.dnsIP().toString() + "\n";
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


ADXL345 accel;
HMC5883L mag;
ITG3200 gyro;

int16_t ax, ay, az;
int16_t mx, my, mz;
int16_t gx, gy, gz;

void setup() {
  
  Serial.begin(115200);
  // while (!Serial); // ESP32에서 시리얼 연결을 기다립니다.

  Wire.begin();

  accel.initialize();
  mag.initialize();
  gyro.initialize();

  if (!accel.testConnection()) Serial.println("ADXL345 connection failed");
  else Serial.println("ADXL345 connection successful");
  
  if (!mag.testConnection()) Serial.println("HMC5883L connection failed");
  else Serial.println("HMC5883L connection successful");
  
  if (!gyro.testConnection()) Serial.println("ITG3200 connection failed");
  else Serial.println("ITG3200 connection successful");
}

void loop() {
  // 가속도계 데이터 읽기
  accel.getAcceleration(&ax, &ay, &az);
  
  // 자기장계 데이터 읽기
  mag.getHeading(&mx, &my, &mz);
  
  // 자이로스코프 데이터 읽기
  gyro.getRotation(&gx, &gy, &gz);
  

  Serial.printf("Accel: %d %d %d, Mag: %d %d %d, Gyro: %d %d %d\n", ax, ay, az, mx, my, mz, gx, gy, gz);

  // delay(1000);
}
