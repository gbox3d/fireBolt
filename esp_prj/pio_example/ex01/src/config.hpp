#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <ArduinoJson.h>
#include <EEPROM.h>


class Config {
public:
#ifdef ESP8266
    static const size_t EEPROM_SIZE = 1024;
    static const int EEPROM_START_ADDRESS = 0;
#else
    static const size_t EEPROM_SIZE = 512;
    static const int EEPROM_START_ADDRESS = 0;
#endif

    String jsonDoc;

    Config() {
        // jsonDoc = "{}";
        EEPROM.begin(EEPROM_SIZE);
        load();
    }

    void load() {
        char buffer[EEPROM_SIZE];
        for (size_t i = 0; i < EEPROM_SIZE; ++i) {
            buffer[i] = EEPROM.read(i);
        }


        //if empty, set default
        if (buffer[0] != '{' && buffer[0] != '[') {
            jsonDoc = "{}";
        } else {
            jsonDoc = String(buffer);
        }
    }

    void save() {
        for (size_t i = 0; i < EEPROM_SIZE; ++i) {
            if (i < jsonDoc.length()) {
                EEPROM.write(i, jsonDoc[i]);
            } else {
                EEPROM.write(i, 0);
            }
        }
        EEPROM.commit();
    }

    // Generic set and get
    template <typename T> void set(const char* key, T value) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonDoc);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        doc[key] = value;
        serializeJson(doc, jsonDoc);
        save();
    }

    template <typename T> T get(const char* key) const {

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonDoc);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return T();
        }

        return doc[key].as<T>();
    }

    void getArray(const char* key, JsonDocument &_doc) const {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, jsonDoc);

        Serial.println(doc[key].as<String>());


        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            // return JsonArray();
        }

        JsonDocument tempDoc;

        error = deserializeJson(_doc, doc[key].as<String>());
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            // return JsonArray();
        }

        Serial.println(_doc.as<JsonArray>().size());
        // array = tempDoc.as<JsonArray>();
        // return tempDoc.as<JsonArray>();
    }



    String dump() const {
        return jsonDoc;
    }



    void clear() {
        jsonDoc = "{}";
        save();
    }


    
};

#endif // CONFIG_HPP
