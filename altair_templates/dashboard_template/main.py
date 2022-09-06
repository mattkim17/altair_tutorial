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
# -. altair: visualization library for Streamlit app 
# -. PIL(Image): used for importing and embedding images into the Streamlit app

import json
import pandas as pd
import modules.smartworks_sdk as swx
import streamlit as st
import requests
import time
from pandas.io.json import json_normalize
from dateutil import parser
import altair as alt
from PIL import Image

#Altair Host Link 
API_HOST = 'https://api.swx.altairone.com'

#SmartWorks Space/App Credentials - Fill out your information below
#ebike-demo------------------------------------------------------
CLIENT_ID = 'YOUR_ID' 
CLIENT_SECRET = 'YOUR_SECRET' 
PATH = "/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/?property:state=opened"
CREATE_PATH = "/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things"


#Put in your SmartWorks Space and Category names
space_name = 'YOUR_SPACE_NAME'
category_name = 'YOUR_CATEGORY_NAME'

#initializing the containers 
header = st.container()
background = st.container()
user_input = st.container()
intro_pic = st.container()


#Initialize Session State for UID & other Metrics
#Session state variables are stored between reruns and across multiple scopes
if "uid" not in st.session_state:
    st.session_state['uid'] = ""

if "humidity" not in st.session_state:
    st.session_state['humidity'] = ""

if "temperature" not in st.session_state:
    st.session_state['temperature'] = ""


with header: 
    st.title("Altair SmartWorks Dashboard App")
    st.markdown("---") #creates a line

with background: 
    st.write("The app webpage will used to visualize the data received from the sensors!")

#Without using CSS/HTML coding, the image is centered using 3 st.columns and image placed in the 2nd slot 
with intro_pic:
    image = Image.open('iot.jpg') 
    col1, col2, col3 = st.columns([0.7, 6, 0.2])
    with col1:
        st.write(' ')

    with col2:
        st.image("iot.jpg")

    with col3:
        st.write(' ')

#Container where the user inputs the UID of interest 
with user_input: 
    UID = st.text_input(label = 'Input the UID of the ride')
    st.session_state['uid'] = UID 
    if UID != "":
        st.success("Continue...")

# Metric
#st.empty: used to contain a single element and is replaced after a rerun --> prevents from stacking graphs/metrics/elements
placeholder  = st.empty()
chart_holder = st.empty()

if st.button("See Stats!"):

    stop = st.button("Stop Data Stream", key = 1)
    token = swx.get_access_token(CLIENT_ID, CLIENT_SECRET, API_HOST)
    

    with placeholder:
        while True:
            
            # try-except : HTTP requests in a loop can return Error 404 occasionally
            # try-except utilized to circumvent that issue and only update when HTTP request is successful 
            try: 
                prop_path = "https://api.swx.altairone.com/spaces/YOUR_SPACE_NAME/categories/YOUR_CATEGORY_NAME/things/" + st.session_state.uid + "/properties" #include your Space and Category name

                headers = {"Authorization": "Bearer " + token}
                data_request = {}
                
                json_path = "https://api.swx.altairone.com/spaces/YOUR_SPACE_NAME/data?source%5B%5D=" + st.session_state.uid #include your Space and Category name



                response = requests.request("GET", prop_path, headers=headers, json = data_request)
                df_response = requests.request("GET", json_path, headers=headers, json = data_request)

                
                col1, col2, col3 = st.columns(3)

                new_H = response.json()['humidity']
                new_T = response.json()['temperature']

                # if-else block for calculating the delta in metrics
                if(st.session_state.temperature and st.session_state.humidity) == "":
                    delta_h = 0
                    delta_t = 0
                    st.session_state.humidity = new_H
                    st.session_state.temperature = new_T
                else:
                    delta_h = int(new_H) - int(st.session_state.humidity)
                    delta_t = int(new_T) - int(st.session_state.temperature)
                    st.session_state.humidity = new_H
                    st.session_state.temperature = new_T

                #visualizing/displaying the metrics in 3 columns 
                col1.metric("Ride Type", response.json()['ride_type'])
                col2.metric("Humidity", response.json()['humidity'], delta = delta_h)
                col3.metric("Temperature", response.json()['temperature'], delta = delta_t)
                
                #container for visualizing the data with graphs 
                with chart_holder:
                    df = pd.json_normalize(df_response.json()["data"])
                    
                    c = alt.Chart(df).mark_line().encode(
                    alt.X(r'content\.time_elapsed:O',
                    axis = alt.Axis(
                        labelOverlap = True
                    )), 
                    alt.Y(r'content\.humidity:Q'))
                    st.altair_chart(c, use_container_width=True)
                    

            except: 
                continue 
            
            #if button pressed --> break out of while loop
            
            if stop:
                break
            #delay 
            time.sleep(2)

    #revoke token to prevent disconnect "device"
    swx.revoke_token(token, CLIENT_ID, CLIENT_SECRET, API_HOST)
             






