#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "configuration.h"


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void update_display(String txt)
{
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(txt);
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.display();
}

void update_display(String txt, float t, String uom)
{
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(txt);
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.print(t);
  display.print(" ");
  display.setTextSize(3);
  display.print(uom);
  display.display();
}

void display_next(){
  delay (4000);
  display.clearDisplay();
  delay(2000);
  
}

void clear_display(){
  display.clearDisplay();
  display.display();
}
void update_display(String str_0, String str_1)
{
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(str_0);
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.print(str_1);
  display.print(" ");
  display.display();
}
void update_display(String txt, float t, String uom)
{
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(txt);
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.print(t);
  display.print(" ");
  display.setTextSize(3);
  display.print(uom);
  display.display();
}


