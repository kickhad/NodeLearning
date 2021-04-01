#ifndef KFNETMAN_H
#define KFNETMAN_H

#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())


#define LED_ON HIGH
#define LED_OFF LOW

#include "FS.h"
#include "WiFiMulti.h"
#include "WiFiClient.h"
class KfConfig
{
public:
    void saveConfigData(void);
    void loadConfigData(void);
    bool readConfigFile();
    bool writeConfigFile();
    void newConfigData();

private:
    int _id;
};

class KfClient
{
public:
    void publishMQTT();
    void toggleLED();
    void heartBeatPrint(void);
    void check_WiFi(void);
    void check_status(void);
    void deleteOldInstances(void);
    void createNewInstances(void);
    void wifi_manager();
    uint8_t connectMultiWiFi(void);
    void MQTT_connect();

    //    void publishMQTT(void);
     KfClient(void);

private:
    int _id;
    WiFiClient *client;
    KfConfig _config;
};

#endif



// // Loop function
// void loop()
// {
//     // Call the double reset detector loop method every so often,
//     // so that it can recognise when the timeout expires.
//     // You can also call drd.stop() when you wish to no longer
//     // consider the next reset as a double reset.
//     if (drd)
//         drd->loop();

//     // this is just for checking if we are connected to WiFi
//     check_status();
// }