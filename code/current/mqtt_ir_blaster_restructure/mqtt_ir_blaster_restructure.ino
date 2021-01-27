#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <Arduino.h>
#include <IRsend.h>
#include <WiFi.h>
#include <Adafruit_BME280.h>

float temp_storage;
float humid_storage;


#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
bool bme_found;

int polling_rate = 5; // Number of seconds between polling bme280 attempts
int loop_iter; 

const char* mqttServerIp = "***REMOVED***";
const short mqttServerPort = 1883;
const char* mqttUsername = "***REMOVED***";
const char* mqttPassword = "***REMOVED***";
const char* mqttClientName = "***REMOVED***";
const char* mqtt_data_topic = "***REMOVED***/bme280";
const char* mqtt_command_topic = "***REMOVED***/command";
const char* mqtt_response_topic = "***REMOVED***/response";
const char* mqtt_log_topic = "***REMOVED***/log";
const char *willTopic = "***REMOVED***/status";
const char *willMessage = "offline";
int mqtt_failed_connection_attempts;
int willQoS = 1;
int willRetain = 1;
bool wasConnected = 0;


char *ssid      = "***REMOVED***";               // Set you WiFi SSID
char *password  = "***REMOVED***";               // Set you WiFi password

WiFiClient espClient;
PubSubClient mqtt_client(espClient);


const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

//captured by IRrecvDumpV2.ino
const uint16_t shaw_power_raw[27] = {4950,2002,966,1062,942,3016,914,3042,960,1070,940,1064,938,1092,940,1064,938,1092,940,1064,938,3020,964,1066,940,3016,940};
const uint16_t shaw_guide[27] = {4976,2000,968,1038,938,3018,992,2968,992,2988,944,3014,968,1038,964,1066,942,1060,966,1066,968,1038,962,2996,992,1036,968};
const uint16_t shaw_last_channel[27] = {4928,2024,942,1062,940,1090,942,1062,940,3018,964,2994,966,1062,942,1064,966,1062,942,1064,966,2988,968,3016,940,3016,940};
const uint16_t shaw_go_back[27] = {4956,1998,970,1034,962,2996,994,2964,994,1034,970,2988,936,1094,970,1036,938,1092,970,2988,970,2988,970,2988,970,2988,962};
const uint16_t shaw_enter[27] = {4956,1998,968,1036,992,1038,968,2990,968,2988,916,3042,964,1066,970,1034,962,1068,968,1036,962,2996,994,1036,970,1034,994};
const uint16_t shaw_arrow_down[27] = {4896,2052,938,1092,942,1064,938,1092,942,1062,916,1116,942,3016,942,1062,960,1072,942,1062,940,3018,966,1062,942,3016,942};
const uint16_t shaw_arrow_up[27] = {4956,1998,968,2990,970,2988,916,1114,970,1034,968,1036,994,2988,968,1036,958,1072,968,2988,996,2962,968,2988,938,3020,990};
const uint16_t shaw_arrow_left[27] = {4954,1998,970,2986,940,1092,968,2990,996,2960,968,2990,968,1036,994,1036,968,1038,994,2988,968,1036,940,1090,970,1034,938};
const uint16_t shaw_arrow_right[27] = {5046,1908,1088,2872,1058,948,1082,946,1058,948,1056,972,1088,918,1084,924,1078,950,1056,2900,1000,2958,1078,954,1082,942,1032};
const uint16_t shaw_1[27] = {4948,2026,942,3016,942,3016,942,3014,940,3018,966,1064,942,1062,964,1066,942,1062,964,1066,942,3014,944,1062,968,1062,942};
const uint16_t shaw_2[27] = {4956,1998,970,2988,970,1034,994,1038,968,1036,992,2966,994,1034,970,1036,994,1036,968,1036,994,2962,998,2986,970,1034,940};
const uint16_t shaw_3[27] = {4956,1998,970,1036,960,2998,994,1036,996,1008,992,2966,994,1036,970,1036,994,1034,968,2990,968,2988,964,1068,970,1034,962};
const uint16_t shaw_4[27] = {4972,1978,994,2964,992,2988,968,2990,968,1036,992,2966,992,1036,970,1036,994,1036,968,1038,994,1036,968,2988,968,2988,962};
const uint16_t shaw_5[27] = {4978,1998,968,2988,970,1036,994,1036,970,2988,970,2988,938,1092,968,1036,916,1114,996,2962,970,1034,964,2996,994,2964,994};
const uint16_t shaw_6[27] = {4956,1998,970,2988,968,2990,916,1114,968,2990,968,2988,970,1036,994,1036,968,1036,992,2966,994,2988,968,1036,938,3020,964};
const uint16_t shaw_7[27] = {4976,1998,968,1036,940,3018,994,1036,970,2988,970,1036,994,1034,968,1036,996,1034,970,2988,970,1034,996,2986,970,2988,968};
const uint16_t shaw_8[27] = {4974,2000,942,3014,942,1062,966,1064,942,3016,968,1038,992,1038,942,1062,992,1038,968,1038,992,1038,942,1064,992,2966,990};
const uint16_t shaw_9[27] = {4974,2000,968,1038,990,2968,994,2988,968,2990,968,1036,962,1068,968,1036,962,1068,968,2990,970,1036,992,1036,968,1036,992};
const uint16_t shaw_0[27] = {4978,1998,968,1036,960,1070,968,2988,970,1034,992,1038,970,2988,970,1036,1022,1008,970,1036,996,2960,996,2988,970,1036,962};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "***REMOVED***", 3600, 60000);


String formattedDate;
String dayStamp;
String timeStamp;

int mqttLog(const char* str) {
  if (mqtt_client.connected()){
    DynamicJsonDocument doc(256);
    timeClient.update();
    int mqtt_log_time = timeClient.getEpochTime();
    doc["time"] = mqtt_log_time;
    doc["msg"] = str;
    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    mqtt_client.publish(mqtt_log_topic, buffer, n);
    // mqtt_client.publish(mqtt_log_topic, str);
    return 1;
  }else{
    // print to serial
    // also figure out a way to store. 
    return 0;
  }
}
void mqttConnect() {
  Serial.print("Attempting MQTT connection...");
  char mqtt_log_message[256];
  if (mqtt_client.connect(mqttClientName, mqttUsername, mqttPassword, willTopic, willQoS, willRetain, willMessage)) {
      if (wasConnected == 1){
      // We where connected but lost connection for some reason.
      sprintf(mqtt_log_message, "Lost Connection to mqtt, attempted %d times to reconnect.", mqtt_failed_connection_attempts);
      }else{
      sprintf(mqtt_log_message, "Made first connection.");
      }
      mqttLog(mqtt_log_message);
      mqtt_failed_connection_attempts = 0;
      mqtt_client.publish(willTopic, "online", true);       
      Serial.println(willTopic);
      // empty out watever command is on the mqtt channel
      mqtt_client.publish(mqtt_command_topic, "");
      // ...
      // Subcribe here. 
      mqtt_client.subscribe(mqtt_command_topic);
      // ...
      mqtt_client.loop();
  }else{
      mqtt_failed_connection_attempts++;
      Serial.print("failed, rc = ");
      Serial.print(mqtt_client.state());
      Serial.println("Waiting for 5 seconds before trying again.");
      delay(5000);
  }
}
int start_bme(){
  Serial.println("Attempting to start BME");  
  bme_found = bme.begin(0x76); 
  if (bme_found == 1) {
      Serial.println("BME Started");
      Serial.print("Polling every ");
      int buff = polling_rate;
      Serial.print(buff);
      Serial.println(" Seconds");
      return 1;
  }else{
    Serial.println("Starting BME Failed");
    return 0;
  }
}
void read_BME(){
  float h = bme.readHumidity();
  float t = bme.readTemperature();
  float p = bme.readPressure() / 100.0F;

  DynamicJsonDocument doc(1024);
  if (isnan(h) || isnan(t)) {
    //Use the stored value instead
    doc["d_t"] = temp_storage;
    doc["d_h"] = humid_storage;
    doc["error"] = 1;

  } else {
    char humidity[15];
    char temp[15];
    char pressure[15];
    sprintf(temp, "%.1f", t);
    sprintf(humidity, "%.1f", h);
    doc["d_t"] = temp;
    doc["d_h"] = humidity;
    doc["d_p"] = p;
    doc["error"] = 0;
    temp_storage = t;
    humid_storage = h;
  }
  timeClient.update();
  int epochDate = timeClient.getEpochTime();
  doc["t"] = epochDate;
  char buffer[1024];
  size_t n = serializeJson(doc, buffer);
  mqtt_client.publish(mqtt_data_topic, buffer, n);
  mqtt_client.loop();
}
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  mqtt_client.setServer(mqttServerIp, mqttServerPort);
  mqtt_client.setCallback(callback);
  timeClient.begin();
  Serial.println("WiFi connected:");
  Serial.print(WiFi.localIP());
  start_bme();
  irsend.begin();
	delay(500);
}
void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<1024> r_doc;
  deserializeJson(r_doc, payload, length);
  timeClient.update();
  DynamicJsonDocument doc(1024);
  doc["rx_t"] = timeClient.getEpochTime();
  doc["msg"] = "Received Command but didn't understand";

  if (r_doc["BME_polling"]) {
    if (r_doc["BME_polling"].as<int>() != 0){
      polling_rate = r_doc["BME_polling"].as<int>();
      char mqtt_log_message[64];
      sprintf(mqtt_log_message, "Setting the polling rate at %d seconds" , polling_rate);
      doc["msg"] = mqtt_log_message;
    }
  }
  if (r_doc["channel"]){
    char chan_buffer[5];
    sprintf (chan_buffer, "%i", r_doc["channel"].as<int>());
    for (int i = 0; i < 3; i++){
      switch(chan_buffer[i]){
          case '1':
            irsend.sendRaw(shaw_1, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '2':
            irsend.sendRaw(shaw_2, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '3':
            irsend.sendRaw(shaw_3, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '4':
            irsend.sendRaw(shaw_4, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '5':
            irsend.sendRaw(shaw_5, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '6':
            irsend.sendRaw(shaw_6, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '7':
            irsend.sendRaw(shaw_7, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '8':
            irsend.sendRaw(shaw_8, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '9':
            irsend.sendRaw(shaw_9, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          case '0':
            irsend.sendRaw(shaw_0, 27, 38); 
            delay(100);
            Serial.println(chan_buffer[i]);
            break;
          default:
            doc["msg"] = "Could not decode number string?";
            break;
      }
    }
    doc["channel"] = chan_buffer;
  }
  if (r_doc["start_bme"]){
    int return_value = start_bme();
    if (return_value == 1) {
      doc["msg"] = "Started BME successfully";
    }else{
      doc["msg"] = "Failed to start BME";
    }
  }
  if (r_doc["sam_power"]){
    irsend.sendSAMSUNG(0xE0E040BF, 32);
    doc["msg"] = "sam_power";
  }
  if (r_doc["sam_mute"])  {
    irsend.sendSAMSUNG(0xE0E0F00F, 32);
    doc["msg"] = "sam_mute";
  }
  if (r_doc["sam_source"]) {
    irsend.sendSAMSUNG(0xE0E0807F,32,2);
    doc["msg"] = "sam_source";
  }
  if (r_doc["sam_vol_down"]) {
    irsend.sendSAMSUNG(0xE0E0D02F,32,2);
    doc["msg"] = "sam_vol_down";
  }
  if (r_doc["sam_vol_up"]) {
    irsend.sendSAMSUNG(0xE0E0E01F,32,2);
    doc["msg"] = "sam_vol_up";
  }
  if (r_doc["ccast_to_sat"]) {
    irsend.sendSAMSUNG(0xE0E0807F,32,2);
    delay(1000);
    irsend.sendSAMSUNG(0xE0E0807F,32,2);
    doc["msg"] = "Changing from Ccast To Sat";
  }
  ///// Shaw Utility
  if (r_doc["shaw_power_raw"]) {
    irsend.sendRaw(shaw_power_raw, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_power_raw";
  }
  if (r_doc["shaw_guide"]) {
    irsend.sendRaw(shaw_guide, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "Shaw Guide";
  }
  if (r_doc["shaw_last_channel"]) {
    irsend.sendRaw(shaw_last_channel, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_last_channel";
  }
  if (r_doc["shaw_go_back"]) {
    irsend.sendRaw(shaw_go_back, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_go_back";
  }
  if (r_doc["shaw_enter"]) {
    irsend.sendRaw(shaw_enter, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_enter";
  }
  ///// Shaw Arrows
  if (r_doc["shaw_arrow_down"]) {
    irsend.sendRaw(shaw_arrow_down, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_arrow_down";
  }
  if (r_doc["shaw_arrow_up"]) {
    irsend.sendRaw(shaw_arrow_up, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_arrow_up";
  }
  if (r_doc["shaw_arrow_left"]) {
    irsend.sendRaw(shaw_arrow_left, 27, 38);  // Send a raw data capture  at 38kHz.
    doc["msg"] = "shaw_arrow_left";
  }
  if (r_doc["shaw_arrow_right"]) {
    irsend.sendRaw(shaw_arrow_right, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_arrow_right";
  }
  ///// Shaw Numbers
  if (r_doc["shaw_1"]) {
    irsend.sendRaw(shaw_1, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_1";
  }
  if (r_doc["shaw_2"]) {
    irsend.sendRaw(shaw_2, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_2";
  }
  if (r_doc["shaw_3"]) {
    irsend.sendRaw(shaw_3, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_3";
  }
  if (r_doc["shaw_4"]) {
    irsend.sendRaw(shaw_4, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_4";
  }
  if (r_doc["shaw_5"]) {
    irsend.sendRaw(shaw_5, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_5";
  }
  if (r_doc["shaw_6"]) {
    irsend.sendRaw(shaw_6, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_6";
  }
  if (r_doc["shaw_7"]) {
    irsend.sendRaw(shaw_7, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_7";
  }
  if (r_doc["shaw_8"]) {
    irsend.sendRaw(shaw_8, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_8";
  }
  if (r_doc["shaw_9"]) {
    irsend.sendRaw(shaw_9, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_9";
  }
  if (r_doc["shaw_0"]) {
    irsend.sendRaw(shaw_0, 27, 38);  // Send a raw data capture at 38kHz.
    doc["msg"] = "shaw_0";
  }
  char buffer[1024];
  size_t n = serializeJson(doc, buffer);
  mqtt_client.publish(mqtt_log_topic, buffer, n);
  mqtt_client.loop();

}
void loop() {
  delay(50);
  //20 Iterations per second
  //No need to keep running if we fail the mqtt connection
  //Maybe in future we can store some of our readings
  if (!mqtt_client.connected()) {
   mqttConnect();
  }
  loop_iter = loop_iter + 1;
  if (bme_found == 1) {
    if (loop_iter == (polling_rate * 20)) {
      read_BME();
      loop_iter = 0;   
    }
  }
  mqtt_client.loop();


}
