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
import serial
from serial.tools import list_ports

# Remote mqtt broker address
MQTT_HOST="52.28.77.176"
MQTT_HOST_PORT=1883

# Local mqtt broker address
MQTT_LOCAL_HOST="localhost"
MQTT_LOCAL_PORT=1883

# Sensors read period, in seconds
sensor_read_period = 5

# Sensor list
sensors = ['t1', 't2', 'ts']

# configuration topic name - this is the name of the topic for
# configuration values. e.g. CONFIG_TOPIC/ts
CONFIG_TOPIC="beermon_config"

# Minimum and maximum temperature values
MIN_TEMP=5
MAX_TEMP=25

# Baud rate of the controller
BAUD_RATE=19200

# Supported board ids
supported_sensors_id = ['123456', 'tca']

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
    print("sending message: [" + topic + "]" + message)
    try:
        publish.single(topic, message, hostname=MQTT_HOST, port=MQTT_HOST_PORT, qos=1)
    except Exception as err:
        print("Exception %r while sending message {%s}, queue = %d" %(err, "[" + topic + "]" + message, queue))
        if queue == 1:
            print("Appending previous message to the message queue")
            message_queue.append([topic, message])
        return -1
    return 0

def send_message_local(message, topic):
    print("sending local message: [" + topic + "]" + message)
    try:
        publish.single(topic, message, hostname=MQTT_LOCAL_HOST, port=MQTT_LOCAL_PORT, qos=1, retain=True)
    except Exception as err:
        print("Exception %r while sending message {%s}" %(err, "[" + topic + "]" + message))
        return -1
    return 0

'''
Function sends command over serial and reads
back result. This function is thread safe

Function returns string with response or 'NOK' if
execution did not finished correctly
'''
def send_over_serial_read_result(command):
    with lock:
        print("Processing serial command '%s'" %(command,))
        command = command + "\n"
        try:
            ser.write(command.encode())
        except Exception as err:
            print("Got %r while writing command to serial port" %(err,))
            return "NOK"
        try:
            line = ser.readline()
        except Exception as err:
            print("Got %r while reading command output from serial port" %(err,))
            return "NOK"

        return line.decode(encoding="utf-8", errors="ignore")

'''
Function returns true if string s is representation
of float
'''
def is_float(s):
    try:
        float(s)
    except ValueError:
        return False
    return True

'''
Functioe reads value from sensor via INTERFACE
Function returns string with value, or NOK in
case sensor could not be read
'''
def read_sensor(sensor):
    value = send_over_serial_read_result("get " + sensor) 
    if is_float(value):
        return value
    else:
        return "NOK"

'''
Reads sensor's id
'''
def read_sensor_id():
    id = send_over_serial_read_result("get id")
    return id

'''
Function checks whether sensor is supported by the script
'''
def check_sensor_id(sensor_id):
    if sensor_id in supported_sensors_id:
        return True
    return False

def process_message(topic, text):
    print("Processing message: [%s] %s" %(topic, text))
    variable = topic.split('/')[-1]
    value = text
    if variable == "ts":
        if value == "?":
            curr_ts = read_sensor("ts")
            send_message_local(curr_ts, CONFIG_TOPIC + "/ts")
        else:
            print("Setting %s with %s" %(variable, value))

def beermon_handler_on_connect(client, userdata, rc):
    print("Connected with result code "+str(rc))
    print("Subscribing to " + CONFIG_TOPIC + "/# topic")
    client.subscribe(CONFIG_TOPIC + "/#")

def beermon_handler_on_message(client, userdata, msg):
    text = msg.payload.decode(encoding="utf-8", errors="ignore")
    process_message(msg.topic, text)

'''
Function iterates over all comports and tries to discover on which
port is controller connected
'''
def discover_controller():
    ports = list_ports.comports()
    # How many times to go over all ports to discover board
    max_loops=3
    for i in range(0, max_loops):
        for comport in ports:
            print("[%d] probing %s" %(i, comport.device))
            with serial.Serial(comport.device, BAUD_RATE, timeout=1, write_timeout=1) as ser:
                ser.write(b'bla')
                time.sleep(1);
                #response = ser.readline()
                #print("Got response %s on %s" %(response, comport.device))
    return None

if __name__ == "__main__":

    print("Starting " + os.path.basename(__file__))

    lock = threading.Lock()

    #port = discover_controller()
    port = "/dev/ttyUSB1"

    ser = serial.Serial(port, BAUD_RATE, timeout=1, write_timeout=1)

    sensor_id = read_sensor_id()
    print("Respononse: " + sensor_id)
    if not check_sensor_id(sensor_id):
        print("Sensor is not supported!")
    else:
        print("Sensor is supported")

    temp = read_sensor("t1")
    print("Sensor t1: " + temp)

    debug = True

    # Global variable for mqtt unsent message queue
    message_queue = deque(maxlen = 20000)

    msg_queue_thread = threading.Thread(target=msg_queue_worker,
                        name = "msg_queue_worker", args=())
    msg_queue_thread.setDaemon(True)
    msg_queue_thread.start()

    beermon_handler = mqtt.Client()
    beermon_handler.on_connect = beermon_handler_on_connect
    beermon_handler.on_message = beermon_handler_on_message
    beermon_handler.connect(MQTT_LOCAL_HOST, MQTT_LOCAL_PORT, 60)
    beermon_handler.loop_start()

    while True:
        for sensor in sensors:
            value = read_sensor(sensor)
            if value == "NOK":
                print("FAILED to read sensor " + sensor)
                continue
            date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            message = str(date) + "," + value
            print("Sending " + message)
            if debug:
                print("Debug - not sending")
            else:
                send_message_to_server(message, "beermon/" + sensor)
        time.sleep(sensor_read_period)
