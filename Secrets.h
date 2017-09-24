#define TOPIC_T "__full-MQTT-topic__"
#define OTA_HOSTNAME "__OTA-hostname__"

// Number of seconds on power applied before device actually turns on. For power outages, restarts, etc...
#define STARTUP_DELAY_SECONDS 900

#define wifiScanTag "____ScanTag____"
#define password "__wlan-password__"
#define mqtt_server "__MQTT-server-ip__"

uint16_t onData[__length-of-array__] = {__raw-ir-data-comma-separated-unsigned__};

uint16_t offData[__length-of-array__] = {__raw-ir-data-comma-separated-unsigned__};
