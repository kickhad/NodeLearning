#ifndef SENSORS_H
#define SENSORS_H
struct SensorData
{
  float air_temp;
  float humidity;
  uint16_t soil_cap;
  double light;
  float soil_tempC;
};
class Sensors
{
public:
    Sensors();
    void read(void);
    void refresh();
    SensorData data;
    SensorData p_data;

private:
    // void advancedRead(void);
    void getStatus(void);
    void configureLightSensor(void);
    void displayLightSensorDetails(void);
};

#endif