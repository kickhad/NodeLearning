#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "MyDisplay.h"
#include "ESP_WiFI.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"
#include <ArduinoJson.h>
#include <CooperativeMultitasking.h>
#include <PubSubClient.h>
#include "configuration.h"
#include <WiFiClient.h>
// #include <MQTT.h>
DHT dht(DHT_PIN, DHT_TYPE);
MyDisplay display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WiFiClient client;

void setup()
{

  // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
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

  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.print("\nStarting ConfigOnSwitch using " + String(FS_Name));
  Serial.println(" on " + String(ARDUINO_BOARD));
  Serial.println("ESP_WiFiManager Version " + String(ESP_WIFIMANAGER_VERSION));

  Serial.setDebugOutput(false);

  if (FORMAT_FILESYSTEM)
  {
    Serial.println(F("Forced Formatting."));
    FileFS.format();
  }

  // Format FileFS if not yet
#ifdef ESP32
  if (!FileFS.begin(true))
#else
  if (!FileFS.begin())
#endif
  {
    Serial.print(FS_Name);
    Serial.println(F(" failed! AutoFormatting."));

#ifdef ESP8266
    FileFS.format();
#endif

    CooperativeMultitasking tasks;
    Continuation connectMQTTClientIfNeeded;
    // MQTTClient mqttclient(&tasks, &wificlient, host, 1883, clientid, username, password);
    // MQTTTopic topic(&mqttclient, topicname);
    //   // mqttclient.connect();
    // tasks.after(10000, beginWifiIdNeeded)
  }

  unsigned long startedAt = millis();

  //Local intialization. Once its business is done, there is no need to keep it around
  // Use this to default DHCP hostname to ESP8266-XXXXXX or ESP32-XXXXXX
  //ESP_WiFiManager ESP_wifiManager;
  // Use this to personalize DHCP hostname (RFC952 conformed)
  ESP_WiFiManager ESP_wifiManager("ConfigOnSwitch");

  ESP_wifiManager.setDebugOutput(true);

  // Use only to erase stored WiFi Credentials
  //resetSettings();
  //ESP_wifiManager.resetSettings();

  //set custom ip for portal
  //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

  ESP_wifiManager.setMinimumSignalQuality(-1);

  // From v1.0.10 only
  // Set config portal channel, default = 1. Use 0 => random channel from 1-13
  ESP_wifiManager.setConfigPortalChannel(0);
  //////

#if !USE_DHCP_IP
#if USE_CONFIGURABLE_DNS
  // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
  ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
#else
  // Set static IP, Gateway, Subnetmask, Use auto DNS1 and DNS2.
  ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask);
#endif
#endif

  // New from v1.1.1
#if USING_CORS_FEATURE
  ESP_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

  // We can't use WiFi.SSID() in ESP32as it's only valid after connected.
  // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
  // Have to create a new function to store in EEPROM/SPIFFS for this purpose
  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();

  //Remove this line if you do not want to see WiFi password printed
  Serial.println("Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

  // SSID to uppercase
  ssid.toUpperCase();

  // From v1.1.0, Don't permit NULL password
  if ((Router_SSID != "") && (Router_Pass != ""))
  {
    LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
    wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());

    ESP_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
    Serial.println("Got stored Credentials. Timeout 120s for Config Portal");
  }
  else
  {
    Serial.println("Open Config Portal without Timeout: No stored Credentials.");
    digitalWrite(PIN_LED, LED_ON); // Turn led on as we are in configuration mode.

    initialConfig = true;
  }

  if (initialConfig)
  {
    Serial.println("Starting configuration portal.");
    digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

    //sets timeout in seconds until configuration portal gets turned off.
    //If not specified device will remain in configuration mode until
    //switched off via webserver or device is restarted.
    //ESP_wifiManager.setConfigPortalTimeout(600);

    // Starts an access point
    if (!ESP_wifiManager.startConfigPortal((const char *)ssid.c_str(), password))
      Serial.println("Not connected to WiFi but continuing anyway.");
    else
    {
      Serial.println("WiFi connected...yeey :)");
    }

    // Stored  for later usage, from v1.1.0, but clear first
    memset(&WM_config, 0, sizeof(WM_config));

    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      String tempSSID = ESP_wifiManager.getSSID(i);
      String tempPW = ESP_wifiManager.getPW(i);

      if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);

      if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);

      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ((String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw);
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }

    saveConfigData();
  }

  digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.

  startedAt = millis();

  if (!initialConfig)
  {
    // Load stored data, the addAP ready for MultiWiFi reconnection
    loadConfigData();

    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ((String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw);
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("ConnectMultiWiFi in setup");

      connectMultiWiFi();
    }
  }

  Serial.print("After waiting ");
  Serial.print((float)(millis() - startedAt) / 1000L);
  Serial.print(" secs more in setup(), connection result is ");

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("connected. Local IP: ");
    Serial.println(WiFi.localIP());
  }
  else
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
