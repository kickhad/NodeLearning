#include "kfnetman.h"

#define ESP_ASYNC_WIFIMANAGER_VERSION_MIN_TARGET "ESPAsync_WiFiManager v1.6.1"
#define _ESPASYNC_WIFIMGR_LOGLEVEL_ 4
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "WiFiMulti.h"
WiFiMulti wifiMulti;
#define USE_LITTLEFS true