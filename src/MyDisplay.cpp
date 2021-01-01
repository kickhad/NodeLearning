#include "Arduino.h"
#include "MyDisplay.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>
// #include "configuration.h"


// MyDisplay::MyDisplay(int w, int h, TwoWire* twi, int reset_pin)
// { 
  
// }

void MyDisplay::update_display(String txt)
{
  this->setTextSize(2);
  this->setCursor(0, 0);
  this->print(txt);
  this->setTextSize(3);
  this->setCursor(0, 30);
  this->display();
}

void MyDisplay::update_display(String txt, float t, String uom)
{
  this->setTextSize(2);
  this->setCursor(0, 0);
  this->print(txt);
  this->setTextSize(3);
  this->setCursor(0, 30);
  this->print(t);
  this->print(" ");
  this->setTextSize(3);
  this->print(uom);
  this->display();
}

void MyDisplay::display_next(){
  delay (4000);
  this->clearDisplay();
  delay(2000);
  
}

void MyDisplay::clear_display(){
  this->clearDisplay();
  this->display();
}
void MyDisplay::update_display(String str_0, String str_1)
{
  this->setTextSize(2);
  this->setCursor(0, 0);
  this->print(str_0);
  this->setTextSize(3);
  this->setCursor(0, 30);
  this->print(str_1);
  this->print(" ");
  this->display();
}


