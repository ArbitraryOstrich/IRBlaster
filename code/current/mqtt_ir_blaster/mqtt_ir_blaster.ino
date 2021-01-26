#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <Arduino.h>
// #include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFi.h>


#include <Adafruit_BME280.h>
float temp_storage = 0;
float humid_storage = 0;
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
bool bme_found;



const char* mqttServerIp = "***REMOVED***";
const short mqttServerPort = 1883;
const char* mqttUsername = "***REMOVED***";
const char* mqttPassword = "***REMOVED***";
const char* mqttClientName = "***REMOVED***";

const char* mqtt_data_topic = "***REMOVED***/dht22";
const char* mqtt_command_topic = "***REMOVED***/command";
const char* mqtt_response_topic = "***REMOVED***/response";
const char* mqtt_log_topic = "***REMOVED***/log";
const char *willTopic = "***REMOVED***/status";
const char *willMessage = "offline";
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
// NTPClient timeClient(ntpUDP, "192.168.138.1", 3600, 60000);

String formattedDate;
String dayStamp;
String timeStamp;

void mqttLog(const char* str) {
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
  }else{
    // print to serial
    // also figure out a way to store. 
  }
}


void mqttConnect() {
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt_client.connect(mqttClientName, mqttUsername, mqttPassword, willTopic, willQoS, willRetain, willMessage)) {
      Serial.println("Connection Complete");
      Serial.println(willTopic);
      mqtt_client.publish(willTopic, "online", true);
      // empty out watever command is on the mqtt channel
      mqtt_client.publish(mqtt_command_topic, "");
      // Subcribe here. 
      mqtt_client.subscribe(mqtt_command_topic);
      if (wasConnected){
        mqttLog("Lost Connection to mqtt but now im back");
      }else{
        mqttLog("First Connection after boot");
      }
      wasConnected = 1;
      } else {
      Serial.print("failed, rc = ");
      Serial.print(mqtt_client.state());
      Serial.println("Trying again in 5 seconds");
      delay(5000);
    }
  }
}

void start_bme(){
  Serial.println("Attempting to start BME");  
  bme_found = bme.begin(0x76); 
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
  Serial.println("BME Started");
  Serial.println("/");
  irsend.begin();
  // dht.begin();
	delay(500);
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  char *payload_c_string = (char *) payload;
  char *topic_c_string = (char *) topic;
  DynamicJsonDocument doc(1024);
  timeClient.update();
  int epochDate = timeClient.getEpochTime();
  doc["t"] = epochDate;
  if (strcmp((char *) topic_c_string, mqtt_command_topic) == 0 ) {
    // Write over doc["n"] later if we actually do something w/ the command.
    doc["n"] = "Received Command but didn't understand";

    // Check if channel is first word of the command
    char buffer[10];
    strncpy(buffer, payload_c_string, 7);
    buffer[7] = '\0';   
    Serial.println(payload_c_string);
    if (strcmp((char *) buffer, "channel") == 0 ) {
      Serial.println("Received channel change command");
      char chan_buffer[5];
      chan_buffer[1] = payload_c_string[8]; 
      chan_buffer[2] = payload_c_string[9]; 
      chan_buffer[3] = payload_c_string[10]; 
      chan_buffer[7] = '\0'; 
      Serial.println(chan_buffer);
      for (int i = 1; i < 4; i++){
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
            doc["n"] = "Could not decode number string?";
            break;
        }
      }
      doc["n"] = chan_buffer;
    }




    /// TV Stuff
    if (strcmp((char *) payload_c_string, "sam_power") == 0 ) {
      irsend.sendSAMSUNG(0xE0E040BF, 32);
      doc["n"] = "sam_power";
    }
    if (strcmp((char *) payload_c_string, "sam_mute") == 0 ) {
      irsend.sendSAMSUNG(0xE0E0F00F, 32);
      doc["n"] = "sam_mute";
    }
    if (strcmp((char *) payload_c_string, "sam_source") == 0 ) {
      irsend.sendSAMSUNG(0xE0E0807F,32,2);
      doc["n"] = "sam_source";
    }
    if (strcmp((char *) payload_c_string, "sam_vol_down") == 0 ) {
      irsend.sendSAMSUNG(0xE0E0D02F,32,2);
      doc["n"] = "sam_vol_down";
    }
    if (strcmp((char *) payload_c_string, "sam_vol_up") == 0 ) {
      irsend.sendSAMSUNG(0xE0E0E01F,32,2);
      doc["n"] = "sam_vol_up";
    }
    if (strcmp((char *) payload_c_string, "ccast_to_sat") == 0 ) {
      irsend.sendSAMSUNG(0xE0E0807F,32,2);
      delay(1000);
      irsend.sendSAMSUNG(0xE0E0807F,32,2);
      doc["n"] = "Changing from Ccast To Sat";
    }
    /////
    /////
    /////
    ///// Shaw Utili
    /////
    /////
    ///// 
    if (strcmp((char *) payload_c_string, "shaw_power_raw") == 0 ) {
      irsend.sendRaw(shaw_power_raw, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_power_raw";
    }
    if (strcmp((char *) payload_c_string, "shaw_guide") == 0 ) {
      irsend.sendRaw(shaw_guide, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "Shaw Guide";
    }
    if (strcmp((char *) payload_c_string, "shaw_last_channel") == 0 ) {
      irsend.sendRaw(shaw_last_channel, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_last_channel";
    }
    if (strcmp((char *) payload_c_string, "shaw_go_back") == 0 ) {
      irsend.sendRaw(shaw_go_back, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_go_back";
    }
    if (strcmp((char *) payload_c_string, "shaw_enter") == 0 ) {
      irsend.sendRaw(shaw_enter, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_enter";
    }
    /////
    /////
    /////
    ///// Shaw Arrows
    /////
    /////
    /////
    if (strcmp((char *) payload_c_string, "shaw_arrow_down") == 0 ) {
      irsend.sendRaw(shaw_arrow_down, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_arrow_down";
    }
    if (strcmp((char *) payload_c_string, "shaw_arrow_up") == 0 ) {
      irsend.sendRaw(shaw_arrow_up, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_arrow_up";
    }
    if (strcmp((char *) payload_c_string, "shaw_arrow_left") == 0 ) {
      irsend.sendRaw(shaw_arrow_left, 27, 38);  // Send a raw data capture  at 38kHz.
      doc["n"] = "shaw_arrow_left";
    }
    if (strcmp((char *) payload_c_string, "shaw_arrow_right") == 0 ) {
      irsend.sendRaw(shaw_arrow_right, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_arrow_right";
    }
    /////
    /////
    /////
    ///// Shaw Numbers
    /////
    /////
    /////
    if (strcmp((char *) payload_c_string, "shaw_1") == 0 ) {
      irsend.sendRaw(shaw_1, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_1";
    }
    if (strcmp((char *) payload_c_string, "shaw_2") == 0 ) {
      irsend.sendRaw(shaw_2, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_2";
    }
    if (strcmp((char *) payload_c_string, "shaw_3") == 0 ) {
      irsend.sendRaw(shaw_3, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_3";
    }
    if (strcmp((char *) payload_c_string, "shaw_4") == 0 ) {
      irsend.sendRaw(shaw_4, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_4";
    }
    if (strcmp((char *) payload_c_string, "shaw_5") == 0 ) {
      irsend.sendRaw(shaw_5, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_5";
    }
    if (strcmp((char *) payload_c_string, "shaw_6") == 0 ) {
      irsend.sendRaw(shaw_6, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_6";
    }
    if (strcmp((char *) payload_c_string, "shaw_7") == 0 ) {
      irsend.sendRaw(shaw_7, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_7";
    }
    if (strcmp((char *) payload_c_string, "shaw_8") == 0 ) {
      irsend.sendRaw(shaw_8, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_8";
    }
    if (strcmp((char *) payload_c_string, "shaw_9") == 0 ) {
      irsend.sendRaw(shaw_9, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_9";
    }
    if (strcmp((char *) payload_c_string, "shaw_0") == 0 ) {
      irsend.sendRaw(shaw_0, 27, 38);  // Send a raw data capture at 38kHz.
      doc["n"] = "shaw_0";
    }

  }
  // Prep json
  char buffer[1024];
  size_t n = serializeJson(doc, buffer);
  mqtt_client.publish(mqtt_response_topic, buffer, n);
}



int loop_iter; 

void loop() {
  delay(50);
  if (!mqtt_client.connected()) {
   mqttConnect();
  }
  loop_iter = loop_iter + 1;
  // 2 seconds
  if (loop_iter == 40) {
    read_BME();
    loop_iter = 0;
  }
  mqtt_client.loop();


}
