#include <Arduino.h>
#include "kfnetman.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ESP_wifiManager.h"
// #include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "MyDisplay.h"
// #include "ESP_WiFI.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"
#include <ArduinoJson.h>
// #include <CooperativeMultitasking.h>
#include <PubSubClient.h>
#include "configuration.h"
#include <WiFiClient.h>
// #include <MQTT.h>
DHT dht(DHT_PIN, DHT_TYPE);
MyDisplay display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WiFiClient client;
ESP_WiFiManager wifiManager;
void setup()
{

  // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  wifiManager = wifiManager();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.update_display("Kitchen Farms");
  display.display();
  // pinMode(PIN_LED, OUTPUT);
  // pinMode(TRIGGER_PIN, INPUT_PULLUP);
  // pinMode(TRIGGER_PIN2, INPUT_PULLUP);

  Serial.println(ESP_wifiManager.getStatus(WiFi.status()));
  WiFiClient client;
  PubSubClient mqtt_client(MQTT_SERVER, MQTT_PORT, client);
}

void loop()
{
  configPortalRequested();

  // DHT Poll & Print

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h))
  {
    Serial.println("Failed to read from DHT sensor");

    display.update_display("Temp:", t, "C");
    display.display_next();
    display.update_display("Humidity:", h, "\%");
    display.display_next();

    //SOIL MOISTURE
    {
      int readSensor();
      digitalWrite(SENSOR_POWER, HIGH);
      delay(10); // Allow power to settle
      int val = digitalRead(SENSOR_PIN);
      digitalWrite(SENSOR_POWER, LOW);
      if (val)
      {
        display.print("Wet");
      }
      else
      {
        display.print("Dry");
      }
      display.update_display("");
      display.clear_display();
      display.display_next();
    }

    //LIGHT

    int sensorValue = analogRead(A0);

    float percent = (sensorValue / 1023.0) * 100;
    // display.update_display("Light:", percent, "%");
    display.clear_display();

    //JSON
    {
      StaticJsonDocument<BUFFER_LENGTH> doc;
      int battery_level = 0;
      int tint = TINT_LEVEL;
      doc["temp"] = t;
      doc["humidty"] = h;
      doc["batt"] = battery_level;
      doc["tint"] = tint;

      JsonArray data = doc.createNestedArray("data");

      // char DeviceDatamessageBuffer[300];
      serializeJson(doc, Serial);
    }
    // {
    // PubSubClient (server, port, [callback], client, [stream])
  }
  //  device = device_id + device_mac;
  Serial.println();

  // client.publish("kitchenfarms/device", DeviceDatamessageBuffer);
}
