/*
 *  This sketch shows the WiFi event usage
 *
 *  In this example you can receive and process below events.
 *  Refer to ESP8266WiFiType.h
 *  
 typedef enum {
    WIFI_EVENT_STAMODE_CONNECTED = 0,
    WIFI_EVENT_STAMODE_DISCONNECTED,
    WIFI_EVENT_STAMODE_AUTHMODE_CHANGE,
    WIFI_EVENT_STAMODE_GOT_IP,
    WIFI_EVENT_STAMODE_DHCP_TIMEOUT,
    WIFI_EVENT_SOFTAPMODE_STACONNECTED,
    WIFI_EVENT_SOFTAPMODE_STADISCONNECTED,
    WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED,
    WIFI_EVENT_MAX
} WiFiEvent_t;
 * 
 * hardcopyworld.com
 */

#include <ESP8266WiFi.h>

const char *ssid = "your_ssid";
const char *password = "your_password";

void WiFiEvent(WiFiEvent_t event) 
{
    switch(event) 
    {
        case WIFI_EVENT_STAMODE_CONNECTED:
            Serial.println("WiFi connected");
            
            break;
        case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case WIFI_EVENT_STAMODE_GOT_IP:
            Serial.println("Get IP address");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            digitalWrite(LED_BUILTIN, LOW);
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            Serial.println("WiFi lost connection");
            digitalWrite(LED_BUILTIN, HIGH);
            break;
        case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
            Serial.println("DHCP timeout");
            break;
        case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
            Serial.println("Station connected to soft-AP");
            break;
        case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
            Serial.println("Station disconnected from soft-AP");
            break;
        case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
            Serial.println("Probe request received");
            break;
        default:
            Serial.printf("[WiFi-event] event: %d\n", event);
            break;
    }
}

void setup() 
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(LED_BUILTIN, HIGH);
    
    // delete old config
    WiFi.disconnect(true);

    delay(1000);

    WiFi.onEvent(WiFiEvent);

    WiFi.begin(ssid, password);

    Serial.println();
    Serial.println();
    Serial.println("Wait for WiFi... ");
}


void loop() 
{
    delay(1000);
}
