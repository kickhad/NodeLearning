#include "Adafruit_TSL2591.h"
#include <Adafruit_AHTX0.h>
#include "Adafruit_seesaw.h"
#include <Adafruit_Sensor.h>
#include "logger/logger.h"
#include "sensors.h"
#include "configuration.h"
#include "ArduinoJson.h"
#include "PubSubClient.h"
#include "WiFiClient.h"
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)
Adafruit_seesaw ss;
Adafruit_AHTX0 aht;
// TODO rework with ptrs
uint32_t lum, ir, full;
WiFiClient client;
PubSubClient mqtt_client;

void Sensors::getStatus(void)
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

void Sensors::configureLightSensor(void)
{
  // TODO On the fly change possible. Move to server overrideable
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

void Sensors::displayLightSensorDetails(void)
{
  sensor_t sensor;
  // TODO Add this info to device provisioning
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

Sensors::Sensors(void)
{

  // ctor
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
  else
  {

    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
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
    ;
  }
  
  /* Configure the sensor (including the interrupt threshold) */
  this->configureLightSensor();
  /* Display some basic information on this sensor */
  this->displayLightSensorDetails();
}

void Sensors::refresh()
{
  // TODO Err Check Sensor sanity, is warmup needed?
  // TODO Implement stabilizing filter
  sensors_event_t humidity, temp;
  SensorData data;
  uint8_t aht_status = aht.getStatus();
  data.aht_status = aht_status;
  logPrintA("Sensor check: ");
  aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
  float tmp = temp.temperature;
  float rh = humidity.relative_humidity;
  data.air_temp = tmp;
  data.humidity = rh;
  logPrintA("\tAHT [*]");
  data.soil_moisture = ss.touchRead(0);
  data.soil_temp = ss.getTemp();
  logPrintA("\tSoil [*]");
  lum = tsl.getFullLuminosity();
  logPrintlnA("\tLight [*]");
  uint16_t ir = lum >> 16;
  uint16_t full = lum & 0xFFFF;
  data.ir = ir;
  data.full = full;
  uint16_t lux;
  lux = tsl.calculateLux(full, ir);
  data.lux = lux;
  StaticJsonDocument<256> doc;
  doc["air_temp"] = data.air_temp;
  doc["rh"] = data.humidity;
  doc["soil_cap"] = data.soil_moisture;
  doc["soil_temp"] = data.soil_temp;
  doc["ir"] = data.ir;     // TODO daylight sensor
  doc["full"] = data.full; // TODO daylight sensor
  doc["lux"] = data.lux;
  char buffer[256];
  serializeJson(doc, buffer);
  this->p_data = data;
  
  mqtt_client.setClient(client);
  mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
   if (mqtt_client.connect("kfdevice", MQTT_USER, MQTT_PASS))
  {
    mqtt_client.publish("kitchenfarms/device_alpha", buffer);
  }
  else
  {
    logPrintE("MQTT Disconnected");
  } 
}

void Sensors::outputData()
{
  SensorData data = this->p_data;
  logPrintlnA("Poll results:");
  logPrintlnA("Air: ");
  logPrintlnA("\t" + String(data.air_temp, 1) + " C");
  logPrintlnA("\t" + String(data.humidity) + " %RH");
  logPrintlnA("\tLight:\t");
  logPrintlnA("\t\tIR:\t" + String(data.ir));
  logPrintlnA("\t\tVsbl\t" + String(data.full));
  logPrintlnA("\t\tLux\t" + String(data.lux));

  logPrintlnA("\tSoil\t");
  logPrintlnA("\t\tMoist\t" + String(data.soil_moisture));
  logPrintlnA("\t\tTemp\t" + String(data.soil_temp));

  logPrintlnA("Status\t" + String(data.aht_status));
}
