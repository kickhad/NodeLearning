
// #ifndef DEBUG
#define MQTT_BUFFER_LENGTH 512
#define MQTT_SERVER "206.189.137.171"
#define MQTT_PORT 1883
#define MQTT_USER "kitchenfarms"
#define MQTT_PASS "kitchenfarms"
// #else
// #define MQTT_SERVER "192.168.0.11"
// #define MQTT_PORT 1883
// #define MQTT_USER "test"
// #define MQTT_PASS "testtest"
// #endif
//DHT
#define DHT_PIN 27
#define DHT_TYPE DHT11
#define LDR_PIN 2
#define SENSOR_POWER 6
#define SENSOR_PIN 2

//MISC
#define DEVICE_ID "AK-"
#define DEVICE_TYPE "KORYSTICK"
#define BATTERY 50
#define TINT_LEVEL 2

//STRINGS
#define STR_ATTEMPT_MQTT "Attempting MQTT connection..."
#define STR_CONNECTED "Connected"
#define STR_CLIENT_ID "ESP8266Client-"
#define STR_ERR_CONNECT_TIMEOUT "failed to connect and hit timeout"
#define STR_INTERNET "Internet: "
#define STR_FAILED "Failed!"


#define TLS2591_INT_THRESHOLD_LOWER (100)
#define TLS2591_INT_THRESHOLD_UPPER (1500)
//#define TLS2591_INT_PERSIST        (TSL2591_PERSIST_ANY) // Fire on any valid change
#define TLS2591_INT_PERSIST (TSL2591_PERSIST_60) // Require at least 60 samples to fire



#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32