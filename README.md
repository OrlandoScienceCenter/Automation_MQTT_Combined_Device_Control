Generic MQTT IOT device code for ESP8266

This code is meant to control the following:

Relays (SSR or conventional) Both hook up with D1 as a control pin.

Infrared remote emulation (Feature not implemented yet.)

Computer power button press emulation (Computer should be hooked up with computer GND shared with the ESP, 
  USB+ into A0 on the ESP, and the D5 spliced into the sense pin attached to the computer's power switch.) 
  There are two pins attached to a computer's power button, sense and GND. You want the one that doesn't read as GND.
	
	
To get started, add a file called Secrets.h to your sketch directory. It should look like:

```
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PSK";
const char* mqtt_server = "MQTT_SERVER_IP_OR_URL";

#define DEVICE_TOPIC "your/topic/here/DemoRelay1"
#define OTA_HOSTNAME "DemoRelay1"
```

And that's it. Set what device type you want to control via the global variables in the main sketch file and configure a delay time on startup.

Commands you can send to devices are: powerOff, powerOn, getStatus, startOTA, resetESP

Most of these should be self-explanatory, getStatus returns 1 if on, 0 if the device is off.

startOTA sets up the ESP to be programmable OTA for five minutes. Note that if you start OTA but don't program it within the 5 minute timeout, the hostname will persist in your ports menu in the arduino IDE until you reset the ESP. This is good for keeping clutter in your ports menu down.