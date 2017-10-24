Generic MQTT IOT device code for ESP8266

```
Dependencies:
  https://github.com/markszabo/IRremoteESP8266
  https://github.com/knolleary/pubsubclient
```

This code is meant to control the following:

Relays (SSR or conventional) Both hook up with D1 as a control pin.

Infrared remote emulation (You'll need the library above.)

Computer power button press emulation (Computer should be hooked up with computer GND shared with the ESP, 
  USB+ into A0 on the ESP, and the D5 spliced into the sense pin attached to the computer's power switch.) 
  There are two pins attached to a computer's power button, sense and GND. You want the one that doesn't read as GND.

```
Pins:
  Computer Control: 
    GND - USB - from computer
    A0 -- USB + from computer
    D5 -- Spliced into the sense wire side of the power button
  
  Relay Control:
    D1 -- Relay Control +
    GND - Relay Control -
```

To get started, add a file called Secrets.h to your sketch directory. It should look like:

```
#define TOPIC_T "Topic/Goes/Here/DeviceName"
#define OTA_HOSTNAME "Name_for_OTA_and_Such"

// Number of seconds on power applied before device actually turns on. For power outages, restarts, etc...
#define STARTUP_DELAY_SECONDS 30

#define ssid "Your_Wifi_SSID"
#define password "Your_Wifi_PSK"
#define mqtt_server "my.mqttserver.com" // Or whatever
```

And that's it. Set what device type you want to control via the global variables in the main sketch file and configure a delay time on startup.

Commands you can send to devices are: powerOff, powerOn, getStatus, startOTA, powerReset, resetESP

Most of these should be self-explanatory, getStatus returns 1 if on, 0 if the device is off.

startOTA sets up the ESP to be programmable OTA for five minutes. Note that if you start OTA but don't program it within the 5 minute timeout, the hostname will persist in your ports menu in the arduino IDE until you reset the ESP. This is good for keeping clutter in your ports menu down.

powerReset power-cycles the exhibit, leaving it off for whatever startup delay time was specified in the main arduino sketch
