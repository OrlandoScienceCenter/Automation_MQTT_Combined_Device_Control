// --------------------============================== Begin Config Block ==============================--------------------

    // Device type, set only one of these to 1.
    const int deviceIsComputer = 0;          
    const int deviceIsRelay = 1;
    const int deviceIsInfrared = 0;

    // Number of seconds on power applied before device actually turns on. For power outages, restarts, etc... Due to Wi-Fi connection time, 15 is pretty short.
    #define STARTUP_DELAY_SECONDS 15
    
    // Set this to 0 if you want exhibit to start up on power applied/after a brownout
    bool startupFlag = 1;

    // If device is a projector, use "getStatusPowerDump" to see power levels over time and set powerThreshold IN THE SETUP FUNCTION of the code to slightly less than the max you regularly see. DO NOT CHANGE THIS HERE

    //#define SONOFF // Uncomment this if you're using a sonoff (Different pin mappings)

//////////////////////////////////////// FLOOR NUMBERS ////////////////////////////////////////

    const char FLOOR_NUM[] = "Floor1";

//////////////////////////////////////// ROOM NAMES ////////////////////////////////////////

    const char ROOM_NAME[] = "NameOfRoom";
     
//////////////////////////////////////// HOSTNAMES ////////////////////////////////////////

    const char OTA_HOSTNAME[] = "YourDeviceName";

// --------------------============================== End Config Block ==============================--------------------

#define mqtt_server "192.168.1.10" // URL will also work.

// Your Wi-Fi names. It will auto-pick the strongest.
#define WIFI_1 "SSID1"
#define WIFI_2 "SSID2"
#define WIFI_3 "Etc..."
#define WIFI_4 ""
#define WIFI_5 ""
#define WIFI_6 ""

#define WIFI_PSK "WPAPasswordGoesHere"

int powerThreshold = 100;      // If device is a projector, use "getStatusPowerDump" to see power levels over time and set powerThreshold IN THE SETUP FUNCTION of the code to slightly less than the max you regularly see. DO NOT CHANGE THIS HERE
int computerButtonStateReversed = 0; // Used for some models of computers that pull high when their switches are pressed instead of low. Also good for activating relays.