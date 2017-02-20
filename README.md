# Beer Temperature Control
This project uses Arduino Pro Mini ([Atmega328P](http://www.atmel.com/Images/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf))
board. However, all sources are provided in AVR native C.

The goal is to provide accurate fermentation chamber temperature control (within 0.1 degree celsius). Similar
to brew pi, the complete system contains AVR microcontroller board running control algorithm. The SW on board
implements serial terminal for controller configuration and state reading, but the ultimate goal is to create
stand alone controller configurable via digital input elements (push button + incremental encoder) while state
reading will be possible via i2c lcd screen.

Although sharing similar system architecture to [brew pi](https://github.com/BrewPi/), this project is
completely independent and created from scratch.

Until all hardware elements are in place, Atmega will be controlled with raspberry pi via serial interface

## Directory Structure

* web_server - Contains sources for web server and services running on it
* rpi - Contains sources of the raspberry pi
* src - Contains sources for the AVR

## Board documentation

Board documentation can be found [here](https://www.arduino.cc/en/Main/arduinoBoardProMini).

Here is also direct link to [pdf schematics](https://www.arduino.cc/en/uploads/Main/Arduino-Pro-Mini-schematic.pdf).

## Installation

### Web Server

Installing SW on web server goes down to:

 - installing web page
 - installing handler for messages

To install web page, copy server_software/web_server contents into /var/www/html/beer to your favourite server.

To install handler for messages, copy server_software/apps/beermon_handler.py to server and try to run it.

It is possible you'll need some additional SW. To install necessary SW, execute (at least):

```
sudo apt-get update
sudo apt-get install python python3 mosquitto mosquitto-clients python-mosquitto python3-pip python-pip -y
sudo pip3 install paho-mqtt
sudo pip2 install paho-mqtt
```

After this, make sure your firewall is configured to receive on 1883

Once beermon_handler.py works, you can proceed with service installation. Copy beermon.service:

```
sudo cp beermon.service /etc/systemd/system/
sudo systemctl enable beermon.service
sudo systemctl start beermon
```

### RPi Software

RPi part is installed by executing rpi/beermon-rpi.py on rpi startup. For detailed instructions, check instructions within rpi sw directory.

## AVR Toolchain

These instruction assume that either USBasp or buspirate AVR programmer with avrdude are used for device programming.

### Windows
Install following tools

* [avr toolchain](http://www.atmel.com/tools/atmelavrtoolchainforwindows.aspx)
* [gnu make](http://fab.cba.mit.edu/classes/4.140/doc/projects/ftsmin/make-3.81.exe)
* [avrdude](http://fab.cba.mit.edu/classes/4.140/doc/projects/ftsmin/avrdude-win-64bit.zip)

and configure your PATH variable.

### Linux

Here will be given Linux instructions.

Install required packages:

sudo apt-get install avr-libc avrdude binutils-avr gcc-avr srecord

## Interfacing programmer

### Buspirate 3.6

| Buspirate   | Arduino Pro Mini |
|-------------|------------------|
|    GND      | GND              |
| +5V or +3V3 | Vcc              |
| CS          | RESET            |
| MOSI        | MOSI (11)        |
| MISO        | MISO (12)        |
| SCLK,CLK    | SCK (13)         |

## Building and downloading
Clone the repo, build the sources with _make_, and push _hex_ file
to board with following command.

### usbasp

```
$ avrdude -c usbasp -p m328p -u -U flash:w:beer_tc.hex
```

### buspirate:

Before any operations, learn port of the buspirate and set it:

```
port=/dev/ttyUSB0
```

To program device, execute:

```
sudo avrdude -c buspirate -P $port -p m328p -u -U flash:w:bin/beer_tc.hex
```

To power atmega from buspirate, execute:

```
echo "m 2 " > $port;echo "W" > $port
```

## Command set
When beer_tc is running on AVR board, the following commands can be issued via serial interface. all
commands are terminated with new line character.

### Get commands
Get commands come in following form

        get <object>

and the response is formated requested object type. The list of retrievable objects as well as belonging
types are given in following table.

### Set commands
Set commands come in following form.

        set <object> <value>

If execution is successful, command returns _ok_. If not the command fail reason string is returned.

### Command objects
While _get_ command is supported for all listed objects, only some can be updated with _set_ command.

| object | type      | r/w | description                                      | status |
|:------:|-----------|-----|--------------------------------------------------|--------|
| ts     | decimal   | rw  | controller set temperature                       | a      |
| t1     | decimal   | ro  | temperature reading for 1st sensor               | a      |
| t2     | decimal   | ro  | temperature reading for 2nd sensor               | a      |
| dt     | integer   | rw  | control loop discretization time (ms)            | n/a    |
| ct     | integer   | rw  | minimal output change interval duration (s)      | n/a    |
| un     | string    | rw  | unit system in use, i for imperial, m for metric | n/a    |
| fw     | string    | ro  | firmware version                                 | n/a    |
| id     | integer   | ro  | board id                                         | a      |