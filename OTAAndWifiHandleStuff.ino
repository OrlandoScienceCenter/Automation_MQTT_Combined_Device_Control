void wifiSetupOTA(){

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(OTA_HOSTNAME);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    client.loop();
    client.publish(TOPIC_T, "OTA Update starting now.");
    client.loop();
        
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println(F("\nEnd"));
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  // I don't think any of the MQTT messages will actually get out because we're in OTA. I should probably remove them. 
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println(F("ERR: OTA Auth Failed"));
      client.publish(TOPIC_T, "ERR: OTA Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println(F("ERR: OTA Begin Failed"));
      client.publish(TOPIC_T, "ERR: OTA Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println(F("ERR: OTA Connect Failed"));
      client.publish(TOPIC_T, "ERR: OTA Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println(F("Receive Failed"));
      client.publish(TOPIC_T, "ERR: OTA Recieve Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println(F("End Failed"));
      client.publish(TOPIC_T, "ERR: OTA End Failed");
    }
  });
  
  ArduinoOTA.begin();
  Serial.println(F("Ready for OTA at IP:"));
  Serial.println(WiFi.localIP());
}

void wifiSetup(){
  WiFi.mode(WIFI_STA);
  
  Serial.println();
  Serial.print(F("Connecting"));

  wifiMulti.addAP(WIFI_1, WIFI_PSK);
  wifiMulti.addAP(WIFI_2, WIFI_PSK);
  wifiMulti.addAP(WIFI_3, WIFI_PSK);
  wifiMulti.addAP(WIFI_4, WIFI_PSK);
  wifiMulti.addAP(WIFI_5, WIFI_PSK);
  wifiMulti.addAP(WIFI_6, WIFI_PSK);  
  
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(1000);
    Serial.print('.');
    i++;  // Increment timeout counter
    
    if (i > 20){
      // If we've been trying to connect for more than 20 seconds...
      i = 0; // Reset timeout
      break; // Dump out of the loop
  
    }
  }

  // Check if we're actually connected.
  if (wifiMulti.run() == WL_CONNECTED){
    wifiStillNeedsToConnect = 0;
    
    Serial.print(F("Connected to "));
    Serial.println(WiFi.SSID());              // Tell us what network we're connected to
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
  } else {wifiStillNeedsToConnect = 1;}
}
