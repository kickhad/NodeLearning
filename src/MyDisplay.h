#ifndef MyDisplay_h
#define MyDisplay_h

#include "Arduino.h"
#include <Adafruit_SSD1306.h>

class MyDisplay : public Adafruit_SSD1306 {

// Adafruit_SSD1306 display();
public:
    MyDisplay(int width, int height, TwoWire* wire, int reset);
    // void Adafruit_SSD1306::setTextSize();
    // void Adafruit_SSD1306::setCursor();
    // void Adafruit_SSD1306::display();
    // void Adafruit_SSD1306::print();
    
    
    void update_display(String );
    void update_display(String txt, float t, String uom);
    void clear_display();
    void display_next();
    void update_display(String str_0, String str_1);
private:
  Adafruit_SSD1306 _display;
        // void Adafruit_SSD1306::clearDisplay();
    
};

#endif