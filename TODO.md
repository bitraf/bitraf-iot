
# TODO

## Physical dooropener/status

* Visualize the state where frontdoor+lab is open differently from only lab open.
This to provide feedback when lab is (still) open, and hitting button to open front again.
Two levels of light (using PWM)? 
* Detect when doorbell rings and send on MQTT.
Either on `BUS` input to phone, or on the telefone speaker output.
* Make the doorbell ringing blink the guest login LED

## Error handling in door webinterface

* Webinterface should assert that door did in fact open successfully.
Could maybe use the Python wrapper code?

## Dynamic reconfiguration

* Fix Msgflo runtime/container not running

## More door data

* Add reed-switch sensors to each of the doors
* Log them to InfluxDB
* Combined with doorbell should be able to tell when/if people are let in
* In case of problems can be used together with authentication log to check access

## Radio

* Deploy a bridge from `MQTT <-> MPD`, for controlling music over network.
Minimally useful: Next/previous, now-playings

## Power turnoff

To ensure that dangerous equipment is off when people leave the lab,
we would like to have a single button (by the door) to turn their power off.

* Install standard 433Mhz power control for 230v on lab-benches, 3d-printers
* Deploy a two-way bridge `MQTT<->433MHz` bridge
* Create a controller with buttons for on/off

## Environmental data

* Particle size. Especially in laser-room, CNC room, woodworking room
[SDS11](https://www.banggood.com/Nova-PM-Sensor-SDS011-High-Precision-Laser-PM2_5-Air-Quality-Detection-Sensor-Module-p-1144246.html)
* Temperature+moisture, indoors and outdoors. DHT22

## Wanted MQTT services

* Soil moisture from plants, light/tempature, watering status
* Time/RTC broadcast
* TV: Show image
* TV: Show URL
* TV: Speak using text-to-speech
* TV: Trigger common sounds
* Room activity sensors using PIR
* Machine activity sensors. Using current sensing?
* Wind sensor outdoors.

