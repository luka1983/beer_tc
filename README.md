# Beer Temperature Control
This project uses Arduino Pro Mini (Atmega328P) board. However, all sources are provided in AVR native C.

The goal is to provide accurate fermentation chamber temperature control (within 0.1 degree celsius). Similar
to brew pi, the complete system contains AVR microcontroller board running control algorithm. The SW on board
implements serial terminal for controller configuration and state reading, but the ultimate goal is to create
stand alone controller configurable via digital input elements (push button + incremental encoder) while state
reading will be possible via i2c lcd screen.

Although sharing similar system architecture to [brew pi](https://github.com/BrewPi/), this project is
completely independent and created from scratch.

## Board documentation

Board documentation can be found here: https://www.arduino.cc/en/Main/arduinoBoardProMini

Here is also direct link to pdf schematics: https://www.arduino.cc/en/uploads/Main/Arduino-Pro-Mini-schematic.pdf

## Installation
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

usbasp:
       $ avrdude -c usbasp -p m328p -u -U flash:w:beer_tc.hex

buspirate:
       $ sudo avrdude -c buspirate -P /dev/ttyUSB2 -p m328p -u -U flash:w:beer_tc.hex

## Command set
When beer_tc is running on AVR board, the following commands can be issued via serial interface. all
commands are terminated with new line character.

### Get commands
Get commands come in following form

        get <object>

and the response is formated requested object type. The list of retrievable objects as well as belonging
types are given in following table.

| object | type      | values                                           |
|:------:|-----------|--------------------------------------------------|
| ts     | decimal   | controller set temperature                       |
| tr     | decimal   | current temperature reading                      |
| dt     | integer   | control loop discretization time (ms)            |
| ct     | integer   | minimal output change interval duration (s)      |
| un     | character | unit system in use, i for imperial, m for metric |
| sv     | decimal   | software version                                 |


### Set commands
