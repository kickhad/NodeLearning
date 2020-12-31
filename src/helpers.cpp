#include "Arduino.h"
#include "helpers.h"
#include "stdint.h"
void serial_monitor(String str){
  #ifdef SERIAL_DEBUG
    Serial.println("failed to connect and hit timeout");
  #endif
}
void serial_monitor(uint32_t str){
  #ifdef SERIAL_DEBUG
  String out_str = String(str);
    Serial.println();
  #endif
}