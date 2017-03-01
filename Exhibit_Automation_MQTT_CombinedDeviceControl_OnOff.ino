#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include "Secrets.h"
#define SECONDS 1000

// To change this code for a new device, make sure these lines below are correct,
// and then change the floor, room, and OTA hostname in Secrets.h, the topic is built off those.

// -----===== Begin Config Block =====-----
int deviceIsRelay = 0;
int deviceIsComputer = 1;
int deviceIsInfrared = 0;
// -----===== End Config Block =====-----

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long OTAUntilMillis = 0;
unsigned long now = 0;
unsigned long computerPowerOffByTimeout = 0;

char msg[150];
char DEVICE_TOPIC[150];

int delayTime = STARTUP_DELAY_SECONDS * SECONDS;
int value = 0;
int curQueryStat = 0;
int curState = 0;

int OTAReadyFlag = 0;
int startupFlag = 0;
int initMsgFlag = 0;
int computerPowerOffCheckingFlag = 0;
int computerNeedsToTurnBackOnFlag = 0;

void setup(void) {
  pinMode (D1, OUTPUT);
  pinMode (D5, INPUT);
  pinMode (A0, INPUT);
  digitalWrite(D1, LOW);

  snprintf(DEVICE_TOPIC, 150, "OSC/%s/%s/%s", FLOOR_F, ROOM_R, OTA_HOSTNAME);

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

  // To prevent device damage from starting back up too quickly after a power outage, reset, etc...
  if (!startupFlag && now > delayTime) {
    if (deviceIsComputer) {
      powerOnComputer();
    }
    if (deviceIsRelay) {
      powerOnRelay();
    }
  }

  // This block gives computer exhibits a two minute timeout and hard-resets them if they're frozen
  if (computerPowerOffCheckingFlag) {
    int curState = analogRead(A0);
    if (curState < 900) {
      // If computer turned off within the timeout
      computerPowerOffCheckingFlag = 0;

      if (computerNeedsToTurnBackOnFlag){
      // Then these two lines make it do the auto startup after delay in the main loop.
      delayTime = STARTUP_DELAY_SECONDS * SECONDS + now;
      startupFlag = 0;
      computerNeedsToTurnBackOnFlag = 0;
      }
    }

    if (now > computerPowerOffByTimeout) {
      hardPowerOffComputer();
            
      // Then these two lines make it do the auto startup after delay in the main loop.
      delayTime = STARTUP_DELAY_SECONDS * SECONDS + now;
      startupFlag = 0;
      computerPowerOffCheckingFlag = 0;
    }
  }

  snprintf (msg, 150, "%s's ESP8266 is now up. Device starting after startup delay of %ld seconds.", OTA_HOSTNAME, STARTUP_DELAY_SECONDS);
  if (!initMsgFlag) {
    client.publish(DEVICE_TOPIC, msg);
    initMsgFlag = 1;
  }
}
