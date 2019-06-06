extern bool OTAReportFlag;
extern bool wifiStillNeedsToConnect;

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
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Updating " + type);
    Serial.println();
    yield(); // Give the serial data time to get out before we kill it
    delay(10);
    Serial.end(); // Kill serial because FOR SOME REASON it's unstable when doing OTA with serial on.
    // See https://github.com/esp8266/Arduino/issues/3881 for more information. 
    delay(10);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.begin(115200); // OTA is done we need serial back now
    delay(500);
    Serial.println(F("\nEnd"));
    OTAReportFlag = 1;
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100))); 
    // Can't output serial during OTA see above github issue URL for more info
  });

  // Sent MQTT errors so user can see if there's a problem.
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.begin(115200); // OTA is having a problem we need serial back now
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println(F("ERR: OTA Auth Failed"));
      client.publish(TOPIC_T, "ERR: OTA Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println(F("ERR: OTA Begin Failed"));
      client.publish(TOPIC_T, "ERR: OTA Begin Fail. Manually reset ESP and try again. Sorry ><");
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

  // We set this so we have some time after OTA to post serial messages and MQTT message
  ArduinoOTA.setRebootOnSuccess(false);
  
  ArduinoOTA.begin();
  Serial.println(F("Ready for OTA at IP:"));
  Serial.println(WiFi.localIP());  
}

void wifiSetup(){
  yield();
  ESP.wdtFeed(); 
      
  WiFi.mode(WIFI_STA);
  
  Serial.println();
  Serial.print(F("Connecting"));

  if (strcmp(ROOM_NAME, "OurPlanet") == 0){
    // This is so the OP exhibits only can connect to the OP router, because sometimes it's off when they are getting turned on.
    wifiMulti.addAP(WIFI_4, WIFI_PSK); // Just OP
  } else {
    wifiMulti.addAP(WIFI_1, WIFI_PSK);
    wifiMulti.addAP(WIFI_2, WIFI_PSK);
    wifiMulti.addAP(WIFI_3, WIFI_PSK);
    wifiMulti.addAP(WIFI_4, WIFI_PSK);
    wifiMulti.addAP(WIFI_5, WIFI_PSK);
    wifiMulti.addAP(WIFI_6, WIFI_PSK);  
  }
  
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    for(int j = 0; j < 10; j++){
      delay(100);  
     
      yield();
      ESP.wdtFeed(); 
    }
    
    Serial.print('.');
    i++;  // Increment timeout counter
    
    if (i > 10){
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
