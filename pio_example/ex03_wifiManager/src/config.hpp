#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Preferences.h>

class CCongifData
{
    Preferences preferences;

public:

    int mDeviceNumber;
    String mTargetIp;
    int mTargetPort;
    int16_t mOffsets[6];

    bool save()
    {
        preferences.begin("config", false);
        preferences.putInt("mDeviceNumber", mDeviceNumber);
        preferences.putString("mTargetIp", mTargetIp);
        preferences.putInt("mTargetPort", mTargetPort);

        for (int i = 0; i < 6; i++)
        {
            preferences.putInt((String("offset") + i).c_str(), mOffsets[i]);
        }

        preferences.end();
        return true;
    }

    bool load()
    {
        preferences.begin("config", true);

        mTargetIp = preferences.getString("mTargetIp", "");
        mTargetPort = preferences.getInt("mTargetPort", 0);
        mDeviceNumber = preferences.getInt("mDeviceNumber", 0);

        for (int i = 0; i < 6; i++)
        {
            mOffsets[i] = preferences.getInt((String("offset") + i).c_str(), 0);
        }

        preferences.end();
        return true;
    }

    bool clear()
    {
        preferences.begin("config", false);
        preferences.clear();
        preferences.end();
        return true;
    }
    void print()
    {
        Serial.println("mTargetIp: " + mTargetIp);
        Serial.println("mTargetPort: " + String(mTargetPort));
        Serial.println("mDeviceNumber: " + String(mDeviceNumber));
        Serial.println("mOffsets: ");
        for (int i = 0; i < 6; i++)
        {
            Serial.println((String("offset") + i + ": ").c_str() + String(mOffsets[i]));
        }
    }
};

#endif // CONFIG_HPP