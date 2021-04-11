#include <Arduino.h>
#include <ArduinoJson.h>
#include "kfcore/core.h"
#include "ArduinoJson.h"
#include "MyDisplay.h"
#include "Adafruit_seesaw.h"
#include <Adafruit_Sensor.h>
// #include <Adafruit_TSL2561_U.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include "WiFiClient.h"
#include "configuration.h"
#include <Adafruit_AHTX0.h>
#include "logger.h"
#include "Adafruit_TSL2591.h"
#include "secrets.h"
#include "PubSubClient.h"
#include "sensors.h"
// #include "Fonts/FreeMono12pt7b.h"

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define TLS2591_INT_THRESHOLD_LOWER (100)
#define TLS2591_INT_THRESHOLD_UPPER (1500)
//#define TLS2591_INT_PERSIST        (TSL2591_PERSIST_ANY) // Fire on any valid change
#define TLS2591_INT_PERSIST (TSL2591_PERSIST_60) // Require at least 60 samples to fire

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED); // 25x gain
  // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS); // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */
  Serial.println("------------------------------------");
  Serial.print("Gain:         ");
  tsl2591Gain_t gain = tsl.getGain();
  switch (gain)
  {
  case TSL2591_GAIN_LOW:
    Serial.println("1x (Low)");
    break;
  case TSL2591_GAIN_MED:
    Serial.println("25x (Medium)");
    break;
  case TSL2591_GAIN_HIGH:
    Serial.println("428x (High)");
    break;
  case TSL2591_GAIN_MAX:
    Serial.println("9876x (Max)");
    break;
  }
  Serial.print("Timing:       ");
  Serial.print((tsl.getTiming() + 1) * 100, DEC);
  Serial.println(" ms");
  Serial.println("------------------------------------");
  Serial.println("");

  /* Setup the SW interrupt to trigger between 100 and 1500 lux */
  /* Threshold values are defined at the top of this sketch */
  tsl.clearInterrupt();
  tsl.registerInterrupt(TLS2591_INT_THRESHOLD_LOWER,
                        TLS2591_INT_THRESHOLD_UPPER,
                        TLS2591_INT_PERSIST);

  /* Display the interrupt threshold window */
  Serial.print("Interrupt Threshold Window: ");
  Serial.print(TLS2591_INT_THRESHOLD_LOWER, DEC);
  Serial.print(" to ");
  Serial.println(TLS2591_INT_THRESHOLD_UPPER, DEC);
  Serial.println("");
}
SSD1306Wire display(0x3c, SDA, SCL);
Adafruit_AHTX0 aht;
Adafruit_seesaw ss;
WiFiClient client;
PubSubClient mqtt_client(client);

void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" lux");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" lux");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution, 4);
  Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

unsigned long tik = 0;
struct SensorData
{
  float temp;
  float humidity;
  uint16_t soil_cap;
  double light;
  float soil_tempC;
};

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void configureLightSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED); // 25x gain
  // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS); // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */
  Serial.println("------------------------------------");
  Serial.print("Gain:         ");
  tsl2591Gain_t gain = tsl.getGain();
  switch (gain)
  {
  case TSL2591_GAIN_LOW:
    Serial.println("1x (Low)");
    break;
  case TSL2591_GAIN_MED:
    Serial.println("25x (Medium)");
    break;
  case TSL2591_GAIN_HIGH:
    Serial.println("428x (High)");
    break;
  case TSL2591_GAIN_MAX:
    Serial.println("9876x (Max)");
    break;
  }
  Serial.print("Timing:       ");
  Serial.print((tsl.getTiming() + 1) * 100, DEC);
  Serial.println(" ms");
  Serial.println("------------------------------------");
  Serial.println("");

  /* Setup the SW interrupt to trigger between 100 and 1500 lux */
  /* Threshold values are defined at the top of this sketch */
  tsl.clearInterrupt();
  tsl.registerInterrupt(TLS2591_INT_THRESHOLD_LOWER,
                        TLS2591_INT_THRESHOLD_UPPER,
                        TLS2591_INT_PERSIST);

  /* Display the interrupt threshold window */
  Serial.print("Interrupt Threshold Window: ");
  Serial.print(TLS2591_INT_THRESHOLD_LOWER, DEC);
  Serial.print(" to ");
  Serial.println(TLS2591_INT_THRESHOLD_UPPER, DEC);
  Serial.println("");
}

SensorData data;

// DHTesp dht;
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
  if (!ss.begin(0x36))
  {
    logPrintlnA("ERROR! seesaw not found");
    ;
  }
  else
  {
    logPrintlnA("seesaw started! version: ");
    logPrintlnA(String(ss.getVersion()));
  }
  if (!aht.begin())
  {
    logPrintlnA("Could not find AHT? Check wiring");
    while (1)
      delay(10);
  }
  logPrintlnA("AHT10 or AHT20 found");
  if (!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    logPrintlnA("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
  }
logPrintA("Starting Adafruit TSL2591 interrupt Test!");

  if (tsl.begin())
  {
    logPrintlnA("Found a TSL2591 sensor");
  }
  else
  {
    logPrintlnA("No sensor found ... check your wiring?");
    while (1)
      ;
  }

  /* Configure the sensor (including the interrupt threshold) */
  configureLightSensor();
  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Setup the sensor gain and integration time */
  configureSensor();
  display.init();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.display();

  delay(500);
  // wifiManager = wifiManager();
  mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
}
void advancedRead(void)
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print("[ ");
  Serial.print(millis());
  Serial.print(" ms ] ");
  Serial.print("IR: ");
  Serial.print(ir);
  Serial.print("  ");
  Serial.print("Full: ");
  Serial.print(full);
  Serial.print("  ");
  Serial.print("Visible: ");
  Serial.print(full - ir);
  Serial.print("  ");
  Serial.print("Lux: ");
  Serial.println(tsl.calculateLux(full, ir));
}

void getStatus(void)
{
  uint8_t x = tsl.getStatus();
  // bit 4: ALS Interrupt occured
  // bit 5: No-persist Interrupt occurence
  if (x & 0x10)
  {
    Serial.print("[ ");
    Serial.print(millis());
    Serial.print(" ms ] ");
    Serial.println("ALS Interrupt occured");
  }
  if (x & 0x20)
  {
    Serial.print("[ ");
    Serial.print(millis());
    Serial.print(" ms ] ");
    Serial.println("No-persist Interrupt occured");
  }

  // Serial.print("[ "); Serial.print(millis()); Serial.print(" ms ] ");
  Serial.print("Status: ");
  Serial.println(x, BIN);
  tsl.clearInterrupt();
}

void getSensorData()

{
  logPrintlnA("Start Sensor run");
  sensors_event_t humidity, temp;
  uint16_t ir, full;
  aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
  logPrintA("\tAHT [*]");
  data.temp = temp.temperature;
  data.humidity = humidity.relative_humidity;
  logPrintA("\tTouch [*]");
  data.soil_cap = ss.touchRead(0);
  logPrintA("\tTemp [*]");
  data.soil_tempC = ss.getTemp();
  logPrintlnA("\tLight [*]");
  uint32_t lum = tsl.getFullLuminosity();
  ir = lum >> 16;
  full = lum & 0xFFFF;
  logPrintlnA("Poll results:");    
  logPrintA("\tAir: ");
  logPrintA(String(temp.temperature, 1) + " C\t");
  logPrintlnA(String(humidity.relative_humidity, 0) + "%RH");
  logPrintlnA("\tLight:");
  logPrintA("\t\t" + String(ir, 1) + "IR");
  logPrintA("\t\t" + String(full, 1) + "IR");
  logPrintA("\t\t" + String(full - ir, 1) + "Visible");
  logPrintA("\t\t"+ String(tsl.calculateLux(full,ir)) + " lux");

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
void mqtt_push()
{
//
#define BUFFER_LENGTH 256
  logPrintlnA("MQTT Prep");
  StaticJsonDocument<BUFFER_LENGTH> doc;
  // int battery_level = 0;
  // int tint = TINT_LEVEL;

  doc["air_temp"] = data.temp;
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
  getSensorData();
  logPrintlnA("Poll Finished");
  mqtt_push();
  delay(1000 * 3); //60 * 15);
}