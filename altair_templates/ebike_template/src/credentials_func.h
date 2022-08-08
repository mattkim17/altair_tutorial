#ifndef H_CREDENTIALS_FUNC_H
#define H_CREDENTIALS_FUNC_H
#include <Arduino.h> //allows for String type vars

//---------------------------------------Declare Global Variables---------------------------------------------------------- 

extern const char* networkSSID; // Complete with your network SSID (name)
extern const char* networkPASSWORD; // Complete with your network password (use for WPA, or use as key for WEP)
extern const char* mqttSERVER; 
extern const char* mqttUSERNAME; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username
extern const char* mqttPASSWORD; // In SmartWorks IoT platform, AnythingDB>your thing >Interfaces > MQTT Username

// const char* API_HOST = "https://api.swx.altairone.com";
// const char* updateps_func = "/spaces/ebike2/functions/updateproperties/invoke" ;
// const char* opentrack_path = "/spaces/ebike2/functions/opentracker3/invoke";
// const char* tokenserver = "https://api.swx.altairone.com/oauth2/token"; 

extern const char* client_id; //SmartWorks AppID
extern const char* client_secret; //SmartWorks App Secret PW
// const char* temp_token = "1Z-k8QAzHijmW1kG_fBPnzrthXlW6BEfDnKWLsHosdc.FPsKHZYCqCKhx0faq-UomU-1ZrG7H6rgGiBEhpdGEXY";
//char publishTopic_data[] = "set/ebike2/collections/esp32_data/things/"; //thing_id/data"

extern const char* ca_cert;

extern String uid;
extern String publishTopic_datas;
extern String publishTopic_props; 
extern String publishTopic_props1;
extern String mqtt_user; 


struct credentials{
  const char* mqtt_user; 
  const char* uid; 
};

extern struct credentials creds; 

//------------------------------------------Header Functions------------------------------------------------------------------
int revoke(const char* token, const char* certificate, const char* client_id, const char* client_secret); 
credentials main_invoke(const char* certificate, const char* client_id, const char* client_secret);


#endif