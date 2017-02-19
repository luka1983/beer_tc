#!/usr/bin/env python3

import syslog
import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import datetime
import threading
import subprocess
import os.path
from os import listdir
from datetime import timedelta
from datetime import datetime
import sys
import re

# Local mqtt broker address
MQTT_HOST="localhost"
MQTT_HOST_PORT=1883

RESULT_DIR="/var/www/html/beer/results/"

def process_message(topic, text):
    sensor = topic.split('/')[-1]
    date_value_pair = text.split(',')
    if date_value_pair[0] is not None:
        # Check whether date is formatted as date
        date_pattern = re.compile("2[0-9]{3}-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]")
        if date_pattern.match(date_value_pair[0]):
            date = date_value_pair[0]
        else:
            print("ERROR: Pattern on date doesnt match");
            return
    if date_value_pair[1] is not None:
        # Check that value matches decimal number
        value_pattern = re.compile("[\d\.\d]+")
        if value_pattern.match(date_value_pair[1]):
            value = date_value_pair[1];
        else:
            print("ERROR: Pattern on value doesnt match");
            return
    with open(RESULT_DIR + sensor, "a") as myfile:
        print("Writing %s to %s" %(text, RESULT_DIR + sensor))
        myfile.write(text + "\n")

def beermon_handler_on_connect(client, userdata, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("beermon/#")

def beermon_handler_on_message(client, userdata, msg):
    text = msg.payload.decode(encoding="utf-8", errors="ignore")
    print("Processing message: topic <%s>, text <%s>" %(msg.topic, text))
    process_message(msg.topic, text)

if __name__ == "__main__":

    print("Starting beermon handler")
    beermon_handler = mqtt.Client()
    beermon_handler.on_connect = beermon_handler_on_connect
    beermon_handler.on_message = beermon_handler_on_message
    beermon_handler.connect(MQTT_HOST, MQTT_HOST_PORT, 60)
    beermon_handler.loop_start()

    while True:
        print("Alive... ")
        time.sleep(600)
