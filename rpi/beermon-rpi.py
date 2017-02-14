#!/usr/bin/env python3

import syslog
import random
import datetime
import time
import threading
import array
import fcntl
import re
import subprocess
import os
import pwd
import sys
import shlex
import shutil
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import stat
from collections import deque

# Local mqtt broker address
MQTT_HOST="localhost"
MQTT_HOST_PORT=1883

# Sensors read period, in seconds
sensor_read_period = 30

# Sensor list
sensors = ['t1', 't2']

def msg_queue_worker():
    msg_worker_period = 30
    message_backoff_tmo = 5

    print("Starting message queue worker, worker period = %d, backoff = %d"
            %(msg_worker_period, message_backoff_tmo))

    while True:
        try:
            message = message_queue.popleft()
        except IndexError as err:
            # Empty queueu
            time.sleep(msg_worker_period)
            continue

        if not isinstance(message[0], str) or not isinstance(message[1], str):
            print("message_worker: Found invalid message in queue " + str(message))
            time.sleep(msg_worker_period)
            continue

        print("message_worker: Found message in the queue, sending")

        retval = send_message_to_server(message[1], message[0], 0)
        if retval != 0:
            message_queue.appendleft(message)
        else:
            # Sleep a bit, dont storm all mqtt messages to the server
            time.sleep(message_backoff_tmo)
            continue

        time.sleep(msg_worker_period)

'''
Sends mqtt message to MQTT_HOST
If message fails to be sent, it will be queued for later sending
    message - Full message to send
    topic   - topic to which to sent
    queueu  - if message fails, queue it to the message queue, default True
'''
def send_message_to_server(message, topic, queue=1):
    syslog.syslog(syslog.LOG_INFO, "sending message: [" + topic + "]" + message)
    try:
        publish.single(topic, message, hostname=MQTT_HOST, port=MQTT_HOST_PORT, qos=1)
    except Exception as err:
        print("Exception %r while sending message {%s}, queue = %d" %(err, "[" + topic + "]" + message, queue))
        if queue == 1:
            print("Appending previous message to the message queue")
            message_queue.append([topic, message])
        return -1
    return 0

'''
Function reads value from sensor via INTERFACE
This is obviously todo
'''
def read_sensor(sensor):
    value = random.uniform(15, 25)
    f_value = "{0:0.4f}".format(value)
    return f_value

if __name__ == "__main__":

    print("Starting " + os.path.basename(__file__))

    # Global variable for mqtt unsent message queue
    message_queue = deque(maxlen = 20000)

    msg_queue_thread = threading.Thread(target=msg_queue_worker,
                        name = "msg_queue_worker", args=())
    msg_queue_thread.setDaemon(True)
    msg_queue_thread.start()

    while True:
        for sensor in sensors:
            value = read_sensor(1)
            date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            message = str(date) + "," + value
            print("Sending " + message)
            send_message_to_server(message, "beermon/" + sensor)
        time.sleep(sensor_read_period)
