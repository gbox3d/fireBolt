#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif ESP32
#include <WiFi.h>
#include <vector>
#endif


#ifdef ESP8266
#include <ESPAsyncUDP.h>
#elif ESP32
#include <AsyncUDP.h>
#endif

#include <TaskScheduler.h>
#include <ArduinoJson.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


#ifdef ESP8266
#define PIN D5 // Pin where NeoPixels are connected to
#elif SEED_XIAO_ESP32C3
#define PIN        D1 // On Trinket or Gemma, suggest changing this to 1
#endif

// How many NeoPixels are attached to the Arduino?
// #define NUMPIXELS 8 // Popular NeoPixel ring size
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

int NUMPIXELS = 8;
Adafruit_NeoPixel *g_pPixels;


#include "tonkey.hpp"

#include "config.hpp"

Scheduler g_ts;
tonkey g_MainParser;
Config g_config;

Task task_neoDemo_1(150, TASK_FOREVER, []() {
    static int i = 0;
    static int dir = 1;

    static u_int8_t color[3] = {255, 0, 0};

    // Adafruit_NeoPixel &pixels = *g_pPixels;
    
    g_pPixels->clear();
    g_pPixels->setPixelColor(i, g_pPixels->Color(color[0], color[1], color[2]));
    g_pPixels->show();

    // pixels.clear();
    // pixels.setPixelColor(i, pixels.Color(color[0], color[1], color[2]));
    //pixels.show();

    i += dir;
    if(i >= NUMPIXELS) {
        i = NUMPIXELS - 1;
        dir = -1;
        color[0] = 0;
        color[1] = 0;
        color[2] = 255;
    }
    else if(i < 0) {
        i = 0;
        dir = 1;
        color[0] = 255;
        color[1] = 0;
        color[2] = 0;
    }
});




Task task_Cmd(100, TASK_FOREVER, []() {
    if (Serial.available() > 0)
    {
        // Adafruit_NeoPixel &pixels = *g_pPixels;
        String _strLine = Serial.readStringUntil('\n');
        _strLine.trim();
        Serial.println(_strLine);
        g_MainParser.parse(_strLine);

        if(g_MainParser.getTokenCount() > 0) {
          String cmd = g_MainParser.getToken(0);
            JsonDocument _res_doc;
            if (cmd == "about")
            {
                /* code */
                _res_doc["result"] = "ok";
                _res_doc["os"] = "cronos-v1";
                _res_doc["app"] = "example 09 ws2812";
                _res_doc["version"] = "1.0.0";
                _res_doc["author"] = "gbox3d";

                
            }
            // neo 0 255 0 0 (index, r, g, b)
            else if(cmd == "neo") {
                if(g_MainParser.getTokenCount() > 4) {
                    int index = g_MainParser.getToken(1).toInt();
                    int r = g_MainParser.getToken(2).toInt();
                    int g = g_MainParser.getToken(3).toInt();
                    int b = g_MainParser.getToken(4).toInt();

                    g_pPixels->setPixelColor(index, g_pPixels->Color(r, g, b));
                    // pixels.setPixelColor(index, pixels.Color(r, g, b));
                    g_pPixels->show();
                    // pixels.show();

                    _res_doc["result"] = "ok";
                    _res_doc["ms"] = "neo set";
                }
                else {
                    _res_doc["result"] = "fail";
                    _res_doc["ms"] = "need index, r, g, b";
                }
            }
            else if(cmd == "demo") {
                if(g_MainParser.getTokenCount() > 2) {
                    String subCmd = g_MainParser.getToken(1);
                    int demoIndex = g_MainParser.getToken(2).toInt();
                    if(demoIndex == 1) {

                        if(subCmd == "start") {
                            task_neoDemo_1.enable();
                            _res_doc["result"] = "ok";
                            _res_doc["ms"] = "demo start";
                        }
                        else {
                            task_neoDemo_1.disable();
                            _res_doc["result"] = "ok";
                            _res_doc["ms"] = "demo stop";
                        }
                    }
                    else {
                        _res_doc["result"] = "fail";
                        _res_doc["ms"] = "unknown demo index";
                    }
                }
                else {
                    _res_doc["result"] = "fail";
                    _res_doc["ms"] = "need sub command";
                }
                
            }
            
            else if(cmd == "neoclear") {
                // pixels.clear();
                // pixels.show();

                g_pPixels->clear();
                g_pPixels->show();

                _res_doc["result"] = "ok";
                _res_doc["ms"] = "neo clear";

            }
            else if(cmd == "reboot") {
                ESP.restart();
            }
            else if(cmd == "config") {
                if(g_MainParser.getTokenCount() > 1) {
                    String subCmd = g_MainParser.getToken(1);
                    if(subCmd == "load") {
                        g_config.load();
                        _res_doc["result"] = "ok";
                        _res_doc["ms"] = "config loaded";
                    }
                    else if(subCmd == "save") {
                        g_config.save();
                        _res_doc["result"] = "ok";
                        _res_doc["ms"] = "config saved";
                    }
                    else if(subCmd == "dump") {
                        
                        //parse json g_config.dump()
                        String jsonStr = g_config.dump();
                        DeserializationError error = deserializeJson(_res_doc["ms"], jsonStr);
                        if (error) {
                            // Serial.print(F("deserializeJson() failed: "));
                            // Serial.println(error.f_str());
                            // return;
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "json parse error";
                        }
                        else {
                            _res_doc["result"] = "ok";
                        }
                        
                    }
                    else if(subCmd == "clear") {
                        g_config.clear();
                        _res_doc["result"] = "ok";
                        _res_doc["ms"] = "config cleared";
                    }
                    else if(subCmd == "set") {
                        if(g_MainParser.getTokenCount() > 2) {
                            String key = g_MainParser.getToken(2);
                            String value = g_MainParser.getToken(3);
                            g_config.set(key.c_str(), value);
                            _res_doc["result"] = "ok";
                            _res_doc["ms"] = "config set";
                        }
                        else {
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "need key and value";
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
                                _res_doc["result"] = "fail";
                                _res_doc["ms"] = "json parse error";
                            }
                            else {
                                JsonArray array = tempDoc[key].as<JsonArray>();

                                g_config.set(key.c_str(), tempDoc);
                                _res_doc["result"] = "ok";
                                _res_doc["ms"] = tempDoc;
                            }
                            // g_config.set(key.c_str(), value);
                            // _res_doc["result"] = "ok";
                            // _res_doc["ms"] = "config set";
                        }
                        else {
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "need key and value";
                        }
                        
                    }
                    else if(subCmd == "get") {
                        if(g_MainParser.getTokenCount() > 2) {
                            String key = g_MainParser.getToken(2);

                            //check key exist
                            if(!g_config.hasKey(key.c_str())) {
                                _res_doc["result"] = "fail";
                                _res_doc["ms"] = "key not exist";
                                // serializeJson(_res_doc, Serial);
                                // Serial.println();
                                // return;
                            }
                            else {
                                _res_doc["result"] = "ok";
                                _res_doc["value"] = g_config.get<String>(key.c_str());
                            }
                        }
                        else {
                            _res_doc["result"] = "fail";
                            _res_doc["ms"] = "need key";
                        }
                    }
                    else {
                        _res_doc["result"] = "fail";
                        _res_doc["ms"] = "unknown sub command";
                    
                    }
                }
                else {
                    _res_doc["result"] = "fail";
                    _res_doc["ms"] = "need sub command";
                }
            }
            else {
              _res_doc["result"] = "fail";
              _res_doc["ms"] = "unknown command";
            }
            serializeJson(_res_doc, Serial);
            Serial.println();
            
        }
    }
}, &g_ts, true);


// the setup function runs once when you press reset or power the board
void setup()
{
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

    g_config.load();

    if(g_config.hasKey("numpixels")) {
        NUMPIXELS = g_config.get<int>("numpixels");
    }

    g_pPixels = new Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

    // pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


    g_pPixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    g_pPixels->clear(); // Set all pixel colors to 'off'

    // pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    // pixels.clear(); // Set all pixel colors to 'off'

    Serial.begin(115200);
    

    Serial.println(":-]");
    Serial.println("Serial connected");
    Serial.println("num pixels : " + String(NUMPIXELS));
#ifdef ESP8266
    Serial.println("ESP8266");
#endif


    g_ts.addTask(task_neoDemo_1);
    task_neoDemo_1.enable();
    
    g_ts.startNow();

} 

// the loop function runs over and over again forever
void loop()
{
    g_ts.execute();
}


/*
#include <Arduino.h> // Arduino 헤더 파일 포함
#include <TaskScheduler.h>
#include <WiFi.h>
#include <AsyncUDP.h>

// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        D1 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  Serial.begin(115200); // Initialize serial communication

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop

    // Serial.println(i);
  }
}

*/