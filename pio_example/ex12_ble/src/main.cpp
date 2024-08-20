#include <Arduino.h>

#include <esp_system.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <WiFi.h>
#include <vector>

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include "tonkey.hpp"

#include "config.hpp"
#include "etc.hpp"

Scheduler g_ts;
tonkey g_MainParser;

Config g_config;

#ifdef SEED_XIAO_ESP32C3
#define LED_BUILTIN D10
#else
#define LED_BUILTIN 4
#endif

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;


// UUID for service and characteristic
                      
#define SERVICE_UUID "457556f0-842a-41ac-b777-cb4af6f47720"
#define CHARACTERISTIC_UUID "3c3ffbea-1856-460a-b0dd-b8a2a3a8352b"


// Task taskNotify(1000, TASK_FOREVER, []()
//               {
//     if (deviceConnected)
//     {
//         static int counter = 0;
//         String message = "Notify message #" + String(counter++);
//         pCharacteristic->setValue(message.c_str());
//         pCharacteristic->notify();
//         Serial.println("Notify: " + message);
//     }
// }, &g_ts, false);

// 전역 변수로 선언
JsonDocument g_res_doc;

String ParseCmd(String _strLine) {
    g_MainParser.parse(_strLine);
    g_res_doc.clear();  // 응답 문서 초기화

    g_MainParser.parse(_strLine);

    if(g_MainParser.getTokenCount() > 0) {
        String cmd = g_MainParser.getToken(0);
        if (cmd == "about")
        {
            /* code */
            g_res_doc["result"] = "ok";
            g_res_doc["os"] = "cronos-v1";
            g_res_doc["app"] = "ex12-ble";
            g_res_doc["version"] = "1.0.0";
            g_res_doc["author"] = "gbox3d";
            g_res_doc["chipid"] = ESP.getEfuseMac();
            
        }
        else if(cmd == "reboot") {
            ESP.restart();
        }
        else if(cmd == "config") {
            if(g_MainParser.getTokenCount() > 1) {
                String subCmd = g_MainParser.getToken(1);
                if(subCmd == "load") {
                    g_config.load();
                    g_res_doc["result"] = "ok";
                    g_res_doc["ms"] = "config loaded";
                }
                else if(subCmd == "save") {
                    g_config.save();
                    g_res_doc["result"] = "ok";
                    g_res_doc["ms"] = "config saved";
                }
                else if(subCmd == "dump") {
                    
                    //parse json g_config.dump()
                    String jsonStr = g_config.dump();
                    DeserializationError error = deserializeJson(g_res_doc["ms"], jsonStr);
                    if (error) {
                        // Serial.print(F("deserializeJson() failed: "));
                        // Serial.println(error.f_str());
                        // return;
                        g_res_doc["result"] = "fail";
                        g_res_doc["ms"] = "json parse error";
                    }
                    else {
                        g_res_doc["result"] = "ok";
                    }
                    
                }
                else if(subCmd == "clear") {
                    g_config.clear();
                    g_res_doc["result"] = "ok";
                    g_res_doc["ms"] = "config cleared";
                }
                else if(subCmd == "set") {
                    if(g_MainParser.getTokenCount() > 2) {
                        String key = g_MainParser.getToken(2);
                        String value = g_MainParser.getToken(3);
                        g_config.set(key.c_str(), value);
                        g_res_doc["result"] = "ok";
                        g_res_doc["ms"] = "config set";
                    }
                    else {
                        g_res_doc["result"] = "fail";
                        g_res_doc["ms"] = "need key and value";
                    }
                }
                else if(subCmd == "setA") { //set json array
                    if(g_MainParser.getTokenCount() > 2) {
                        String key = g_MainParser.getToken(2);
                        String value = g_MainParser.getToken(3);
                        //parse json value
                        // JSON 문자열 파싱을 위한 임시 객체
                        JsonDocument tempDoc; // 임시 JSON 문서

                        // JSON 문자열 파싱
                        DeserializationError error = deserializeJson(tempDoc, value);
                        // DeserializationError error = deserializeJson(g_config[key.c_str()], value);
                        if (error) {
                            // Serial.print(F("deserializeJson() failed: "));
                            // Serial.println(error.f_str());
                            // return;
                            g_res_doc["result"] = "fail";
                            g_res_doc["ms"] = "json parse error";
                        }
                        else {
                            // JsonArray array = tempDoc[key].as<JsonArray>();

                            g_config.set(key.c_str(), tempDoc);
                            g_res_doc["result"] = "ok";
                            g_res_doc["ms"] = tempDoc;
                        }
                        // g_config.set(key.c_str(), value);
                        // _res_doc["result"] = "ok";
                        // _res_doc["ms"] = "config set";
                    }
                    else {
                        g_res_doc["result"] = "fail";
                        g_res_doc["ms"] = "need key and value";
                    }
                    
                }
                else if(subCmd == "get") {
                    if(g_MainParser.getTokenCount() > 2) {
                        String key = g_MainParser.getToken(2);

                        //check key exist
                        if(!g_config.hasKey(key.c_str())) {
                            g_res_doc["result"] = "fail";
                            g_res_doc["ms"] = "key not exist";
                            // serializeJson(_res_doc, Serial);
                            // Serial.println();
                            // return;
                        }
                        else {
                            g_res_doc["result"] = "ok";
                            g_res_doc["value"] = g_config.get<String>(key.c_str());
                        }
                    }
                    else {
                        g_res_doc["result"] = "fail";
                        g_res_doc["ms"] = "need key";
                    }
                }
                else {
                    g_res_doc["result"] = "fail";
                    g_res_doc["ms"] = "unknown sub command";
                
                }
            }
            else {
                g_res_doc["result"] = "fail";
                g_res_doc["ms"] = "need sub command";
            }
        }
        else if(cmd == "ble") {

            // BLE
            if(g_MainParser.getTokenCount() > 1) {
                String subCmd = g_MainParser.getToken(1);
                if(subCmd == "info") {
                    g_res_doc["result"] = "ok";
                    g_res_doc["ms"] = "ble info";
                    g_res_doc["deviceConnected"] = deviceConnected;
                    g_res_doc["serviceUUID"] = SERVICE_UUID;
                    g_res_doc["characteristicUUID"] = CHARACTERISTIC_UUID;

                    if (BLEDevice::getInitialized()) {
                        // return "BLE not initialized";
                        BLEAddress bleAddress = BLEDevice::getAddress();
                        String addressStr = bleAddress.toString().c_str();
                        
                        g_res_doc["address"] = addressStr;
                        
                            // ESP32의 MAC 주소 가져오기
                        uint8_t macAddr[6];
                        esp_efuse_mac_get_default(macAddr);
                        char macStr[18];
                        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                                macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);

                        if (addressStr == String(macStr)) {
                            g_res_doc["addressType"] = "Public";
                        } else {
                            g_res_doc["addressType"] = "Random";
                        }
                    }
                    else {
                        g_res_doc["address"] = "ble not initialized";
                    }

                    
                    // // BLE 스택 상태 추가
                    g_res_doc["bleStackStarted"] = btStarted() ? "Yes" : "No";
                }
            }
            else {
                g_res_doc["result"] = "fail";
                g_res_doc["ms"] = "need sub command";
            }
            
        }
        else {
            g_res_doc["result"] = "fail";
            g_res_doc["ms"] = "unknown command";
        }
        // serializeJson(g_res_doc, Serial);
        // Serial.println();
        
    }
    else {
        g_res_doc["result"] = "fail";
        g_res_doc["ms"] = "need command";
    }

    String response;
    serializeJson(g_res_doc, response);
    return response;
}



Task task_Cmd(100, TASK_FOREVER, []() {

    if (Serial.available() > 0) {
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println("Received Serial command:");
        Serial.println(_strLine);

        String response = ParseCmd(_strLine);
        Serial.println("Response:");
        Serial.println(response);
    }

}, &g_ts, true);

Task task_LedBlink(500, TASK_FOREVER, []()
              {
                  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
              }, &g_ts, true);


void printMtuSize(BLEServer *pServer) {
    uint16_t currentMtu = pServer->getPeerMTU(pServer->getConnId());
    Serial.print("current MTU size: ");
    Serial.println(currentMtu);
}

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {

        task_LedBlink.disable();
        deviceConnected = true;

        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("client connected");
        pServer->getAdvertising()->stop(); // 클라이언트 연결 시 광고 중지

        // // 환영 메시지 설정 및 알림 전송
        pCharacteristic->setValue("welcome to ESP32 BLE Server");

        printMtuSize(pServer);
    }

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("client disconnected");
        pServer->getAdvertising()->start(); // 클라이언트 연결 해제 시 광고 재시작

        task_LedBlink.enable();
        // taskNotify.disable();
    }

    void onMtuChanged(BLEServer *pServer, uint16_t mtu) {
        Serial.print("MTU size changed to: ");
        Serial.println(mtu);
    }
};

class MyCharateristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            Serial.println("Received BLE command:");
            Serial.println(value.c_str());

            String response = ParseCmd(String(value.c_str()));
            Serial.println("Response:");
            Serial.println(response);

            printMtuSize(pServer);

            // BLE를 통해 응답 전송
            pCharacteristic->setValue(response.c_str());
            pCharacteristic->notify();
        }
    }

    void onRead(BLECharacteristic *pCharacteristic) {
        Serial.println("BLE read : ");
        
        std::string value = pCharacteristic->getValue();
        Serial.println(value.c_str());
        
        printMtuSize(pServer);
    }
};



// the setup function runs once when you press reset or power the board
void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED off by making the voltage LOW

    Serial.begin(115200);

    g_config.load();

    Serial.println(":-]");
    Serial.println("Serial connected");
    
    g_ts.startNow();

    // BLE 장치 생성
    BLEDevice::init("ESP32_BLE" + std::string(getChipID().c_str()));
    
    // BLE 서버 생성
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // // MTU 크기 설정
    // Serial.println("Setting MTU size to 512");
    //BLEDevice::setMTU(512);
    

    // BLE 서비스 생성
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // BLE 특성 생성
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | // 읽기 속성
        BLECharacteristic::PROPERTY_WRITE | // 쓰기 속성
        BLECharacteristic::PROPERTY_NOTIFY | // 알림 속성
        BLECharacteristic::PROPERTY_INDICATE // 표시 속성
    );

    // 디스크립터 추가
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new MyCharateristicCallbacks());

    // 서비스 시작
    pService->start();

    // 광고 시작
    pServer->getAdvertising()->start();

    Serial.println("BLE Ready....");
}

// the loop function runs over and over again forever
void loop()
{
  g_ts.execute();
}