#ifndef MyDisplay_h
#define MyDisplay_h

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "configuration.h"

// Adafruit_SSD1306 display();
class MyDisplay : public Adafruit_SSD1306 {
  public:
    MyDisplay(int, int, TwoWire*, int): Adafruit_SSD1306(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, OLED_RESET){};
    // Update display with (String) or (String, float, String) or (String, String)
    void update_display(String txt);
    void update_display(String txt, float t, String uom);
    void update_display(String str_0, String str_1);
    
    // "unsafe" clear display. No wait/yield display may need
    void clear_display();
    // display clean with builtin waits.
    void display_next();
private:
    int _int;
};
#endif