// if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW))
//   {
//     Serial.println("\nConfiguration portal requested.");
//     digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

//     //Local intialization. Once its business is done, there is no need to keep it around
//     ESP_WiFiManager ESP_wifiManager("ConfigOnSwitch");

//     ESP_wifiManager.setMinimumSignalQuality(-1);

//     // From v1.0.10 only
//     // Set config portal channel, default = 1. Use 0 => random channel from 1-13
//     ESP_wifiManager.setConfigPortalChannel(0);
//     //////

//     //set custom ip for portal
//     //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));

// #if !USE_DHCP_IP    
//   #if USE_CONFIGURABLE_DNS  
//     // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
//     ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask, dns1IP, dns2IP);  
//   #else
//     // Set static IP, Gateway, Subnetmask, Use auto DNS1 and DNS2.
//     ESP_wifiManager.setSTAStaticIPConfig(stationIP, gatewayIP, netMask);
//   #endif 
// #endif       

//   // New from v1.1.1
// #if USING_CORS_FEATURE
//   ESP_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
// #endif

//     //Check if there is stored WiFi router/password credentials.
//     //If not found, device will remain in configuration mode until switched off via webserver.
//     Serial.print("Opening configuration portal. ");
//     Router_SSID = ESP_wifiManager.WiFi_SSID();
//     Router_Pass = ESP_wifiManager.WiFi_Pass();
    
//     // From v1.1.0, Don't permit NULL password
//     if ( (Router_SSID != "") && (Router_Pass != "") )
//     {
//       ESP_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
//       Serial.println("Got stored Credentials. Timeout 120s");
//     }
//     else
//       Serial.println("No stored Credentials. No timeout");

//     //Starts an access point
//     //and goes into a blocking loop awaiting configuration
//     if (!ESP_wifiManager.startConfigPortal((const char *) ssid.c_str(), password))
//     {
//       Serial.println("Not connected to WiFi but continuing anyway.");
//     }
//     else
//     {
//       //if you get here you have connected to the WiFi
//       Serial.println("connected...yeey :)");
//       Serial.print("Local IP: ");
//       Serial.println(WiFi.localIP());
//     }

//     // Only clear then save data if CP entered and with new valid Credentials
//     // No CP => stored getSSID() = ""
//     if ( String(ESP_wifiManager.getSSID(0)) != "" && String(ESP_wifiManager.getSSID(1)) != "" )
//     {
//       // Stored  for later usage, from v1.1.0, but clear first
//       memset(&WM_config, 0, sizeof(WM_config));
      
//       for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
//       {
//         String tempSSID = ESP_wifiManager.getSSID(i);
//         String tempPW   = ESP_wifiManager.getPW(i);
    
//         if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
//           strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
//         else
//           strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);
    
//         if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
//           strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
//         else
//           strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);  
    
//         // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
//         if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE) )
//         {
//           LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
//           wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
//         }
//       }
    
//       saveConfigData();
//     }

//     digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.
//   }