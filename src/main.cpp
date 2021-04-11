#include <Arduino.h>
#include <ArduinoJson.h>
#include "kfcore/core.h"
#include "ArduinoJson.h"
#include "MyDisplay.h"
#include <Wire.h>
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
WiFiClient client;
PubSubClient mqtt_client(client);
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
  mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
}

void mqtt_push()
{
//
#define BUFFER_LENGTH 256
  logPrintlnA("MQTT Prep");
  StaticJsonDocument<BUFFER_LENGTH> doc;
  // int battery_level = 0;
  // int tint = TINT_LEVEL;
  SensorData data = sensors->p_data;
  doc["air_temp"] = data.air_temp;
  doc["rh"] = data.humidity;
  doc["soil_cap"] = data.soil_cap;
  doc["soil_temp"] = data.soil_tempC;
  doc["sunlight"] = 0; // TODO daylight sensor
  // doc["batt"] = battery_level;
  // doc["tint"] = tint;
  char buffer[BUFFER_LENGTH];
  serializeJson(doc, buffer);
  if (mqtt_client.connect("kfdevice", MQTT_USER, MQTT_PASS))
  {
    mqtt_client.publish("kitchenfarms/device_alpha", buffer);
  }
  else
  {
    logPrintE("MQTT Disconnected");
  } // mqtt_push();
  // mqtt_client.publish("kitchenfarms/device_alpha", buffer);
  logPrintlnA("MQTT Sent");
}

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
  logPrintlnA("Polling Sensors");
  sensors->refresh();
  logPrintlnA("Poll Finished");
  mqtt_push();
  delay(1000 * 3); //60 * 15);
}