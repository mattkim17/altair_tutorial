import requests
import pandas as pd
import json

############### GET CREDENTIALS USING UID###############
uid=[]

def loads_things(url, headers):
    global uid

    response=requests.get(url = url, headers = headers)
    things=json.loads(response.text)
    pd_things=pd.DataFrame(things['data'])
    uid=pd_things['uid']
    
############### GET CREDENTIALS READING CSV FILE ###############

title = []
clientId = []
ID=[]
clientSecret= []
mqttThingPwd=[]
mqttThingUser=[]

def read_csv(path):
    global title
    global clientId 
    global ID
    global clientSecret
    global mqttThingPwd
    global mqttThingUser
       
    credentials_pd = pd.read_csv(path, delimiter=",")

    title=credentials_pd["title"].to_list()
    clientId=credentials_pd["clientId"].to_list()
    clientSecret=credentials_pd["clientSecret"].to_list()
    mqttThingPwd=credentials_pd["mqttThingPwd"].to_list()
    mqttThingUser=credentials_pd["mqttThingUsername"].to_list()

