#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <ArduinoJson.h>
#include <EEPROM.h>

#ifdef ESP32
#include <nvs_flash.h>
#endif

class Config
{
public:
    const static int SystemVersion = 3;

#ifdef ESP8266
    static const size_t EEPROM_SIZE = 1024;
    static const int EEPROM_START_ADDRESS = 0;
#elif ESP32
    static const size_t EEPROM_SIZE = 1024;
    static const int EEPROM_START_ADDRESS = 0;
#else
    static const size_t EEPROM_SIZE = 512;
    static const int EEPROM_START_ADDRESS = 0;
#endif
    String jsonDoc;

    Config()
    {

#ifdef ESP32
        // NVS 초기화
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);
#endif

#ifdef AVR
#else
        EEPROM.begin(EEPROM_SIZE);
#endif

        load();
    }

    void load()
    {
        char buffer[EEPROM_SIZE];
        for (size_t i = 0; i < EEPROM_SIZE; ++i)
        {
            buffer[i] = EEPROM.read(i);
#ifdef DEBUG
            Serial.print(buffer[i]); // debug
#endif
        }

#ifdef DEBUG
        Serial.println("data length: " + String(strlen(buffer))); // debug
        Serial.println("data: " + String(buffer));                // debug
#endif
        // if empty, set default
        if (buffer[0] != '{' && buffer[0] != '[')
        {
            jsonDoc = "{}";
        }
        else
        {
            jsonDoc = String(buffer);
        }
    }

    void save()
    {
        for (size_t i = 0; i < EEPROM_SIZE; ++i)
        {
            if (i < jsonDoc.length())
            {
                EEPROM.write(i, jsonDoc[i]);
            }
            else
            {
                EEPROM.write(i, 0);
            }
        }

#ifdef DEBUG
        Serial.println("data length: " + String(jsonDoc.length())); // debug
        Serial.println("data: " + jsonDoc);                         // debug
#endif

#ifdef AVR

#else

        EEPROM.commit();
#endif
    }

    // Generic set and get
    template <typename T>
    void set(const char *key, T value)
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonDoc);
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        doc[key] = value;
        serializeJson(doc, jsonDoc);
        save();
    }

    template <typename T>
    T get(const char *key, T defaultValue = T()) const
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonDoc);
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return defaultValue;
        }

        // Use the new API to check if the key exists and has the correct type
        if (!doc[key].is<T>())
        {
            return defaultValue;
        }

        return doc[key].as<T>();
    }

    // check key exist
    bool hasKey(const char *key) const
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonDoc);
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        // return doc.containsKey(key);
        return doc[key].is<JsonVariant>(); // 원하는 타입으로 변경 가능 (예: is<int>(), is<char*>(), is<String>(), 등)
    }

    void getArray(const char *key, JsonDocument &_doc) const
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonDoc);

        Serial.println(doc[key].as<String>());

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            // return JsonArray();
        }

        JsonDocument tempDoc;

        error = deserializeJson(_doc, doc[key].as<String>());
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            // return JsonArray();
        }

        Serial.println(_doc.as<JsonArray>().size());
        // array = tempDoc.as<JsonArray>();
        // return tempDoc.as<JsonArray>();
    }

    String dump() const
    {
        return jsonDoc;
    }

    void clear()
    {
        jsonDoc = "{}";
        save();
    }
};

#endif // CONFIG_HPP
