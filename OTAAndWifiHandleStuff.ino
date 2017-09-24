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
  const char * ssid = wifiSearch(wifiScanTag).c_str();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
}

String wifiSearch(char searchname[]) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
    return("ERROR: NO NETWORKS FOUND");
  }
  else
  {
    int curArrayLoc = 0;
    String  wifiNames[10];
    int wifiStrengths[10];
    for (int i = 0; i < n; ++i) {
      String str = WiFi.SSID(i);
      int str_len = str.length() + 1;
      char char_array[str_len];
      str.toCharArray(char_array,str_len);
      if(strncmp(char_array,searchname,sizeof(searchname)-1)==0){
        wifiNames[curArrayLoc] = str;
        wifiStrengths[curArrayLoc] = WiFi.RSSI(i);
        curArrayLoc++;
      }
    }
    Serial.println("Valid networks found");
    int maxRSSI = -1000;
    int maxLoc = -1;
    for (int i = 0; i < curArrayLoc; ++i){
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(wifiNames[i]);
      Serial.print(" (");
      Serial.print(wifiStrengths[i]);
      Serial.println(")dB");
      if(wifiStrengths[i] > maxRSSI) {
        maxLoc = i;
        maxRSSI = wifiStrengths[i];
      }
      delay(10);
    }
    Serial.print("STRONGEST WIFI IS: ");
    Serial.println(wifiNames[maxLoc]);
    return(wifiNames[maxLoc]);
  }
}
