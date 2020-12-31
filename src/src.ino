#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "configuration.h"
#include "MyDisplay.h"

//node.dsleep(us, option, instant)
// WiFiServer server(80);
// #define DEBUG

WiFiClient espClient;
PubSubClient client(espClient);
String device;
String device_id = DEVICE_ID;
String device_type = DEVICE_TYPE;
String device_mac = WiFi.macAddress();
DHT dht(DHT_PIN, DHT_TYPE);
MyDisplay oled(SCREEN_WIDTH, SCREEN_WIDTH, &Wire, OLED_RESET);
int battery = BATTERY;
int tint = TINT;

void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  payload[length] = '\0';
  String strTopic = String((char *)topic);
}

void setup()
{

Serial.begin(115200);
#define custom_html
  // PIN MODES
  pinMode(SENSOR_POWER, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(DHT_PIN, INPUT);
  pinMode(SENSOR_PIN, INPUT);
  digitalWrite(SENSOR_POWER, LOW);
  // START WIFI MANAGER
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect())
  {
    Serial.println(STR_ERR_CONNECT_TIMEOUT);

    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  Serial.println(STR_CONNECTED);

  {
    Serial.println(STR_CONNECTED);
    oled.update_display("Internet:", "Failed");
    delay(5000);
    oled.clear_display();
  }

  if (!client.connected())
  {
    Serial.println(STR_CONNECTED);
    oled.update_display("Internet", STR_CONNECTED);
    delay(5000);
    oled.clear_display();
    Serial.print(STR_ATTEMPT_MQTT);
    String clientId = STR_CLIENT_ID;
    clientId += String(random(0xffff), HEX);
    (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS));
    {
      Serial.println(STR_CONNECTED);

      client.publish("outTopic", "Kitchen Farms");
      client.setServer(MQTT_SERVER, MQTT_PORT);
      client.setCallback(callback);
      client.publish("kitchenfarms/device", "hello"); //Topic name
    }
  }
  /// END WIFI MANAGER

  // dht.begin();
}
void loop()
{

  // //read temperature and humidity
  // float t = 1.3;
  // //  dht.readTemperature();
  // float h = 3.3;
  // // dht.readHumidity();
  // if (isnan(h) || isnan(t))
  // {
  //   Serial.println("Failed to read from DHT sensor!");
  // }
  // oled.display_next();
  // // display temperature
  // oled.update_display("Temp: ", t, "C");
  // oled.display_next();

  // // display humidity
  // oled.update_display("Humidity: ", h, "%");
  // oled.display_next();

  // int sensorValue = analogRead(A0);             // read the input on analog pin 0
  // float percent = (sensorValue / 1023.0) * 100; // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)

  // oled.update_display("Light:", percent, "%");
  // oled.display_next();

  // int readSensor();
  // digitalWrite(SENSOR_POWER, HIGH);  // Turn the sensor ON
  // delay(10);                         // Allow power to settle
  // int val = digitalRead(SENSOR_PIN); // Read the analog value form sensor
  // digitalWrite(SENSOR_POWER, LOW);   // Turn the sensor OFF

  // // Determine status of our soil moisture situation
  // if (val)
  // {
  //   oled.update_display("Wet");
  // }
  // else
  // {
  //   oled.update_display("Dry");
  // }

  // oled.display_next();

  // {
  //   StaticJsonBuffer<300> JSONbuffer;
  //   JsonObject &JSONencoder = JSONbuffer.createObject();
  //   device = device_id + device_mac;
  //   JSONencoder["device_id"] = device;
  //   JSONencoder["device_type"] = device_type;
  //   JsonArray &values = JSONencoder.createNestedArray("values");
  //   values.add(percent);
  //   values.add(t);
  //   values.add(val);
  //   values.add(h);
  //   values.add(tint);
  //   values.add(battery);

  //   char DeviceDatamessageBuffer[300];
  //   JSONencoder.printTo(DeviceDatamessageBuffer, sizeof(DeviceDatamessageBuffer));

  //   Serial.println(DeviceDatamessageBuffer);

  //   client.publish("kitchenfarms/device", DeviceDatamessageBuffer);
  // }
  // digitalWrite(OUTPUT, LOW);
  // // <img src="data/Picture1.png" alt="Kitchen Farm Logo">);
  // //  client.println("<img src=\"https://www.kitchenfarms.ca/wp-content/uploads/2020/03/kitchenfarms-1-1.png\" alt=\"kitchenFarms Logo\" height=\"42\" width=\"42\">");

  delay(10000);
  // oled.clear_display();
  // Serial.println("I am going to sleep now");
  // ESP.deepSleep(0);
  // Serial.println("All done");
  //node.dsleep(60000000);
}
// ESP.deepSleep(0xffffffff);