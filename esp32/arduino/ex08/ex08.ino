#include "SPIFFS.h"

// spiffs example 


void setup()
{
    Serial.begin(115200);
    Serial.println("check spiffs");
    
    // 최초로 플레쉬에 접근할경우 포맷을 해야함
    // E (52) SPIFFS: mount failed, -10025 메시지가 나오고 좀 기다리면 포멧이 완료됨
    if( !SPIFFS.begin(true))  //true 는 포멧을 자동으로 수행하도록 함
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    Serial.println("SPIFFS Mount Successful");

    File file = SPIFFS.open("/test.txt", FILE_WRITE);
    if (!file) {
        Serial.println("There was an error opening the file for writing");
        return;
    }

    if (file.print("Hello World")) {
        Serial.println("File was written");
    } else {
        Serial.println("File write failed");
    }

    file.close();

    File file2 = SPIFFS.open("/test.txt");

    if (!file2) {
        Serial.println("There was an error opening the file for reading");
        return;
    }

    Serial.println("File Content:");
    while (file2.available()) {
        Serial.write(file2.read());
    }
    file2.close();

    Serial.println("Done!");
}

void loop()
{
}