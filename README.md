# Beer Temperature Control
This project uses Arduino Pro Mini (Atmega328P) board. However, all sources are provided in AVR native C.

## Installation
These instruction assume that USBasp AVR programmer with avrdude is used for device programming.

### Windows
Install following tools

* [avr toolchain](http://www.atmel.com/tools/atmelavrtoolchainforwindows.aspx)
* [gnu make](http://fab.cba.mit.edu/classes/4.140/doc/projects/ftsmin/make-3.81.exe)
* [avrdude]()

and configure your PATH variable.

### Linux

## Building and downloading
Clone the repo, build the sources with _make_, and download _hex_ file
board with following command.

       $ avrdude -c usbasp -p m328p -u -U flash:w:beer_tc.hex
