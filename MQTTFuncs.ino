extern unsigned long OTAUntilMillis;
extern unsigned long now;
extern unsigned long computerPowerOffTimeout;

extern char msg[50];

extern int value;
extern int curQueryStat;

extern bool OTARdyFlag;
extern bool startupFlag;
extern bool computerPowerOffCheckingFlag;
extern bool computerNeedsToTurnBackOnFlag;

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  
  for (unsigned int i = 0; i < length; i++) {
    payloadStr += ((char)payload[i]);
  }

  Serial.println(payloadStr);

  // powerOff, powerOn, getStatus, startOTA, powerReset, resetESP

  // powerOff
  if (payloadStr.equals(F("powerOff"))) {
    digitalWrite(BUILTIN_LED, HIGH);   // Turn the LED off (HIGH is off)
    if (deviceIsRelay) {
      powerOffRelay();
    }
    if (deviceIsComputer) {
      powerOffComputer();
      
      computerPowerOffByTimeout = now + (120 * SECONDS);
      computerPowerOffCheckingFlag = 1;
      computerNeedsToTurnBackOnFlag = 0;
    }
    if(deviceIsInfrared){
      powerOffInfrared();
    }

    // powerOn
  } else if (payloadStr.equals(F("powerOn"))) {
    digitalWrite(BUILTIN_LED, LOW);  // Turn the LED on
    if (deviceIsRelay) {
      powerOnRelay();
    }
    if (deviceIsComputer) {
      powerOnComputer();
    }
    if(deviceIsInfrared){
      powerOnInfrared();
    }

    // getStatus
  } else if (payloadStr.equals(F("getStatus"))) {
    if (deviceIsComputer) {
      if (analogRead(A0) > 900) {
        curQueryStat = 1;
      } else {
        curQueryStat = 0;
      }
      snprintf (msg, 20, "PowerState: %i", curQueryStat);
      client.publish(TOPIC_T, msg);
    }
    if (deviceIsRelay) {
      snprintf (msg, 20, "PowerState: %i", curState);
      client.publish(TOPIC_T, msg);
    }
    if(deviceIsInfrared){
      snprintf (msg, 20, "PowerState: %i", curState);
      client.publish(TOPIC_T, msg);
    }

    // startOTA
  } else if (payloadStr.equals(F("startOTA"))) {
    client.publish(TOPIC_T, "OTA for 5m");
    OTAUntilMillis = now + 300 * SECONDS; // 5 minutes

    // powerReset
  } else if (payloadStr.equals(F("powerReset"))) {
    client.publish(TOPIC_T, "Power-cycling.");
    delay(500);

    if (deviceIsRelay) {
      powerOffRelay();
      // Then these two lines make it do the auto startup after delay in the main loop.
      delayTime = STARTUP_DELAY_SECONDS * SECONDS + now;
      startupFlag = 0;
    }

    if (deviceIsComputer) {
      powerOffComputer();
      
      computerPowerOffByTimeout = now + (120 * SECONDS);
      computerPowerOffCheckingFlag = 1;
      computerNeedsToTurnBackOnFlag = 1;
    }
    
    // resetESP
  } else if (payloadStr.equals(F("resetESP"))) {
    client.publish(TOPIC_T, "ESP8266 resetting");
    delay(500);
    ESP.restart();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Create a random client ID
    String clientId = F("ESPCli-");
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println(F("connected"));
      // Once connected, publish an announcement...
      client.publish(TOPIC_T, "hello world");
      // ... and resubscribe
      client.subscribe(TOPIC_T);
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
