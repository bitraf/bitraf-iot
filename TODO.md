
# TODO

## General setup

* coordinator: Lock down permissions. Dedicated user, use systemd sandboxing features

## Dynamic reconfiguration

* (Msgflo) Show data running coming through connection
* (Msgflo) Remove connections when removing edge
* (Flowhub+Msgflo) Network persist supports
* (Flowhub) Make component examples be for MsgFlo
* (Flowhub) Respect changes coming from runtime side
* (Msgflo) Support [read-only access](https://github.com/msgflo/msgflo/issues/37)

## Stateful webinterface for door

* Make the MQTT interface include outputs `isopen` and `error`, using msgflo-cpp
* Enable WebSocket in Mosquitto for webui
* Update webinterface to show `isopen` status

## Radio

* Deploy a bridge from MQTT to MPD, for controlling music over network

## Power turnoff

To ensure that dangerous equipment is off when people leave the lab,
we would like to have a single button (by the door) to turn their power off.

* Install standard 433Mhz power control for 230v on lab-benches, 3d-printers
* Deploy a two-way bridge `MQTT<->433MHz` bridge
* Create a controller with buttons for on/off

## Kickoff workshop

Documentation

* Generate HTML API docs with topic info from participant data
* Add sensor/actuator using ESP8266+Diller
* Add sensor/actuator using RPi/BB + msgflo-cpp
* How to wire together things using MsgFlo

## Physical status display for door

* Create a device that can show lights
* Wire it up to show the door `isopen` state

## Wanted MQTT services

* Soil moisture from plants, light/tempature, watering status
* Time/RTC broadcast
* TV: Show image
* TV: Show URL
* TV: Speak using text-to-speech
* TV: Trigger common sounds
* Room activity sensors using PIR
* Machine activity sensors. Using current sensing?
* Temperature+moisture, indoors and outdoors
* Wind sensor outdoors
