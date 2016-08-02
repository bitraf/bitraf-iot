# Bitraf's MQTT Protocol for Opening Doors

The client subscribes to `/bitraf/door/$door_id/open` where `door_id`
identifies the particular door. To open the door send an ascii string
of numbers indicating the number of seconds to delay before opening
the door. Any invalid package is silently ignored.

# Bitraf Deployment

The `bitraf-door-publisher` and `bitraf-door-subscriber` takes care of
implementing the protocol. They use these environment variables:

* `BROKER` - the MQTT broker to connect to.
* `MQTT_USERNAME`/`MQTT_PASSWORD` - the username and password to use
  when connecting to the broker. The publisher need to be
  authenticated.
* `DOOR` - the door to open.
* `DELAY` - the value of the message to publish.

The subscriber sets a client it to `bitraf-door-$DOOR_ID` to prevent
misconfiguration by having many subscribers for the same door.

# Installation

To install the applications, run `sudo make install`. The subscriber
is designed to run as a systemd service by copying
`bitraf-door-subscriber.template.service` to `/etc/systemd/system/`,
adjusting the configuration and run:

    systemctl daemon-reload
    systemctl enable bitraf-door-subscriber.service


# [TODO](./TODO.md)
