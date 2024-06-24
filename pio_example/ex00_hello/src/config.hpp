#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Preferences.h>

class CCongifData
{
    Preferences preferences;

public:

    int mNumber = 0;
    String mMsg;

    bool save()
    {
        preferences.begin("config", false);
        
        preferences.putInt("mNumber", mNumber);
        preferences.putString("mMsg", mMsg);
        
        preferences.end();
        return true;
    }

    bool load()
    {
        preferences.begin("config", true);

        mNumber = preferences.getInt("mNumber", 0);
        mMsg = preferences.getString("mMsg", "hello");
        
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
        Serial.printf("mNumber:%d, mMsg:%s\n", mNumber, mMsg.c_str());
    }
};

#endif // CONFIG_HPP