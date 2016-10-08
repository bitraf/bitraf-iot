#!/bin/sh
mosquitto_sub -h mqtt.bitraf.no -t /bitraf/tv/speakmessage | espeak
