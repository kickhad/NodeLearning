#ifndef KFNETMAN_H
#define KFNETMAN_H
#include "FS.h"
class netman
{
public:
    netman(FS filesystem)
    {
        FS _filesystem = filesystem;
    }
    WiFiMulti wifiMulti;
    uint8_t connectMultiWiFi();
    

private:
};

#endif