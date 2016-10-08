#!/bin/sh
mosquitto_sub -h mqtt.bitraf.no -t /bitraf/tv/showmessage | toilet -t --gay > /dev/console
