#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <ArduinoJson.h>
#include <EEPROM.h>


class Config {
public:
    static const size_t EEPROM_SIZE = 512;
    static const int EEPROM_START_ADDRESS = 0;

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
        if (buffer[0] == 0) {
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

    String dump() const {
        return jsonDoc;
    }

    void clear() {
        jsonDoc = "{}";
        save();
    }
};

#endif // CONFIG_HPP
