
# TODO

## General setup

* Put the TV files into this repository

## Dynamic reconfiguration

* (Msgflo) Show data running coming through connection
* (Msgflo) Remove connections when removing edge
* (Flowhub) Respect changes coming from runtime side
* (Msgflo) Support adding new participant in code
* (Flowhub+Msgflo) Network persist supports

## Stateful webinterface for door

* (msgflo-cpp) Add support for MQTT
* Make the MQTT interface include outputs `isopen` and `error`, using msgflo-cpp
* Enable WebSocket in Mosquitto for webui
* Update webinterface to show `isopen` status

## Physical status display

* Create a device that can show lights
* Wire it up to be

## Wanted MQTT services

* Soil moisture from plants, light/tempature, watering status
* Time/RTC broadcast
* TV: Show image
* TV: Show URL
* Room activity sensors using PIR
* Machine activity sensors. Using current sensing?
