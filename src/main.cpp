#include <Arduino.h>
#include <ArduinoJson.h>
// #include "heltec.h"
#include "kfcore/core.h"
#include "ArduinoJson.h"
#include "MyDisplay.h"
// #include "DHTesp.h"
#include "Adafruit_seesaw.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#include "configuration.h"
#include <Adafruit_AHTX0.h>
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_AHTX0 aht;
Adafruit_seesaw ss;
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
    Serial.println("ERROR! seesaw not found");
      ;
  }
  else
  {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  if (!aht.begin())
  {
    Serial.println("Could not find AHT? Check wiring");
    while (1)
      delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
  // Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  // Heltec.display->init();
  // dht.setup(27, DHTesp::DHT11);

  delay(500);
  // Heltec.display->clear();
  // Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  // #TODO Splash version
  // Heltec.display->setFont(ArialMT_Plain_10);
  // Heltec.display->drawString(0, 0, "Kitchen Farms");
  // Heltec.display->display();

  // wifiManager = wifiManager();

  // PubSubClient mqtt_client(MQTT_SERVER, MQTT_PORT, client);
}

void loop()
{
  // configPortalRequested();
  sensors_event_t humidity, temp;
 aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  { // sensor loop
    // DHT Poll & Print
    // Serial.println("Reading DHT");
    
    aht.getEvent(&humidity, &temp);
    float soil_tempC = ss.getTemp();
    uint16_t capread = ss.touchRead(0);
    data.temp = ss.getTemp();       // Soil Temp
    data.soil_cap = ss.touchRead(0); // Soil cap
    // data.humidity = dht.getHumidity();
    // data.soil = 999;
    data.light = 999;
    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degrees C");
    Serial.print("Humidity: ");
    Serial.print(humidity.relative_humidity);
    Serial.println("% rH");

    if (isnan(data.temp))
    {
      data.temp = 999;
    }
    if (isnan(data.humidity))
    {
      data.humidity = 999;
    }

    //SOIL MOISTURE

    // int readSensor;
    digitalWrite(SENSOR_POWER, HIGH);
    delay(10); // Allow power to settle
    int soil = digitalRead(SENSOR_PIN);
    digitalWrite(SENSOR_POWER, LOW);

    // //LIGHT

    // int sensorValue = analogRead(A0);

    // float percent = (sensorValue / 1023.0) * 100;
    // display.update_display("Light:", percent, "%");
    // Update display
    //

    String temp_str = "Temp : ";
    temp_str += String(data.temp, 1);
    temp_str += "C";
    String hum_str = "RH % :";
    hum_str += String(data.humidity, 1);
    // String soil_str = "Soil :";
    // soil_str += String(data.soil, 1);
    // Heltec.display->clear();
    // Heltec.display->drawString(0, 0, temp_str);
    // Heltec.display->drawString(0, 10, hum_str);
    // Heltec.display->drawString(0, 20, soil_str);
    // Heltec.display->display();
  }

  // #define BUFFER_LENGTH 256
  //     //JSON

  //     StaticJsonDocument<BUFFER_LENGTH> doc;
  //     int battery_level = 0;
  //     int tint = TINT_LEVEL;
  //     doc["temp"] = t;
  //     doc["humidty"] = h;
  //     doc["batt"] = battery_level;
  //     doc["tint"] = tint;

  //     JsonArray data = doc.createNestedArray("data");

  //     // char DeviceDatamessageBuffer[300];
  //     serializeJson(doc, Serial);
  delay(3000);
  // {
  // PubSubClient (server, port, [callback], client, [stream])
}
//  device = device_id + device_mac;

// client.publish("kitchenfarms/device", DeviceDatamessageBuffer);
