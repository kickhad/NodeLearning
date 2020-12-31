#ifndef MyDisplay_h
#define MyDisplay_h

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "configuration.h"

// Adafruit_SSD1306 display();
class MyDisplay : public Adafruit_SSD1306 {
  public:
    MyDisplay(int, int, TwoWire*, int): Adafruit_SSD1306(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire, OLED_RESET){};
    void update_display(String txt);
    void update_display(String txt, float t, String uom);
    void clear_display();
    void display_next();
    void update_display(String str_0, String str_1);
private:
    int _int;
};
#endif