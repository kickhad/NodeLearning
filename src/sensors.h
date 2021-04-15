#ifndef SENSORS_H
#define SENSORS_H
#include "ArduinoJson.h"
#include "configuration.h"
struct SensorData
{
  float air_temp;
  float humidity;
  uint16_t soil_moisture, ir, full, lux;
  double light;
  float soil_temp;
  uint8_t aht_status;
};
class Sensors

{
public:
  Sensors();
  void read(void);
  void refresh();
  // SensorData data;
  SensorData p_data;
  void outputData(void);
  char* JSON;
  void mqtt_push(void);
private:
  // void advancedRead(void);
  void getStatus(void);
  void configureLightSensor(void);
  void displayLightSensorDetails(void);
};

#endif