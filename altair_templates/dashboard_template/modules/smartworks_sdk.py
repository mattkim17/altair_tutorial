import requests
import time
import json


def http_post(url, payload, headers, print_out=True):
    r_post = requests.post(url, data=json.dumps(payload), headers=headers)
    if (print_out):
        print("\n URL: \n {} \n VERB: POST  \n Headers: \n {} \n Payload \n {} \n RESPONSE: \n {}".format(
            url, headers, payload, r_post))
    return r_post.text


def http_put(url, payload, headers, print_out=True):
    r_put = requests.put(url, data=json.dumps(payload), headers=headers)
    if (print_out):
        print("\n URL: \n {} \n VERB: PUT \n Headers: \n {} \n Payload \n {} \n RESPONSE: \n {}".format(
            url, headers, payload, r_put))
    return r_put.text


def http_get(url, headers, print_out=True):
    r_get = requests.get(url, headers=headers)
    if (print_out):
        print("\n URL: \n {} \n VERB: GET \n Headers: \n {} \n RESPONSE: \n {}".format(
            url, headers, r_get))
    return r_get.text


def timestamp():
    now = time.localtime()
    hour = now.tm_hour
    minute = now.tm_min
    second = now.tm_sec
    print("\n{}h {}m {}s".format(hour, minute, second))


def get_smartworks_headers(ClientID, Secret, URL, scope):
    global token
    global CLIENT_ID
    global CLIENT_SECRET
    global API_HOST

    API_HOST = URL
    urlToken = URL + "/oauth2/token"
    CLIENT_ID = ClientID
    CLIENT_SECRET = Secret
    client_id_0 = ClientID.split("::")[0]
    client_id_1 = ClientID.split("::")[1]

    payload = 'client_id={}%3A%3A{}&client_secret={}&grant_type=client_credentials&scope={}'.format(
        client_id_0, client_id_1, Secret, scope)
    headers = {
        'Content-Type': 'application/x-www-form-urlencoded'
    }
    response = requests.request(
        "POST", urlToken, headers=headers, data=payload)

    try:
        token = json.loads(response.text)["access_token"]
        print("Successful token: ", token)
        headers = {
            "Authorization": "Bearer " + token,
            "Content-Type": "application/json"
        }
    except Exception:
        print("Token unsuccesfully retrieved")
        headers = False

    return headers


def get_access_token(CLIENT_ID, CLIENT_SECRET, API_HOST):
    # global CLIENT_ID
    # global CLIENT_SECRET
    # global API_HOST

    payload = f'grant_type=client_credentials&' \
              f'client_id={CLIENT_ID}&' \
              f'client_secret={CLIENT_SECRET}&' \
              f'scope=data+thing+function'

    headers = {'Content-Type': 'application/x-www-form-urlencoded'}
    response = requests.request("POST", API_HOST + "/oauth2/token",
                                headers=headers, data=payload)

    return response.json()['access_token']


def revoke_token(token, CLIENT_ID, CLIENT_SECRET, API_HOST):
    # global token
    # global CLIENT_ID
    # global CLIENT_SECRET
    # global API_HOST

    payload = f'token={token}&' \
              f'client_id={CLIENT_ID}&' \
              f'client_secret={CLIENT_SECRET}'

    headers = {'Content-Type': 'application/x-www-form-urlencoded'}
    response = requests.request("POST", API_HOST + "/oauth2/revoke",
                                headers=headers, data=payload)

    return response
