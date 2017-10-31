#!/usr/bin/env python

import sys, os, json, logging
sys.path.append(os.path.abspath("."))

import gevent
import msgflo

class Repeat(msgflo.Participant):
  def __init__(self, role):
    self.frontdooropen = False
    self.labopen = False

    d = {
      'component': 'DoorStatus',
      'label': 'Show current status of door',
      'inports': [
        { 'id': 'labopen', 'type': 'boolean', 'queue': '/bitraf/door/2floor/isopen' },
        { 'id': 'frontdooropen', 'type': 'boolean', 'queue': '/bitraf/door/frontdoor/isopen' },
      ],
      'outports': [
        { 'id': 'out', 'type': 'boolean', 'queue': 'bitraf/guestbutton/lab/led' },
      ],
    }
    msgflo.Participant.__init__(self, d, role)

  def process(self, inport, msg):
    if inport == 'labopen':
        self.labopen = msg.data
    if inport == 'frontdooropen':
        self.frontdooropen = msg.data

    light = False
    if self.labopen and self.frontdooropen:
        light = { "animate": "onoff", "period": 250, "on": 250, "off": 200 }
    elif self.labopen:
        light = 250

    self.send('out', light)

    self.ack(msg)
