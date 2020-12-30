#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "configuration.h"
#include "helpers.h"
#include "display.h"

//node.dsleep(us, option, instant)
// WiFiServer server(80);
#define DEBUG

WiFiClient espClient;
PubSubClient client(espClient);
String device;
String device_id = DEVICE_ID;
String device_type = DEVICE_TYPE;
String device_mac = WiFi.macAddress();
DHT dht(DHT_PIN, DHT_TYPE);

int battery = BATTERY;
int tint = TINT;

void configModeCallback (WiFiManager *myWiFiManager) {
  serial_monitor("Entered config mode");
  serial_monitor(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  serial_monitor(myWiFiManager->getConfigPortalSSID());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String strTopic = String((char*)topic);
}

void setup() {
  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
  #endif
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
  if(!wifiManager.autoConnect()) {    
      serial_monitor(ERR_CONNECT_TIMEOUT);
  
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 
      Serial.println(STR_CONNECTED);
  
   {
    Serial.println(STR_CONNECTED);
    update_display("Internet:","Failed");
    delay (5000);
    clear_display();
  }

  if (!client.connected()) {
    Serial.println(STR_CONNECTED);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Internet:");
    display.setTextSize(2);
    display.setCursor(0, 30);
    display.print(STR_CONNECTED);
    display.display();
    delay (5000);
    display.clearDisplay();
    Serial.print(STR_ATTEMPT_MQTT);
    String clientId = STR_CLIENT_ID;
    clientId += String(random(0xffff), HEX);
    (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)); {
      Serial.println(STR_CONNECTED);


      client.publish("outTopic", "Kitchen Farms");
      client.setServer(MQTT_SERVER, MQTT_PORT);
      client.setCallback(callback);
      client.publish("kitchenfarms/device", "hello"); //Topic name
    }
  }  
  /// END WIFI MANAGER


  
  dht.begin(); {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;);
      display.display();
      delay(2000);

    }
    delay(5000);
    display.clearDisplay();
    display.setTextColor(WHITE);

  }

}


void update_display(String txt)
{
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(txt);
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.display();
}

void update_display(String txt, float t, String uom)
{
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(txt);
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.print(t);
  display.print(" ");
  display.setTextSize(3);
  display.print(uom);
  display.display();
}

void display_next(){
  delay (4000);
  display.clearDisplay();
  delay(2000);
  
}
void loop() {

  //read temperature and humidity
  float t = 1.3;
  //  dht.readTemperature();
  float h = 3.3;
  // dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  display_next();
  // display temperature
  update_display("Temp: ", t, "C");  
  display_next();
  
  // display humidity
  update_display ("Humidity: ", h, "%");
  display_next();
  
  int sensorValue = analogRead(A0);   // read the input on analog pin 0
  float percent = (sensorValue / 1023.0) * 100; // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)

  update_display("Light:", percent, "%");
  display_next();

  int readSensor() ;
  digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
  delay(10);              // Allow power to settle
  int val = digitalRead(sensorPin); // Read the analog value form sensor
  digitalWrite(sensorPower, LOW);   // Turn the sensor OFF


  // Determine status of our soil moisture situation
  if (val) {
    update_display("Wet");
  } else {
    update_display("Dry");
  }
  
  display.print("");
  display.display();
  display_next();
  
  {
    StaticJsonBuffer<300> JSONbuffer;
    JsonObject & JSONencoder = JSONbuffer.createObject();
    device = device_id + device_mac;
    JSONencoder["device_id"] = device;
    JSONencoder["device_type"] = device_type;
    JsonArray& values = JSONencoder.createNestedArray("values");
    values.add(percent);
    values.add(t);
    values.add(val);
    values.add(h);
    values.add(tint);
    values.add(battery);



    char DeviceDatamessageBuffer[300];
    JSONencoder.printTo(DeviceDatamessageBuffer, sizeof(DeviceDatamessageBuffer));

    Serial.println(DeviceDatamessageBuffer);


    client.publish("kitchenfarms/device", DeviceDatamessageBuffer);
  }
  digitalWrite(OUTPUT, LOW);
// <img src="data/Picture1.png" alt="Kitchen Farm Logo">);
//  client.println("<img src=\"https://www.kitchenfarms.ca/wp-content/uploads/2020/03/kitchenfarms-1-1.png\" alt=\"kitchenFarms Logo\" height=\"42\" width=\"42\">");

 
  

   display.display();
  delay(10000);
  display.clearDisplay();
  Serial.println ("I am going to sleep now");
 ESP.deepSleep(0);
Serial.println ("All done");
 //node.dsleep(60000000);

  }
 // ESP.deepSleep(0xffffffff);