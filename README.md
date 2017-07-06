# Bitraf's Internet of Things network

At [Bitraf](https://bitraf.no) hackerspace we run a "Internet of Things" network.

We use the [MQTT](https://en.wikipedia.org/wiki/MQTT) protocol,
with a broker running on `mqtt.bitraf.no` (default port, 1883).

# Live view

[OpenMCT dashboard with graphs of historical data](http://iot.bitraf.no:8080)

[Open live view of the network in Flowhub](http://iot.bitraf.no/live)

# Things

In use

* [Doorlocks](https://github.com/bitraf/dlock13)
* [Door openers](./singlebutton)
* [Tool lock](https://github.com/einsmein/bitraf-thelock)
* [Window open/close detection (Hall effect)](https://github.com/Poohma/IOT_Window_Hall_sensors)
* [Motion sensor (PIR)](./PirMotionSensor)

In development

* [Window openers/closers](https://github.com/apetrynet/altF4)
* [Machine on/off (electrical current)](https://github.com/slunke/onoffsensor)

# [TODO](./TODO.md)

# Developing

## Running locally

* Install and start a [Mosquitto](https://mosquitto.org/) message broker
* Install the Node.js dependencies of this project with `npm install`
* Install the Python dependencies of this project with `pip install -r requirements.pip`
* Start the MsgFlo broker with `MSGFLO_BROKER=mqtt://localhost npm start`

## Running with Docker

* Ensure you have a running Docker daemon
* Start the project with `docker-compose up`

Note: by default the MsgFlo coordinator and MQTT ports are only available on `localhost`. Edit the ports declarations in `docker-compose.yml` if you want to open them to the outside.

## Running [tests](./test)

    npm install # once
    npm test


