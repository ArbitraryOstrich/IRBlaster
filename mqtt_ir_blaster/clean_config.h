
float temp_storage;
float humid_storage;


#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
bool bme_found;

int polling_rate = 5; // Number of seconds between polling bme280 attempts
int loop_iter;

const char* mqttServerIp = "";
const short mqttServerPort = 1883;
const char* mqttUsername = "";
const char* mqttPassword = "";
const char* mqttClientName = "";
const char* mqtt_data_topic = "/bme280";
const char* mqtt_command_topic = "/command";
const char* mqtt_log_topic = "/log";
const char *willTopic = "/status";
const char *willMessage = "offline";
int mqtt_failed_connection_attempts;
int willQoS = 1;
int willRetain = 1;
bool wasConnected = 0;


char *ssid      = "";               // Set you WiFi SSID
char *password  = "";               // Set you WiFi password

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "", 3600, 60000);

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
