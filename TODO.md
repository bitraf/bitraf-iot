
# TODO

## Dynamic reconfiguration

* (Msgflo) Show data running coming through connection
* (Flowhub) Respect changes coming from runtime side
* (Msgflo) Support [read-only access](https://github.com/msgflo/msgflo/issues/37)

## Stateful webinterface for door

* Deploy new [dlock13-msgflo](https://github.com/bitraf/dlock13/tree/master/dlock13-msgflo).
* Enable WebSocket in Mosquitto for webui
* Update webinterface to show `isopen` status from MQTT

## Physical dooropener/status

* Have a participant which reads pressed state from, and sends message to open the doors.
Should also have a 'guestarriving' port, and send on that. Analogous to 'memberlogin'.
* Wire button LED to show the door `isopen` state

## Radio

* Deploy a bridge from `MQTT <-> MPD`, for controlling music over network.
Minimally useful: Next/previous, now-playing

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
