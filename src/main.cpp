#include <Arduino.h>
#include <ArduinoJson.h>
#include "kfcore/core.h"
#include "ArduinoJson.h"
#include "MyDisplay.h"
// #include <Wire.h>
// #include <SSD1306Wire.h>
#include "WiFiClient.h"
#include "configuration.h"
#include "logger/logger.h"
#include "PubSubClient.h"
#include "secrets.h"
#include "sensors.h"
// #include "Fonts/FreeMono12pt7b.h"

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/

// SSD1306Wire display(0x3c, SDA, SCL);
// WiFiClient client;
// PubSubClient mqtt_client(client);
Sensors *sensors;
unsigned long tik = 0;

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/

// KfClient client;
// MyDisplay display;
void setup()
{

  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    logPrintlnA("Connecting to WiFi..");
  }
  Sensors p_sensors = Sensors();
  sensors = &p_sensors;
  // display.init();
  // display.setFont(ArialMT_Plain_10);
  // display.drawString(0, 0, "Hello world");
  // display.display();

  delay(500);
  // wifiManager = wifiManager();
  // 
}

// void mqtt_push()
// {
// //

//   logPrintlnA("MQTT Prep");
//   StaticJsonDocument<128> o_doc = sensors->JSON;
//   // int battery_level = 0;
//   // int tint = TINT_LEVEL;
  
  
//   // doc["batt"] = battery_level;
//   // doc["tint"] = tint;
//   char buffer[128];
//   serializeJson(o_doc, buffer);
//   Serial.print(buffer);
//   if (mqtt_client.connect("kfdevice", MQTT_USER, MQTT_PASS))
//   {
//     mqtt_client.publish("kitchenfarms/device_alpha", buffer);
//   }
//   else
//   {
//     logPrintE("MQTT Disconnected");
//   } // mqtt_push();
//   // mqtt_client.publish("kitchenfarms/device_alpha", buffer);
//   logPrintlnA("MQTT Sent");
// }

//   JsonArray data = doc.createNestedArray("data");

//   // char DeviceDatamessageBuffer[300];
//   serializeJson(doc, Serial);
//   {
//       PubSubClient(server, port, [callback], client, [stream])} device = device_id + device_mac;
//   }
// client.publish("kitchenfarms/device_alpha", DeviceDatamessageBuffer);

void loop()
{
  // configPortalRequested();
  logPrintlnA("<--- sensor run --- ");
  sensors->refresh();
  delay(500);
  // sensors->outputData();
  logPrintlnA("--- end sensor run --->");
  // mqtt_push();
  delay(1000 * 3); //60 * 15);
}