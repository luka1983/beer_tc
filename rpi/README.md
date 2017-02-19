# Beermon for Raspberry pi

This script serves as a control interface to the temperature
controller. Its task is to read current temperature, and
possibily other sensors, and send output via mqtt to the
predefined server. This part runs automatically and doesn't
need user's intervention.

The other part is setting certain values to the controller,
e.g. temperature. This is done via mqtt interface. For latest
topic names and messages, check the script. Here are current
configurable values:

 * beermon_config/ts - get/set controller's temperature

To interface with setting/getting use mqtt. To check last value,
subscribe to the topic/variable, e.g.:

```
$ mosquitto_sub -t beermon_config/ts
```

To query current value, send '?' to topic/variable:

```
$ mosquitto_pub -t beermon_config/ts -m "?"

$ mosquitto_sub -t beermon_config/ts
```

These messages have retain set, so whenever you subscribe, you will
get last value.

To set variable, send value to the topic/variable

```
$ mosquitto_pub -t beermon_config/ts -m "18.5"
```

## Installing the script in general

At the moment, script is started automatically on rpi by managment
sw from previous project. It can also be started via systemd, check
web_server for template if needed.

Requirements for the script are:

```
$ sudo apt-get update
$ sudo apt-get install python python3 mosquitto mosquitto-clients python-mosquitto python3-pip python-pip -y
$ sudo pip3 install paho-mqtt
$ sudo pip2 install paho-mqtt
$ sudo pip2 install pyserial
$ sudo pip3 install pyserial
```

