void wifiSetupOTA(){
  ArduinoOTA.setHostname(OTA_HOSTNAME);
 
  ArduinoOTA.onStart([]() {});
  ArduinoOTA.onEnd([]() {});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print(F("Err"));
  });
  ArduinoOTA.begin();
}

void wifiSetup(){
  Serial.println();
  Serial.print("Connecting");

  wifiMulti.addAP(WIFI_1, WIFI_PSK);
  wifiMulti.addAP(WIFI_2, WIFI_PSK);
  wifiMulti.addAP(WIFI_3, WIFI_PSK);
  wifiMulti.addAP(WIFI_4, WIFI_PSK);
  
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(1000);
    Serial.print('.');
    i++;  // Increment timeout counter
    if (!startupFlag){
      // If the device has something on it that wants to start up when power applied
      if (i > 30){
        // If we've been trying to connect for more than 30 seconds...
        i = 0; // Reset timeout
        break; // Dump out of the loop
      }
    }
  }

  // Check if we're actually connected.
  if (wifiMulti.run() == WL_CONNECTED){
    wifiStillNeedsToConnect = 0;
    
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());              // Tell us what network we're connected to
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
  } else {wifiStillNeedsToConnect = 1;}
}
