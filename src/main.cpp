#include <Arduino.h>
#include <ArduinoJson.h>
#include "kfcore/core.h"
#include "ArduinoJson.h"
#include "MyDisplay.h"
#include "Adafruit_seesaw.h"
#include <Wire.h>
#include "Wire.h"
#include <SSD1306Wire.h>
#include "configuration.h"
#include <Adafruit_AHTX0.h>
#include "logger.h"
#include "secrets.h"
// #include "Fonts/FreeMono12pt7b.h"
const char* ssid = SSID
const char* password = PASSWORD

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

SSD1306Wire display(0x3c, SDA, SCL);
Adafruit_AHTX0 aht;
Adafruit_seesaw ss;
unsigned long tik = 0;
struct SensorData
{
  float temp;
  float humidity;
  uint16_t soil_cap;
  double light;
  float soil_tempC;
};

SensorData data;

// DHTesp dht;
// KfClient client;
// MyDisplay display;
void setup()
{

  Serial.begin(115200);
  if (!ss.begin(0x36))
  {
    logPrintlnA("ERROR! seesaw not found");
    ;
  }
  else
  {
    PrintlnA("seesaw started! version: ");
    PrintlnA(String(ss.getVersion()));
  }
  if (!aht.begin())
  {
    logPrintlnA("Could not find AHT? Check wiring");
    while (1)
      delay(10);
  }
  PrintlnA("AHT10 or AHT20 found");

  display.init();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.display();

  delay(500);
  wifiManager = wifiManager();
  // PubSubClient mqtt_client(MQTT_SERVER, MQTT_PORT, client);
}

void getSensorData()

{
  //logPrintlnD("Start Sensor run");
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
  //logPrintlnD("AHT Success");
  data.temp = temp.temperature;
  data.humidity = humidity.relative_humidity;
  //logPrintlnD("Touch Read");
  data.soil_cap = ss.touchRead(0);
  //logPrintlnD("Temp read");
  data.soil_tempC = ss.getTemp();
  PrintA("Temperature: ");
  PrintA(String(temp.temperature, 2));
  PrintlnA(" degrees C");
  PrintA("Humidity: ");
  PrintA(String(humidity.relative_humidity, 0));
  PrintlnA("% rH");
}

void loop()
{
  // configPortalRequested();
  PrintlnA("Polling Sensors");
  getSensorData();
  PrintlnA("Poll Finished");
  delay(3500);
}

//   // String temp_str = "Temp : ";
//   // temp_str += String(data.temp, 1);
//   // temp_str += "C";
//   // String hum_str = "RH % :";
//   // hum_str += String(data.humidity, 1);
//   // // String soil_str = "Soil :";
//   // // soil_str += String(data.soil, 1);
//   // // Heltec.display->clear();
//   // Heltec.display->drawString(0, 0, temp_str);
//   // Heltec.display->drawString(0, 10, hum_str);
//   // Heltec.display->drawString(0, 20, soil_str);
//   // Heltec.display->display();

//   // #define BUFFER_LENGTH 256
//   //     //JSON

//   //     StaticJsonDocument<BUFFER_LENGTH> doc;
//   //     int battery_level = 0;
//   //     int tint = TINT_LEVEL;
//   //     doc["temp"] = t;
//   //     doc["humidty"] = h;
//   //     doc["batt"] = battery_level;
//   //     doc["tint"] = tint;

//   //     JsonArray data = doc.createNestedArray("data");

//   //     // char DeviceDatamessageBuffer[300];
//   //     serializeJson(doc, Serial);
//   // {
//   // PubSubClient (server, port, [callback], client, [stream])
// }
// //  device = device_id + device_mac;

// // client.publish("kitchenfarms/device", DeviceDatamessageBuffer);
