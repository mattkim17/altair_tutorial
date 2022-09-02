/*
These functions' purpose is to find and store the UID and MQTT Username information of a "opened" ride 
inside a struct for future reference in the main file. The ESP32 utilizes HTTP requests to invoke a function
that was written in SmartWorks. The goal is acheived in 3 steps: 
1) Retrieve Access Token 
2) Use Access Token to authorize HTTP POST request to invoke "main" function
  - Store data in a struct and return that struct
3) Revoke Access Token

*/
#include <Arduino.h>
#include <wifi-mqtt.h>
#include <delay.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "credentials_func.h"


//--------------------------------------------------Initialize Vars -------------------------------------------------------------
extern const char* ca_cert; 
extern const char* networkSSID; // Complete with your network SSID (name)
extern const char* networkPASSWORD; // Complete with your network password (use for WPA, or use as key for WEP)

struct credentials creds; 

//--------------------------------------------------HTTPS Functions -------------------------------------------------------------

//--------------------------------------revoke token function-------------------------------------------
int revoke(const char* token, const char* certificate, const char* client_id, const char* client_secret){
    HTTPClient http;
    WiFiClient client;
    const char* server = "https://api.swx.altairone.com/oauth2/revoke";
    http.begin(server, certificate); //OAuth Token URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
      // Your Domain name with URL path or IP address with path
      http.begin(server, certificate);
  
      // Data to send with HTTP POST --> API_HOST + PATH
      String httpRequestData = "token=" + String(token) + "&client_id=" + String(client_id) + "&client_secret=" + String(client_secret);
      
      Serial.println(httpRequestData); 
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      // Send HTTP POST request 
      int httpResponseCode = http.POST(httpRequestData);
      
      Serial.print("HTTP Revoked Response: ");
      Serial.println(httpResponseCode);
     
      http.end(); //Free the resources

      return httpResponseCode; 
    
    }else{
      return 0; 
    }
}

//--------------------------------------invoke "main" function-------------------------------------------

credentials main_invoke(const char* certificate, const char* client_id, const char* client_secret){

    HTTPClient http;
    WiFiClient client;

    //-----------------------------Retrieving access token and storing it in the ESP32-------------------
    const char* tokenserver = "https://api.swx.altairone.com/oauth2/token";
    http.begin(tokenserver, certificate); //OAuth Token URL & Certificate to access HTTPS 
    int httpCode = http.GET();      //Make the request

    if (httpCode > 0) {
    
    http.begin(tokenserver, certificate); //server URL & Certificate to access HTTPS 
    
    String httpRequestData = "client_id=" + String(client_id) + "&client_secret=" + String(client_secret)\
    + "&grant_type=client_credentials&scope=data+thing+function+collection+category";
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Send HTTP POST request 
    int httpResponseCode = http.POST(httpRequestData);
 
    DynamicJsonDocument doc(500); //allocating memory --> https://arduinojson.org/v6/assistant/
    DeserializationError error = deserializeJson(doc, http.getString()); //deserialize JSON data and get access token

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      
    }
      Serial.print("Access Token: ");
      Serial.println(doc["access_token"].as<const char*>()); 

      //automatic token
      const char* token = doc["access_token"].as<const char*>(); 
      
      http.end(); //Free the resources
      
    
//-------------------------------------invoke main---------------------------------------
    
    Serial.println("Invoking Main Functions"); 
    HTTPClient http; //start new HTTP client

    // ebike-demo
    //  server path set to "main" function created in SmartWorks --> (https://openapi.swx.altairone.com/cloud/functions)
    const char* server = "https://api.swx.altairone.com/spaces/YOUR_SPACE_NAME/functions/YOUR_FUNCTION_NAME/invoke"; // Put in your own Space & function names

    http.begin(server, certificate);
    httpCode = http.GET();      //Make the request

    if (httpCode > 0) { //Check for the returning code
  
    // Your Domain name with URL path or IP address with path
    http.begin(server, certificate);

    
    // Data to send with HTTP POST --> API_HOST + PATH
    String httpRequestData = "{}";
    
    // Specify content-type header
    
    Serial.println("Bearer " + String(token));
    http.addHeader("Authorization", "Bearer " + String(token));
    
    // Send HTTP POST request 
    int httpResponseCode = http.POST(httpRequestData);
    String httpResponse = http.getString(); 

    DynamicJsonDocument resp(4096); //allocating memory --> https://arduinojson.org/v6/assistant/
 
    DeserializationError error = deserializeJson(resp, httpResponse); //deserialize JSON data and get access token

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      
    }

    //Storing UID & MQTT information inside Credentials struct
    const char* uid = resp["UID"].as<char*>();
    const char* mqtt_user = resp["MQTT_USERNAME"].as<char*>();

    creds.uid = uid;
    creds.mqtt_user = mqtt_user; 
    
    
    http.end(); //Free the resources

    Serial.print("Revoking token: ");
    Serial.println(token);
    revoke(token, certificate, client_id, client_secret); //added to revoke tokens

    return creds; 
    
  }
  else {
      Serial.println("WiFi Disconnected");
      
    }
  }
}
