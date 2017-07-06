
# TODO

## Split door control in webinterface

* Update the sender script to take `DOOR` and maybe `DURATION` arguments
* Update door-web UI to have multiple buttons, and pass the DOOR argument

## Error handling in door webinterface

* Deploy fix for overeager post-condition in doorlocks
* Webinterface should assert that door did in fact open successfully.
Could maybe use the Python wrapper code?

## Physical dooropener/status

* Reduce the spam from the guestbutton, currently always sending every 100ms 
* Detect when doorbell rings and send on MQTT.
Either on `BUS` input to phone, or on the telefone speaker output.
* Make the doorbell ringing blink the guest login LED

## Dynamic reconfiguration

* Fix Msgflo runtime not running
* (Flowhub/Msgflo) Support [read-only access](https://github.com/msgflo/msgflo/issues/37)
* (Flowhub) Respect changes coming from runtime side

## Radio

* Deploy a bridge from `MQTT <-> MPD`, for controlling music over network.
Minimally useful: Next/previous, now-playings

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
