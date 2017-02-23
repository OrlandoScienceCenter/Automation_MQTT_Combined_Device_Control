#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include "Secrets.h"
#define SECONDS 1000

// One of these has to be set. To change this code for a new device, make sure these
// lines are set correctly and then change the topic and device name in Secrets.h

int deviceIsRelay = 1;
int deviceIsComputer = 0;
int deviceIsInfrared = 0;
#define STARTUP_DELAY_SECONDS 1  // Number of seconds on power applied before device actually turns on. For power outages, restarts, etc...

// End config block

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long OTAUntilMillis = 0;
unsigned long now = 0;
long lastMsg = 0;
char msg[150];
int value = 0;
int curQueryStat = 0;
int OTAReadyFlag = 0;
int curState = 0;
int startup_flag = 0;
int initMsgFlag = 0;

void setup(void) {
  pinMode (D1, OUTPUT);
  pinMode (D5, INPUT);
  pinMode (A0, INPUT);
  digitalWrite(D1, LOW);

  Serial.begin(115200);

  wifiSetup();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop(void) {
  now = millis();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (now < OTAUntilMillis && OTAReadyFlag == 0) {
    wifiSetupOTA();
    OTAReadyFlag = 1;
  }
  if (now < OTAUntilMillis && OTAReadyFlag == 1) {
    ArduinoOTA.handle();
  }

  int delayTime = STARTUP_DELAY_SECONDS * SECONDS;
  // To prevent device damage from starting back up too quickly after a power outage, reset, etc...
  if (!startup_flag && now > delayTime) {
    if (deviceIsComputer) {
      powerOnComputer();
    }
    if (deviceIsRelay) {
      powerOnRelay();
    }
  }
  
  snprintf (msg, 150, "%s's ESP8266 is now up. Device starting after startup delay of %ld seconds.", OTA_HOSTNAME, STARTUP_DELAY_SECONDS);
  if (!initMsgFlag){client.publish(DEVICE_TOPIC, msg); initMsgFlag = 1;}
}
