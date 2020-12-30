#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "configuration.h"

// Adafruit_SSD1306 display();
void update_display(String txt);
void update_display(String txt, float t, String uom);
void clean_display();
void display_next();
void update_display(String str_0, String str_1);