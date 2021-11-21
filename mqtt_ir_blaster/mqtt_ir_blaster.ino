#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <Arduino.h>
#include <IRsend.h>
#include <WiFi.h>
#include <Adafruit_BME280.h>
#include "config.h"
#include "ir_commands.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "***REMOVED***", 3600, 60000);

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
    doc["msg"] = chan_buffer;
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
    irsend.sendSAMSUNG(0xE0E0807F,32);
    doc["msg"] = "sam_source";
  }
  if (r_doc["sam_vol_down"]) {
    irsend.sendSAMSUNG(0xE0E0D02F,32);
    doc["msg"] = "sam_vol_down";
  }
  if (r_doc["sam_vol_up"]) {
    irsend.sendSAMSUNG(0xE0E0E01F,32);
    doc["msg"] = "sam_vol_up";
  }
  if (r_doc["ccast_to_sat"]) {
    irsend.sendSAMSUNG(0xE0E0807F,32,2);
    doc["msg"] = "Samsung Source";
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
