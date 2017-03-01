extern unsigned long OTAUntilMillis;
extern unsigned long now;
extern unsigned long computerPowerOffTimeout;

extern char msg[150];

extern int value;
extern int curQueryStat;
extern int OTAReadyFlag;
extern int startupFlag;
extern int computerPowerOffCheckingFlag;
extern int computerNeedsToTurnBackOnFlag;

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    payloadStr += ((char)payload[i]);
  }

  Serial.println(payloadStr);

  // powerOff, powerOn, getStatus, startOTA, powerReset, resetESP

  // powerOff
  if (payloadStr.equals("powerOff")) {
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

    // powerOn
  } else if (payloadStr.equals("powerOn")) {
    digitalWrite(BUILTIN_LED, LOW);  // Turn the LED on
    if (deviceIsRelay) {
      powerOnRelay();
    }
    if (deviceIsComputer) {
      powerOnComputer();
    }

    // getStatus
  } else if (payloadStr.equals("getStatus")) {
    if (deviceIsComputer) {
      if (analogRead(A0) > 900) {
        curQueryStat = 1;
      } else {
        curQueryStat = 0;
      }
      snprintf (msg, 20, "PowerState: %i", curQueryStat);
      client.publish(DEVICE_TOPIC, msg);
    }
    if (deviceIsRelay) {
      snprintf (msg, 20, "PowerState: %i", curState);
      client.publish(DEVICE_TOPIC, msg);
    }

    // startOTA
  } else if (payloadStr.equals("startOTA")) {
    client.publish(DEVICE_TOPIC, "Device is now OTA Programmable for the next 5 minutes");
    OTAUntilMillis = now + 300 * SECONDS; // 5 minutes

    // powerReset
  } else if (payloadStr.equals("powerReset")) {
    client.publish(DEVICE_TOPIC, "Device is now power-cycling whatever is attached.");
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
  } else if (payloadStr.equals("resetESP")) {
    client.publish(DEVICE_TOPIC, "Device is now resetting the ESP8266");
    delay(500);
    ESP.restart();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(DEVICE_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
