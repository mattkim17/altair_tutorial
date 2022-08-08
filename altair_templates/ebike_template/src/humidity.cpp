/* This file stores the logic to capture humidity data from the DHT11 sensor connected to Arduino board
The following libraries are needed:
-. Arduino.h: includes Arduino specific commands for coding
-. wifi-mqtt.h: allows to connect to your WIFI and to the MQTT broker. Also allow for publish and subscribe from it.
-. delay.h: allows to get the delay value from the platform
-. topics_humidity_publish.h: stores the topic to publish the data to
Reference: arduino.cc
*/
#include <Arduino.h>
#include <wifi-mqtt.h>
#include <delay.h>
#include <topics_humidity_publish.h>
#include <Adafruit_Sensor.h> //add this .h library if error appears in this file and main.cpp
#include <DHT.h>
#include <stopwatch.h>

#define DHTPIN 5  //set to correct GPIO pin value 
#define DHTTYPE DHT22 //choose correct DHT humidity sensor model (i.e. DHT11 vs. DHT22)
DHT dht(DHTPIN, DHTTYPE);


//Define the structure to send the data to the humidity property in the platform in json format.
char json_humidity_data[] = "{\"humidity\":\"%i\", \"temperature\":\"%i\", \"uid\":\"%s\", \"time_elapsed\":\"%s\"}";
bool test_measurement = false; 

void humidity(char data_path[], char prop_path[], char* uid_cc, char* time_array){
    dht.begin(); 

    if(!test_measurement){ //does not record the first sensor optimizing reading (gives incorrect readings)
        dht.readHumidity(); 
        dht.readTemperature(); 
        test_measurement = true; 
        Serial.println();
        Serial.println("Optimizing Sensor...");
        delay(2000);
    
    }else{

    int h = dht.readHumidity();
    int t = dht.readTemperature(); //pass "True" parameter for F temp

    char json_char_humidity[115]="";
    
    Serial.println();
    Serial.print("HUMIDITY.CPP: topic address: ");
    Serial.println(prop_path);
    Serial.print("HUMIDITY.CPP: topic DATA address: ");
    Serial.println(data_path);
    

    snprintf(json_char_humidity, sizeof(json_char_humidity), json_humidity_data, h, t, uid_cc, time_array);
    wifi_mqtt_send(json_char_humidity, data_path);// Publish a message to the raw history section
    wifi_mqtt_send(json_char_humidity, prop_path);// Publish a message to the property topic
  
    Serial.printf(json_humidity_data, h, t, uid_cc, time_array);
    delay(1000);
    }
}