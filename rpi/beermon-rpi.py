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
sensor_read_period = 30

# Sensor list
sensors = ['t1', 't2', 'ts', 'co']

# configuration topic name - this is the name of the topic for
# configuration values. e.g. CONFIG_TOPIC/ts
CONFIG_TOPIC="beermon_config"

# Minimum and maximum temperature values
MIN_TEMP=5
MAX_TEMP=30

# Baud rate of the controller
BAUD_RATE=19200

# Supported board ids
supported_sensors_id = ['123456', 'tca']

# Max retries to try to read controller
max_retries = 3

def msg_queue_worker():
    msg_worker_period = 30
    message_backoff_tmo = 5

    print("Starting message queue worker, worker period = %d, backoff = %d"
            %(msg_worker_period, message_backoff_tmo))

    syslog.syslog(syslog.LOG_INFO, "Starting message queue worker, worker period = %d, backoff = %d"
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
            syslog.syslog(syslog.LOG_INFO, "message_worker: Found invalid message in queue " + str(message))
            time.sleep(msg_worker_period)
            continue

        print("message_worker: Found message in the queue, sending")
        syslog.syslog(syslog.LOG_INFO, "message_worker: Found message in the queue, sending")

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
    try:
        publish.single(topic, message, hostname=MQTT_HOST, port=MQTT_HOST_PORT, qos=1)
    except Exception as err:
        print("Exception %r while sending message {%s}, queue = %d" %(err, "[" + topic + "]" + message, queue))
        syslog.syslog(syslog.LOG_INFO, "Exception %r while sending message {%s}, queue = %d" %(err, "[" + topic + "]" + message, queue))
        if queue == 1:
            print("Appending previous message to the message queue")
            syslog.syslog(syslog.LOG_INFO, "Appending previous message to the message queue")
            message_queue.append([topic, message])
        return -1
    return 0


def send_message_local(message, topic):
    print("sending local message: [" + topic + "]" + message)
    syslog.syslog(syslog.LOG_INFO, "sending local message: [" + topic + "]" + message)
    try:
        publish.single(topic, message, hostname=MQTT_LOCAL_HOST, port=MQTT_LOCAL_PORT, qos=1, retain=True)
    except Exception as err:
        print("Exception %r while sending message {%s}" %(err, "[" + topic + "]" + message))
        syslog.syslog(syslog.LOG_INFO, "Exception %r while sending message {%s}" %(err, "[" + topic + "]" + message))
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
        command = command + "\n"
        try:
            ser.write(command.encode())
        except Exception as err:
            print("Got %r while writing command '%s' to serial port" %(err,command))
            syslog.syslog(syslog.LOG_INFO, "Got %r while writing command '%s' to serial port" %(err,command))
            return "NOK"
        try:
            line = ser.readline()
        except Exception as err:
            print("Got %r while reading command '%s' output from serial port" %(err,command))
            syslog.syslog(syslog.LOG_INFO, "Got %r while reading command '%s' output from serial port" %(err,command))
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
Function reads sensor from global interface and
returns string formatted as decimal value. Function
throws ValueError in case result obtained is not
decimal value.
Function will try to read max_attempts prior returning
failure.
'''
def read_sensor(sensor, max_attempts = max_retries):
    for i in range(0, max_retries):
        value = send_over_serial_read_result("get " + sensor) 
        if is_float(value):
            return value
        else:
            time.sleep(1)
            continue
    raise ValueError("Response is not decimal, communication FAILED!")

'''
Function writes command via global interface to the controller
and returns True if write was OK, or false if it wasnt.
Function will try to write max_attempts prior returning
failure.
'''
def write_controller(command, max_attempts = max_retries):
    for i in range(0, max_retries):
        response = send_over_serial_read_result(command) 
        if response == "ok":
            return True
        else:
            continue
    return False

'''
Function checks whether sensor is supported by the script
'''
def check_sensor_id(sensor_id):
    if sensor_id in supported_sensors_id:
        return True
    return False

def process_message(topic, text):
    print("Processing message: [%s] %s" %(topic, text))
    syslog.syslog(syslog.LOG_INFO, "Processing message: [%s] %s" %(topic, text))
    variable = topic.split('/')[-1]
    value = text
    if variable == "ts":
        if value == "?":
            curr_ts = read_sensor("ts")
            send_message_local(curr_ts, CONFIG_TOPIC + "/ts")
        else:
            if not is_float(value):
                print("Value to be set not recognized as float <%s>" %(value,))
                syslog.syslog(syslog.LOG_ERR, "Value to be set not recognized as float <%s>" %(value,))
                return
            ret = send_over_serial_read_result("set ts " + value) 
            print("Set %s with %s, retval = %s" %(variable, value, ret))
            syslog.syslog(syslog.LOG_INFO, "Setting %s with %s, retval = %s" %(variable, value, ret))
            # Refresh retained value and notify listeners
            # send_message_local(value, CONFIG_TOPIC + "/ts")

def beermon_handler_on_connect(client, userdata, rc):
    print("Connected with result code "+str(rc))
    syslog.syslog(syslog.LOG_INFO, "Connected with result code "+str(rc))
    print("Subscribing to " + CONFIG_TOPIC + "/# topic")
    syslog.syslog(syslog.LOG_INFO, "Subscribing to " + CONFIG_TOPIC + "/# topic")
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
    for i in range(0, max_retries):
        for comport in ports:
            print("[%d] probing %s" %(i, comport.device))
            syslog.syslog(syslog.LOG_INFO, "[%d] probing %s" %(i, comport.device))
            try:
                with serial.Serial(comport.device, BAUD_RATE, timeout=2, write_timeout=2) as ser:
                    command = "get id\n"
                    try:
                        ser.write(command.encode())
                    except Exception as err:
                        print("Got %r while writing command to serial port" %(err,))
                        syslog.syslog(syslog.LOG_INFO, "Got %r while writing command to serial port" %(err,))
                        continue
                    try:
                        line = ser.readline()
                    except Exception as err:
                        print("Got %r while reading command output from serial port" %(err,))
                        syslog.syslog(syslog.LOG_INFO, "Got %r while reading command output from serial port" %(err,))
                        continue
                controller_id = line.decode(encoding="utf-8", errors="ignore")
                print("[%d] %s: response to 'get id': '%s'" %(i, comport.device, controller_id))
                syslog.syslog(syslog.LOG_INFO, "[%d] %s: response to 'get id': '%s'" %(i, comport.device, controller_id))
                if check_sensor_id(controller_id):
                    print("[%d] %s: found controller, id = %s" %(i, comport.device, controller_id))
                    syslog.syslog(syslog.LOG_INFO, "[%d] %s: found controller, id = %s" %(i, comport.device, controller_id))
                    return comport.device
            except Exception as err:
                print("[%d] While opening serial port %s, %r occured" %(i, comport, err))
                syslog.syslog(syslog.LOG_INFO, "[%d] While opening serial port %s, %r occured" %(i, comport, err))
                # Give time to system to settle down, e.g. udev rules might still be firing
                time.sleep(5)

    return None

'''
function raises alarm
'''
def alarm(topic, message):
    publish.single("alarm/" + topic, message)

'''
Function reads inital ts and publishes it.
This should be called prior subscribing to the topic
for setting/getting values for ts due to reatin used there.
'''
def set_initial_ts():
    try:
        value = read_sensor("ts")
    except ValueError as err:
        return False
    send_message_local(value, CONFIG_TOPIC + "/ts")
    return True

if __name__ == "__main__":

    debug = False

    print("Starting %s, debug is %s" %(os.path.basename(__file__), str(debug)))
    syslog.syslog(syslog.LOG_INFO, "Starting %s, debug is %s" %(os.path.basename(__file__), str(debug)))

    lock = threading.Lock()

    port = discover_controller()
    #port = "/dev/ttyUSB1"
    if port is None:
        print("Failed to discover controller")
        syslog.syslog(syslog.LOG_INFO, "Failed to discover controller")
        alarm("tctr", "Failed to discover controller")
        print("Exiting due to previous errors")
        syslog.syslog(syslog.LOG_INFO, "Exiting due to previous errors")
        sys.exit(1)

    ser = serial.Serial(port, BAUD_RATE, timeout=1, write_timeout=1)

    if not set_initial_ts():
        print("Failed to set inital ts")
        syslog.syslog(syslog.LOG_INFO, "Failed to set inital ts")
        alarm("tctr", "FAILED to read sensor %s, errors in row %d" %(sensor, sens_errors[sensor]))
        print("Exiting due to previous errors")
        syslog.syslog(syslog.LOG_INFO, "Exiting due to previous errors")
        sys.exit(1)

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

    sens_errors = dict()
    for sensor in sensors:
        sens_errors[sensor] = 0

    while True:
        for sensor in sensors:
            try:
                value = read_sensor(sensor)
            except ValueError as err:
                sens_errors[sensor] = sens_errors[sensor] + 1
                print("FAILED to read sensor %s, errors %d" %(sensor, sens_errors[sensor]))
                syslog.syslog(syslog.LOG_INFO, "FAILED to read sensor %s, errors %d" %(sensor, sens_errors[sensor]))
                alarm("tctr", "FAILED to read sensor %s, errors %d" %(sensor, sens_errors[sensor]))
                print("Exiting due to previous errors")
                syslog.syslog(syslog.LOG_INFO, "Exiting due to previous errors")
                sys.exit(1)
            date = datetime.datetime.now()
            if date.second < 30:
                date = date.replace(second = 0)
            else:
                date = date.replace(second = 30)
            date_s = date.strftime("%Y-%m-%d %H:%M:%S")
            message = str(date_s) + "," + value
            print("Sending %s: %s" %(sensor, message))
            syslog.syslog(syslog.LOG_INFO, "Sending %s: %s" %(sensor, message))
            if debug:
                print("Debug - not sending")
                syslog.syslog(syslog.LOG_INFO, "Debug - not sending")
            else:
                send_message_to_server(message, "beermon/" + sensor)
        time.sleep(sensor_read_period)
