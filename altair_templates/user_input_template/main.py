
# This program uses the web app framework, Streamlit, to create a simple interface for the user to 
# start a ride instance (create a Thing on SmartWorks), input their information, and provide a 
# Thing for the ESP32/MCU to send data into


#SmartWorks API Documentation Link: https://openapi.swx.altairone.com/cloud/anything-db 
#Streamlit App for Real-Time Visualization 
#Streamlit Documentation Link: https://docs.streamlit.io/ 

#Imported Libraries 
# -. json: used to read JSON data from HTTP requests 
# -. pandas: converting raw history data into Pandas Dataframe for visualization and analysis 
# -. modules.smartworks_sdk: used for getting tokens, credentials, etc
# -. streamlit: the base for creating streamlit dashboard apps 
# -. requests: used for making HTTP requests/actions 
# -. time: used for measuring time 
# -. pandas.io.json (json_normalize): library for converting json data into a dataframe 
# -. dateutil(parser): helps in the conversion date time objects 

import json
import pandas as pd
import modules.smartworks_sdk as swx
import streamlit as st
import requests
import time
from pandas.io.json import json_normalize
from dateutil import parser
import matplotlib.pyplot as plt

#Altair Host Link 
API_HOST = 'https://api.swx.altairone.com'


#SmartWorks Space/App Credentials --> update with YOUR own credential information 
#ebike-demo------------------------------------------------------
CLIENT_ID = 'YOUR_ID' 
CLIENT_SECRET = 'YOUR_SECRET' 
PATH = "/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/?property:state=opened"
CREATE_PATH = "/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things"


#model schema in JSON format --> can be found in 'thing'
#ebike-demo--------------------------------
# model_schema = 	{"model": {
# 		"name": "user_model",
# 		"version": 1
# 	}}

model_schema = {"YOUR_SCHEMA"}

#Put SmartWorks Space and Category names 
# --> update with YOUR own Space and Category names
space_name = 'YOUR_SPACE_NAME'
category_name = 'YOUR CATEGORY NAME'


#initializing the containers 
header = st.container()
background = st.container()
start_ride = st.container()
user_input = st.container()
end_ride = st.container()


#Initialize Session State for UID
#Session state variables are stored between reruns and across multiple scopes
if "uid" not in st.session_state:
    st.session_state['uid'] = ""


with header: 
    st.title("Altair SmartWorks User-Input App")

with background: 
    st.header("About")
    st.write("The app will record biometric data and send it to SmartWorks IoT where the data can be stored and visualized...")


# Container for "starting ride"
# - sends HTTP request to create a new "thing" at determined space
# - assigns property "state" to "opened" and sets the MQTT password 
#       microcontroller will use SmartWorks function to find thing with "opened" state, connect, and send data to the thing
#       MQTT password set to "ebike" --> mqttPASSWORD in main.cpp file will need to be updated to match what is set here 


with start_ride:
    if st.button("Begin Your Ride"):
        #----Create a new ride/thing instance, set the property, "state" to "opened", and set MQTT Password-----
        token = swx.get_access_token(CLIENT_ID, CLIENT_SECRET, API_HOST)

        headers = {"Authorization": "Bearer " + token}
        
        body = model_schema #creates thing with this model schema
        
        response = requests.request("POST", API_HOST + CREATE_PATH, headers=headers, json=body)
        
        st.session_state['uid'] = response.json()['uid']

        st.write(st.session_state.uid)

        UPDATE_PATH = "/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/" + st.session_state.uid + "/properties" #include your Space and Category name

        state = {"state" : "opened"}

        state_response = requests.request("PUT", API_HOST + UPDATE_PATH, headers = headers,  json = state)

        # Change space & category name in the pw_update url endpoint 
      
        pw_update = "https://api.swx.altairone.com/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/" + st.session_state.uid + "/mqtt-credentials" #include your Space and Category name
        pw_payload = {"password": "YOUR PASSWORD"} #set to your own desired password

        response = requests.request("PUT", pw_update, headers = headers, json = pw_payload)
        
        swx.revoke_token(token, CLIENT_ID, CLIENT_SECRET, API_HOST)
        st.success("Fill Out Required Info Below")

        
#container for getting user input 
with user_input: 
    st.write("Let's Get Started! Please enter your information.")
    name = st.text_input("Enter your name", "Type Here ...")
    ride_type = st.selectbox("What type of ride are you going on?", ("Test Ride", "Leisure Ride", "Health"))
    age = st.text_input("Enter your age")

    if st.button('Submit'):
        result = name.title()

        token = swx.get_access_token(CLIENT_ID, CLIENT_SECRET, API_HOST)
        headers = {"Authorization": "Bearer " + token}
    
        UPDATE_PATH = "/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/" + st.session_state.uid + "/properties" #include your Space and Category name
        
        #sending the JSON payload to set Properties to the values stored
        username = {"username": result}
        ride_label = {"ride_type" : ride_type}
        age_data = {"age" : int(age)}

        #http requests to update Properties
        username_response = requests.request("PUT", API_HOST + UPDATE_PATH, headers = headers, json = username)
        ride_response = requests.request("PUT", API_HOST + UPDATE_PATH, headers = headers, json = ride_label)
        age_response = requests.request("PUT", API_HOST + UPDATE_PATH, headers = headers, json = age_data)
        
        swx.revoke_token(token, CLIENT_ID, CLIENT_SECRET, API_HOST)
        st.success("Get Ready to Start your Workout")



placeholder  = st.empty()

if st.button("See Stats!"):
    st.write("Start Up Your Sensors & microcontroller unit (ESP32)")
    token = swx.get_access_token(CLIENT_ID, CLIENT_SECRET, API_HOST)
    
    with placeholder:
        while True:
       

            prop_path = "https://api.swx.altairone.com/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/" + st.session_state.uid + "/properties" #include your Space and Category name

            headers = {"Authorization": "Bearer " + token}
            data_request = {}

            response = requests.request("GET", prop_path, headers=headers, json = data_request)
            
            col1, col2, col3 = st.columns(3)
            col1.metric("Ride Type", response.json()['ride_type'])
            col2.metric("Humidity", response.json()['content.humidity'])
            col3.metric("Temperature", response.json()['content.temperature'])
            
            if st.button("Stop Data Stream"):
                break

            time.sleep(2)

    swx.revoke_token(token, CLIENT_ID, CLIENT_SECRET, API_HOST)
             


with end_ride: 
    token = swx.get_access_token(CLIENT_ID, CLIENT_SECRET, API_HOST)

    if st.button("End Workout"):

        headers = {"Authorization": "Bearer " + token}

        UPDATE_PATH = "/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/" + st.session_state.uid + "/properties" #include your Space and Category name


        state = {"state": "closed"}

        state_response = requests.request("PUT", API_HOST + UPDATE_PATH, headers = headers, json = state)

        swx.revoke_token(token, CLIENT_ID, CLIENT_SECRET, API_HOST)