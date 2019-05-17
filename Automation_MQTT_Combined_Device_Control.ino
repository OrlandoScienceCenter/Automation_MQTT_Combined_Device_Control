// To change this code for a new device, make sure the config block in Secrets.h is correct.

// Note: If you have just uploaded code to this board via USB you will need to manually restart the board before OTA will behave. 
// This is usually not a big deal, but it's good to know if your board stays powered on by other means after you disconnect USB.
// Just hitting the restart button on the board before OTA will fix this.

// If your device is doing infrared emulation, you'll also need to change the on/off infrared codes in OnOffFuncs

// When adding a new infrared device, run getStatusPowerDump over MQTT and it will spit out the last 60 power readings
// Choose a number towards the high end, way way above what it spits out when the device is off
// And add that to the database at the top of if (deviceIsInfrared) below

#include <ESP8266WiFiMulti.h>
#include <IRremoteESP8266.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <IRsend.h>
#include "Secrets.h"

#define SECONDS 1000

// Pin assignments
#ifdef SONOFF
 #define RELAY_POSPIN 12             // Sonoff relay is on GPIO12
 #define INFRARED_LEDPIN 13          // Runs to the + leg of the IR LED
 #define COMPUTER_SWITCHSENSEPIN 14  // Runs to the wire attached to the computer power switch that isn't GND
#else
 #define INFRARED_LEDPIN D2          // Runs to the + leg of the IR LED
 #define COMPUTER_SWITCHSENSEPIN D5  // Runs to the wire attached to the computer power switch that isn't GND
 #define RELAY_POSPIN D1             // Runs to the control + pin on the relay
#endif

WiFiClient espClient;
PubSubClient client(espClient);
IRsend irsend(INFRARED_LEDPIN); //an IR led is connected with the + leg on INFRARED_LEDPIN
ESP8266WiFiMulti wifiMulti;  

  const int numReadings = 60;  // For power usage min/max

  unsigned long OTAUntilMillis = 0;
  unsigned long now = 0;
  unsigned long computerPowerOffByTimeout = 0;
  unsigned long infraredPowerOffByTimeout = 0;
  
  char msg[50];
  
  char TOPIC_T[100];

  unsigned int infraredTimeoutCtr = 0;
  unsigned int delayTime = STARTUP_DELAY_SECONDS * SECONDS;
  int value = 0;
  int curQueryStat = 0;
  int curState = 0;
  int getStatusPowerUsage[numReadings];
  int powerUsageCtr = 0;
  
  int lastCurState = 0;
      
  bool OTARdyFlag = 0;
  bool initMsgFlag = 0;
  bool computerPowerOffCheckingFlag = 0;
  bool wifiStillNeedsToConnect = 1;
  bool computerNeedsToTurnBackOnFlag = 0;
  bool infraredPowerOffCheckingFlag = 0;

  bool OTAReportFlag = 0;

void setup(void) {
  if (strcmp(OTA_HOSTNAME, "ARSandboxProjector") == 0){powerThreshold = 580;}
  if (strcmp(OTA_HOSTNAME, "SOSProjector1") == 0){powerThreshold = 600;}
  if (strcmp(OTA_HOSTNAME, "SOSProjector2") == 0){powerThreshold = 600;}
  if (strcmp(OTA_HOSTNAME, "SOSProjector3") == 0){powerThreshold = 600;}
  if (strcmp(OTA_HOSTNAME, "SOSProjector4") == 0){powerThreshold = 600;}
  if (strcmp(OTA_HOSTNAME, "TheraminProjector") == 0){powerThreshold = 50;}
  if (strcmp(OTA_HOSTNAME, "KTTheaterProjector") == 0){}
  if (strcmp(OTA_HOSTNAME, "WFTVMitsubishiTV") == 0){}
  if (strcmp(OTA_HOSTNAME, "ScienceLiveProjector") == 0){}
  if (strcmp(OTA_HOSTNAME, "KZSimOneProjector") == 0){powerThreshold = 50;}
  if (strcmp(OTA_HOSTNAME, "KZSimTwoProjector") == 0){powerThreshold = 570;}
  if (strcmp(OTA_HOSTNAME, "KZSimThreeProjector") == 0){powerThreshold = 570;}

  if (strcmp(OTA_HOSTNAME, "KZSimOneComputer") == 0){
    computerButtonStateReversed = 0; 
    pinMode (COMPUTER_SWITCHSENSEPIN, OUTPUT);
    digitalWrite (COMPUTER_SWITCHSENSEPIN, LOW);
  } // Due to running through relays (weird power problems happen with the ESP if 
  if (strcmp(OTA_HOSTNAME, "KZSimTwoComputer") == 0){
    computerButtonStateReversed = 1; // This one is apparently hooked up weird. Go figure
    pinMode (COMPUTER_SWITCHSENSEPIN, OUTPUT);
    digitalWrite (COMPUTER_SWITCHSENSEPIN, LOW);
  } // we don't do it that way) we have to turn on the relay instead of off
  if (strcmp(OTA_HOSTNAME, "KZSimThreeComputer") == 0){
    computerButtonStateReversed = 0;   
    pinMode (COMPUTER_SWITCHSENSEPIN, OUTPUT);
    digitalWrite (COMPUTER_SWITCHSENSEPIN, LOW);
  } // like we normally do to emulate the power button but all else works as normal.
  
  sprintf(TOPIC_T, "OSC/%s/%s/%s", FLOOR_NUM, ROOM_NAME, OTA_HOSTNAME);
  
  pinMode (A0, INPUT);
  pinMode (RELAY_POSPIN, OUTPUT);
  pinMode (INFRARED_LEDPIN, OUTPUT);
  pinMode (COMPUTER_SWITCHSENSEPIN, INPUT);
 
  digitalWrite(RELAY_POSPIN, LOW);

  for (int i = 0; i < numReadings; i++) {
    getStatusPowerUsage[i] = 0;
  }

  Serial.begin(115200);

  wifiSetup();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  irsend.begin();

  Serial.print(F("Topic set as: "));
  Serial.println(TOPIC_T);
  Serial.println(F("INIT DONE"));
  Serial.println(F("Note: If you have just uploaded code to this board via USB you will need to manually restart the board before OTA will behave."));
}

void loop(void) {
  now = millis();

  yield();
  ESP.wdtFeed();  

  // Connect to MQTT if we have Wi-Fi
  if (!wifiStillNeedsToConnect){
    if (!client.connected()) {reconnect();}
    client.loop();  
  }
  
  if(deviceIsInfrared){
    // This is all just to increment a counter if it reads a device that's on and decrement it if it's not seeing anything
    // This is because we're reading AC and it won't always show a read even if something's on, if we read as it crosses zero volts
    // We're doing it this way and not with millis because SRAM. curState is 1 if on, 0 if off.
    
    delay(3);  // No clue why, but it's unstable without this. Gives RC=-2 MQTT disconnect errors. Just fine with it. It's related to analogRead
    curState = analogRead(A0);
    
    getStatusPowerUsage[powerUsageCtr] = curState;
    powerUsageCtr += 1;

    if (powerUsageCtr >= numReadings) {
      powerUsageCtr = 0;
    }

    // You only need to put an entry here if it differs from 570, since the variable is set to 570 when initialized
    // if (OTA_HOSTNAME == "KTTheaterProjector"){powerThreshold = 570;}
    
    if (curState > powerThreshold){if (infraredTimeoutCtr < 5000){infraredTimeoutCtr+=200;}} else {if (infraredTimeoutCtr > 0){infraredTimeoutCtr--;}}

    if (infraredTimeoutCtr > 1900) {  // 1900 so it's off by 100 if we're jumping in increments of 200
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

  snprintf (msg, 50, "%s is up", OTA_HOSTNAME);
  if (!initMsgFlag) {
    client.publish(TOPIC_T, msg);
    initMsgFlag = 1;
  }

  // If we didn't connect, try again.
  if (wifiStillNeedsToConnect){wifiSetup();}
}
