# Bitraf's MQTT => Nexa bridge

It uses WifiManager to start an access point called
`bitraf-xx:xx:xx:xx:xx`. When you connect to the AP your
phone/computer should open the configuration page as a captive portal.
If not try accessing http://192.168.4.1/. In the portal page you can
configure the access point to be used, mqtt server/credentials and
instance id. The instance id is used in the MQTT topic.

The configuration will be cleared and the portal will be started if
you press the flash button.

## MQTT messages

`bitraf/nexa/$INSTANCE_ID` is used as a prefix for all MQTT topics.
The device subscribes on `$prefix/cmd/+` for incoming commands. When
it receives a message on `$prefix/cmd/$id` with the payload `on` or
`off` it will send a message to the radio. The `$id` has to be a
sequence of exactly 26 `0` or `1` characters.

## Schematic

1. take a NodeMCU
1. Connect D0 to the 433MHz sender's DATA line.
1. ...
1. PROFIT!!

## Antenna

If you need extra range, this worked for us:
https://www.elektor.nl/Uploads/Forum/Posts/How-to-make-a-Air-Cooled-433MHz-antenna.pdf
