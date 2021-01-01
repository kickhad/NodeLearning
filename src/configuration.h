// SCREEN PARAMS
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#ifndef ESP8266
    #define ESP8266
#endif
//MQTT

#define MQTT_SERVER     "206.189.137.171"
#define MQTT_PORT   1883
#define MQTT_USER   "kitchenfarms"
#define MQTT_PASS   "kitchenfarms"

//DHT 
#define DHT_PIN        D7
#define DHT_TYPE    DHT11
#define LDR_PIN     A0
#define SENSOR_POWER    D6
#define SENSOR_PIN  D5

//MISC
#define DEVICE_ID   "AK-"
#define DEVICE_TYPE     "KORYSTICK"
#define BATTERY 50
#define TINT 2

//STRINGS
#define STR_ATTEMPT_MQTT "Attempting MQTT connection..."
#define STR_CONNECTED "Connected"
#define STR_CLIENT_ID "ESP8266Client-"
#define STR_ERR_CONNECT_TIMEOUT "failed to connect and hit timeout"
#define STR_INTERNET "Internet: "
#define STR_FAILED "Failed!"
