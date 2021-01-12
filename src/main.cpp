#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "MyDisplay.h"
#include "ESP_WiFI.h"
#include "Adafruit_Sensor.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
MyDisplay display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
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
  pinMode(PIN_LED, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_PIN2, INPUT_PULLUP);

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
}

void loop()
{
  // is configuration portal requested?
  if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW))
  {
    configPortalRequested();
    //****
    //  Try updating the functions in ESP_WiFi to do these things. Follow the logic of configPortalRequested()
    //*****

    //   // Nested If and Else missing catch {}
    //   if (WiFi.status() == WL_CONNECTED)
    //   {
    //     Serial.print("H"); // H means connected to WiFi
    //     display.update_display('Internet', 'Connected');
    //     display.clear_display();
    //     display.display_next();
    //   }
    //   else
    //   {
    //     Serial.print("F");
    //     display.update_display('Internet', ' Failed');
    //     display.clear_display();
    //     display.display_next();
    //   }
    // }
    // if (!client.connected())
    // {
    //   Serial.print("Attempting MQTT connection...");
    //   String clientId = "ESP8266Client-";
    //   clientId += String(random(0xffff), HEX);
    //   (client.connect(clientId.c_str(), mqttUser, mqttPassword));
    //   {
    //     Serial.println("connected");

    //     client.publish("outTopic", "hello world");
    //     client.setServer(mqttServer, mqttPort);
    //     client.setCallback(callback);
    //     // Serial.println(WiFi.macAddress());
    //     //  Serial.print("MAC Address:  ");
    //     client.publish("kitchenfarms/device", "hello"); //Topic name
  }
  //   // put your main code here, to run repeatedly

  // ****
  // have to declare dht at the begining, that didn't get moved from old code
  float t = dht.readTemperature() float h = dht.readHumidity() if (isnan(h) || isnan(t))
                                                Serial.println("Failed to read from DHT sensor");
  // *** 
  // float t declares t as a float variable, which you did above. 
  // things from here down look like objects/variables being used befor they are declared. 
  // *** 

  display.update_display('Temp:', t, 'C');
  // *** 
  // display.clear_display();
  // *** 
  display.display_next();
  display.update_display('Humidity:', h, '%');
  // display.clear_display();
  display.display_next();

  //SOIL MOISTURE
  {
    int readSensor();
    digitalWrite(sensorPower, HIGH);
    delay(10); // Allow power to settle
    int val = digitalRead(sensorPin);
    digitalWrite(sensorPower, LOW);
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
  {
  }
  int sensorValue = analogRead(A0);

  float percent = (sensorValue / 1023.0) * 100;
  display.update_display('Light:', percent, '%');
  display.clear_display();
}

//JSON
{
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject &JSONencoder = JSONbuffer.createObject();
  device = device_id + device_mac;
  JSONencoder["device_id"] = device;
  JSONencoder["device_type"] = device_type;
  JsonArray &values = JSONencoder.createNestedArray("values");
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
      );
      }
