#!/usr/bin/env python

import sys, os, json, logging
sys.path.append(os.path.abspath("."))

import gevent
import msgflo

class Repeat(msgflo.Participant):
  def __init__(self, role):
    self.current_input = False

    d = {
      'component': 'DoorStatus',
      'label': 'Show current status of door',
      'inports': [
        { 'id': 'isopen', 'type': 'boolean', 'queue': '/bitraf/door/2floor/isopen' },
      ],
      'outports': [
        { 'id': 'out', 'type': 'boolean', 'queue': 'bitraf/guestbutton/lab/led' },
      ],
    }
    msgflo.Participant.__init__(self, d, role)

  def process(self, inport, msg):
    print 'received', inport, msg.data

    self.send('out', msg.data)

    self.ack(msg)
