#include <Arduino.h> // Arduino 헤더 파일 포함
#include <TaskScheduler.h>
#include <tonkey.hpp>

void update_asukiaaa();
void init_asukiaaa();
void scan_mpu();

namespace hideaki
{
    void init();
    void update();
    void print();
    
    
}

namespace asukiaaa
{
    void init();
    void update();
    void print();
    Task task_print(
        100, TASK_FOREVER, []()
        {
            print();
        });
}

Scheduler runner;
tonkey g_MainParser;

// CCongifData configData;

// update 함수 포인터
void (*updateFunc)() = nullptr;
//print 함수 포인터
void (*printFunc)() = nullptr;


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
            // DynamicJsonDocument _res_doc(1024);

          String _result = "ok";

          if (cmd == "about")
          {
            _result = "mp9250 test";
                
          }
          else if(cmd == "reboot") {
            ESP.restart();
          }
          else if(cmd == "asuki") {

            asukiaaa::init();
            updateFunc = asukiaaa::update;
            printFunc = asukiaaa::print;
            
            // runner.addTask(asukiaaa::task_print);
            // asukiaaa::task_print.enable();
          }
          // else if(cmd == "stop sensor") {
          //   task_Sensor.disable();
          // }
          else if(cmd == "scan") {
            scan_mpu();
          } 
          else if (cmd == "hideaki") {
            hideaki::init();
            updateFunc = hideaki::update;
            printFunc = hideaki::print;
            
          }
          else
          {
            _result = "unknown command";
          }
          Serial.println(_result);
        }
    } });


Task task_print(
  100, TASK_FOREVER, []()
  {
    if(printFunc != nullptr) {
      printFunc();
    } 
  });


void setup()
{
  Serial.begin(115200);
  // while(!Serial);
  Serial.println("started");
  runner.init();
  runner.addTask(task_Cmd);
  runner.addTask(task_print);
  // runner.addTask(task_Sensor);
  // runner.addTask(task_Hideaki);
  task_Cmd.enable();
  task_print.enable();
}

void loop()
{
  if(updateFunc != nullptr) {
    updateFunc();
  }

  runner.execute();
}