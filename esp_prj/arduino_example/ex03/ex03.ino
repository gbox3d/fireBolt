#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <WiFiUdp.h>

#include <vector>
#include <sstream>

using namespace std;

void listDir(const char *dirname)
{
    Serial.printf("Listing directory: %s\r\n", dirname);
    Dir dir = SPIFFS.openDir(dirname);
    while (dir.next())
    {
        Serial.print("File Name: ");
        Serial.print(dir.fileName());
        if (dir.fileSize())
        {
            File f = dir.openFile("r");
            Serial.print(", Size: ");
            Serial.println(f.size());
        }
    }
}

void readFile(const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);
    File file = SPIFFS.open(path, "r");
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return;
    }
    Serial.println("read from file:");
    while (file.available())
    {
        Serial.write(file.read());
    }
}

void writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);
    File file = SPIFFS.open(path, "w");
    if (!file)
    {
        Serial.println("failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("file written");
    }
    else
    {
        Serial.println("frite failed");
    }
}

void appendFile(const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\r\n", path);
    File file = SPIFFS.open(path, "a");
    if (!file)
    {
        Serial.println("failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("message appended");
    }
    else
    {
        Serial.println("append failed");
    }
}

void renameFile(const char *path1, const char *path2)
{
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (SPIFFS.rename(path1, path2))
    {
        Serial.println("file renamed");
    }
    else
    {
        Serial.println("rename failed");
    }
}

void deleteFile(const char *path)
{
    Serial.printf("Deleting file: %s\r\n", path);
    if (SPIFFS.remove(path))
    {
        Serial.println("file deleted");
    }
    else
    {
        Serial.println("delete failed");
    }
}


// the setup function runs once when you press reset or power the board
void setup()
{
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);

    if (!SPIFFS.begin())
    {
        Serial.println("Failed to mount file system");
        return;
    }
    delay(1000);
    Serial.println("\r\n\r\nFile system mounted");

    FSInfo fsInfo;
    SPIFFS.info(fsInfo);
    Serial.print("totalBytes: ");
    Serial.println(fsInfo.totalBytes);
    Serial.print("usedBytes: ");
    Serial.println(fsInfo.usedBytes);
    listDir("/");
    String temp = "test, 53.3, teH_Sst\"2, 5.2, 85"; // 저장될 문자열 "test, 53.3, teH_Sst"2, 5.2, 85"
}

// the loop function runs over and over again forever
void loop()
{
    
    while (Serial.available() > 0)
    {
        string _strLine = Serial.readStringUntil('\n').c_str();
        string delim = " ";
        vector<string> words{};

        //tokenizing
        size_t pos = 0;
        int nCount = 0;
        while ((pos = _strLine.find(delim)) != string::npos)
        {
            string token = _strLine.substr(0, pos);
            words.push_back(token);
            _strLine.erase(0, pos + delim.length());

            //토킁을 2개까지만 만들기 
            nCount++;
            if(nCount >= 2)
                break;
        }
        words.push_back(_strLine);

        if(words[0]=="ls") {
            listDir("/");
        }
        else if(words[0]=="read") {
            // Serial.printf("%s %s\r\n",words[0].c_str(), words[1].c_str());
            readFile(words[1].c_str());
        }
        else if(words[0]=="write") {
            writeFile(words[1].c_str(), words[2].c_str());
        }
        else if(words[0]=="append") {
            appendFile(words[1].c_str(), words[2].c_str());
        }
        else if(words[0]=="rename") {
            renameFile(words[1].c_str(), words[2].c_str());
        }
        else if(words[0]=="delete") {
            deleteFile(words[1].c_str());
        }
        else if(words[0]=="format") {
            Serial.println("formatting...");
            SPIFFS.format();
            Serial.println("format done");

        }
        else if(words[0]=="help") {
            Serial.println("ls, read, write, append, rename, delete, test, help");
        }
        else {
            Serial.println("Unknown command");
        }
    }
}