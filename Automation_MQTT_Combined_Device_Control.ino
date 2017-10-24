#include <ESP8266WiFiMulti.h>
#include <IRremoteESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <IRsend.h>
#include "Secrets.h"

#define SECONDS 1000

// Pin assignments
#define INFRARED_LEDPIN D2          // Runs to the + leg of the IR LED
#define RELAY_POSPIN D1             // Runs to the control + pin on the relay
#define COMPUTER_SWITCHSENSEPIN D5  // Runs to the wire attached to the computer power switch that isn't GND

// To change this code for a new device, make sure these lines below are correct,
// and then change the topic and OTA Hostname in Secrets.h

// If your device is doing infrared emulation, you'll also need to change the on/off infrared codes in OnOffFuncs

// -----===== Begin Config Block =====-----
int deviceIsRelay = 0;
int deviceIsComputer = 0;
int deviceIsInfrared = 1;

bool startupFlag = 0; // Set this to 0 if you want exhibit to start up on power applied/after a brownout
// -----===== End Config Block =====-----

WiFiClient espClient;
PubSubClient client(espClient);
IRsend irsend(INFRARED_LEDPIN); //an IR led is connected with the + leg on INFRARED_LEDPIN
ESP8266WiFiMulti wifiMulti;  

  unsigned long OTAUntilMillis = 0;
  unsigned long now = 0;
  unsigned long computerPowerOffByTimeout = 0;
  unsigned long infraredPowerOffByTimeout = 0;
  
  char msg[50];

  unsigned int infraredTimeoutCtr = 0;
  unsigned int delayTime = STARTUP_DELAY_SECONDS * SECONDS;
  int value = 0;
  int curQueryStat = 0;
  int curState = 0;
  int lastCurState = 0;
    
  bool OTARdyFlag = 0;
  bool initMsgFlag = 0;
  bool computerPowerOffCheckingFlag = 0;
  bool wifiStillNeedsToConnect = 1;
  bool computerNeedsToTurnBackOnFlag = 0;
  bool infraredPowerOffCheckingFlag = 0;

void setup(void) {
  pinMode (A0, INPUT);
  pinMode (RELAY_POSPIN, OUTPUT);
  pinMode (INFRARED_LEDPIN, OUTPUT);
  pinMode (COMPUTER_SWITCHSENSEPIN, INPUT);
 
  digitalWrite(RELAY_POSPIN, LOW);

  Serial.begin(115200);

  wifiSetup();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  irsend.begin();
}

void loop(void) {
  now = millis();

  if (!wifiStillNeedsToConnect){
    if (!client.connected()) {
      reconnect();
    }
    client.loop();  
  }

  if(deviceIsInfrared){
    // This is all just to increment a counter if it reads a device that's on and decrement it if it's not seeing anything
    // This is because we're reading AC and it won't always show a read even if something's on, if we read as it crosses zero volts
    // We're doing it this way and not with millis because SRAM. curState is 1 if on, 0 if off.

    delay(2);  // No clue why, but it's unstable without this. Gives RC=-2 MQTT disconnect errors. Just fine with it. It's related to analogRead
    curState = analogRead(A0);   

    curState = map(curState, 506, 411, 0, 1024);
    if (curState > 512){if (infraredTimeoutCtr < 5000){infraredTimeoutCtr+=50;}} else {if (infraredTimeoutCtr > 0){infraredTimeoutCtr--;}}
    
    if (infraredTimeoutCtr > 2000) {
      curState = 1;
    } else {
      curState = 0;
    }

    // If state changed
    if (lastCurState != curState){ 
        if (curState == 0){                                  // If device turned off
           if (infraredPowerOffByTimeout <= now) {           // and the timeout is in the past (meaning we didn't set the timeout, meaning we didn't send a MQTT poweroff command.)
            snprintf (msg, 49, "CritErr: Device turned off unexpectedly", curState);
            client.publish(TOPIC_T, msg);
          }
        }
        
        lastCurState = curState; 
      }
  }

  if (now < OTAUntilMillis && OTARdyFlag == 0) {
    wifiSetupOTA();
    OTARdyFlag = 1;
  }
  if (now < OTAUntilMillis && OTARdyFlag == 1) {
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
    if(deviceIsInfrared){
      powerOnInfrared(); 
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

  if (infraredPowerOffCheckingFlag) {
    if (curState == 0) {
      // If device turned off within the timeout
      infraredPowerOffCheckingFlag = 0;
    }

    if (now > infraredPowerOffByTimeout) {
      client.publish(TOPIC_T, "CritErr: Device did not turn off within timeout");
      infraredPowerOffCheckingFlag = 0;
    }
  }

  snprintf (msg, 150, "%s's ESP8266 is up and subscribed", OTA_HOSTNAME);
  if (!initMsgFlag) {
    client.publish(TOPIC_T, msg);
    initMsgFlag = 1;
  }

  if (wifiStillNeedsToConnect){wifiSetup();}
}
