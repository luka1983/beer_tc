# Beermon-v2
This documents describes beermon-v2, beer monitoring web application as a part
of beer temperature monitoring and control solution.

This web application uses mqtt protocol as primary connection to server.

## Development prerequisites
You will need the following things properly installed on your computer.

* [Git](http://git-scm.com/)
* [Node.js](http://nodejs.org/) (with NPM)
* [Bower](http://bower.io/)
* [Ember CLI](http://ember-cli.com/)
* [PhantomJS](http://phantomjs.org/)

## Installation

		$ cd beermon_v2
		$ npm install
		$ bower install

## Running / Development

		$ ember serve
Visit your app at [http://localhost:4200](http://localhost:4200).


## Building
Development:
		$ ember build
Production:
		$ ember build --environment production

## Deploying
Specify what it takes to deploy your app.

## Appendix
Here, some additional instructions are given.

### WebSocket enabled MQTT broker
Since beermon components communicate data/commands with beermon server via
MQTT protocol, the WebSocket enabled MQTT broker is needed. Here are the
instructions for deployment of __mosquitto__ MQTT broker to server running
Debian Jessie Linux distribution. The version of __mosquitto__ broker needs
to be >=1.4 and since Jessie official repo includes older release.

		$ wget http://repo.mosquitto.org/debian/mosquitto-repo.gpg.key
		$ sudo apt-key add mosquitto-repo.gpg.key
		$ sudo apt-get update
		$ sudo apt-get install mosquitto

The content of example mosquitto configuration file _mosquitto_local.conf_
located in _/etc/mosquitto/conf.d/_ directory is given below:

		listener 1883
		listener 1884
		protocol websockets

Port 1884 is configured here to listen for ws connections. After confuguration
change, mosquitto broker restart is needed. It can be performed with:

		$ sudo /etc/init.d/mosquitto restart

