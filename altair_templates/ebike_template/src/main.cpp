/*
The following libraries are needed:
-. Arduino.h: includes Arduino specific commands for coding
-. wifi-mqtt.h: allows to connect to your WIFI and to the MQTT broker. Also allow for publish and subscribe from it.
-. delay.h: allows to get the delay value from the platform
-. Blink.h: stores the logic to make the on-board LED blink
-. SPI.h: This library allows you to communicate with SPI devices, with the Arduino as the master device.
-. ESP8266WiFi.h: This library i sused to connect ESP8266 WiFi module to the wifi
-. PubSubClient.h: A client library for MQTT messaging.
-. DHT.h: Arduino library for DHT11, DHT22, etc Temp & Humidity Sensors
-. humidity.h: stores the logic to get data from humidity sensor
-. Adafruit_Sensor.h: library for reading sensor data (required for Adafruit ESP32 but may not be required for other boards)
-. WiFi.h: library for creating WiFi client to carry out HTTP requests 
-. credentials_func.h: allows for automatic token retrieval/revoking, grabs mqtt_username and UID of newly created "thing"
-. stopwatch.h: measures time elapsed since start of void/main loop function, will be used to measure time of ride/sensor readings --> later used as time axis in Panopticon
Reference: arduino.cc
*/

#include <Arduino.h>
#include <wifi-mqtt.h>
#include <delay.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <humidity.h>
#include <topics_humidity_publish.h>
#include <credentials_func.h>
#include <stopwatch.h>


//--------------------------------------------- DEFINE GLOBAL VARS --------------------------------------------------------
const char* networkSSID = ""; // Complete with your network SSID (name)
const char* networkPASSWORD = ""; // Complete with your network password (use for WPA, or use as key for WEP)
const char* mqttSERVER = "mqtt.swx.altairone.com"; 
const char* mqttUSERNAME; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username

//Additional Notes on mqttPASSWORD
// For this project, we will give the password within this file; however, the password will be set when "thing" is created through Streamlit
//Set the mqttPASSWORD to what you assign when you create a "thing"
const char* mqttPASSWORD = ""; //set your own password

//Your Space App Credentials: Access Control > Apps > your_app > Security Info
const char* client_id = "YOUR_ID" ;
const char* client_secret = "YOUR_SECRET";

// ca_cert is a certificate used for cybersecurity reasons and is needed to access HTTPS (HTTP vs. HTTPS) links 
// can be found in personal browser

const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"\
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"\
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"\
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"\
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"\
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"\
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"\
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"\
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"\
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"\
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"\
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"\
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"\
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"\
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"\
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"\
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"\
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"\
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"\
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"\
"-----END CERTIFICATE-----\n";

String publishTopic_datas;
String publishTopic_props;
String publishTopic_props1;
String uid; 

char publishHumidityTopic[500];
char publishHumidityTopic_data[500];
char t_arr[10];
char uid_cc[30];

String publishHumidityTopicSTR; 
String publishHumidityTopic_dataSTR; 


// setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(115200); // Open serial communications
  
  delay(4000);
  

  WiFi.begin(networkSSID, networkPASSWORD); //connecting to WiFi 

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
  
  // Initialize credentials struct to hold mqtt_user and uid information & invoke "main" function hosted on SmartWorks
  credentials creds;

  creds = main_invoke(ca_cert, client_id, client_secret); 
  mqttUSERNAME = creds.mqtt_user; 
  uid = creds.uid;

  Serial.print("UID: ");
  Serial.println(uid);
  Serial.print("MQTT USERNAME: ");
  Serial.println(mqttUSERNAME);

  Serial.print("mqttPASSWORD: ");
  Serial.println(mqttPASSWORD);

  Serial.print("Wifi Status: ");
  Serial.println(WiFi.status());
  
  WiFi.disconnect(); //added to disconnect this wifi shield

  wifi_mqtt_init(); // Initialize wifi connection and mqtt server
  mqtt_reconnect(); // Initialize MQTT connection

  init_delay(1000); //Set a default delay of 1000 miliseconds


}

// loop function runs over and over 
void loop() {
  //using the "UID" retrieved from "main" function --> auto-fill the data and property path for MCU to send data to 
  publishHumidityTopicSTR = "set/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/" + String(uid) + "/properties/humidity"; //property path
  publishHumidityTopic_dataSTR = "set/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/" + String(uid) + "/data"; //raw history data path

  //converting strings to char arrays 
  int data_size_h = publishHumidityTopic_dataSTR.length() + 1;
  char publishHumidityTopic_data[data_size_h];
  publishHumidityTopic_dataSTR.toCharArray(publishHumidityTopic_data, data_size_h);

  int prop_size_h = publishHumidityTopicSTR.length() + 1;
  char publishHumidityTopic[prop_size_h];
  publishHumidityTopicSTR.toCharArray(publishHumidityTopic,prop_size_h);

  //converting String uid to char uid_cc utilizing strcpy 
  strcpy(uid_cc, uid.c_str()); 

  //intialize and start stopwatch and give it char_array: t_arr to store time data as the following format - HH:MM:SS
  stopwatch(t_arr);

  //start function to send sensor & other data to SmartWorks
  humidity(publishHumidityTopic_data, publishHumidityTopic, uid_cc, t_arr); //humidity data

}