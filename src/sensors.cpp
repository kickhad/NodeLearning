#include "Adafruit_TSL2591.h"
#include <Adafruit_AHTX0.h>
#include "Adafruit_seesaw.h"
#include <Adafruit_Sensor.h>
#include "logger/logger.h"
#include "sensors.h"
#include "configuration.h"

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)
Adafruit_AHTX0 aht;
Adafruit_seesaw ss;
sensors_event_t humidity, temp;
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

    aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
    Serial.println(temp.temperature);
    Serial.println(humidity.relative_humidity);
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
  logPrintlnA("Start Sensor run");
  
  aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
  Serial.println(temp.temperature);
  Serial.println(humidity.relative_humidity);
  logPrintA("\tAHT [*]");
  this->p_data.air_temp = temp.temperature;
  this->p_data.humidity = humidity.relative_humidity;
  logPrintA("\tTouch [*]");
  this->p_data.soil_cap = ss.touchRead(0);
  logPrintA("\tTemp [*]");
  this->p_data.soil_tempC = ss.getTemp();
  logPrintlnA("\tLight [*]");
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  logPrintlnA("Poll results:");
  logPrintlnA("\tAir: ");
  logPrintlnA("\t\t" + String(temp.temperature) + " C\t");
  logPrintlnA("\t\t" + String(humidity.relative_humidity) + "%RH");

  logPrintlnA("\tLight:\t");
  logPrintlnA("\t\tIR:\t" + String(ir));
  logPrintlnA("\t\tVsbl\t" + String(full - ir));
  logPrintlnA("\t\tLux\t" + String(tsl.calculateLux(full, ir)));
}
