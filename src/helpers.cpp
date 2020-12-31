#include <ESP8266AVRISP.h>
#include <WiFi.h>

void serial_monitor(String str){
  #ifdef SERIAL_DEBUG
    Serial.println("failed to connect and hit timeout");
  #endif
}
void serial_monitor(u32_t str){
  #ifdef SERIAL_DEBUG
  String out_str = String(str);
    Serial.println();
  #endif
}