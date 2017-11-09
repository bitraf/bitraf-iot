#!/usr/bin/env python

import sys, os, json, logging
sys.path.append(os.path.abspath("."))

import gevent
import msgflo

class Repeat(msgflo.Participant):
  def __init__(self, role):
    self.current_input = False

    d = {
      'component': 'OpenDoor3floor',
      'label': 'Trigger door opening from using button',
      'inports': [
        { 'id': 'in1', 'type': 'boolean', 'queue': 'bitraf/openbutton/workshop/button' },
        { 'id': 'in2', 'type': 'boolean', 'queue': 'bitraf/openbutton/office3rdfloor/button' },        
      ],
      'outports': [
        { 'id': 'open3workshop', 'type': 'int', 'queue': '/bitraf/door/3workshop/open' },
        { 'id': 'open3office', 'type': 'int', 'queue': '/bitraf/door/3office/open' },
      ],
    }
    msgflo.Participant.__init__(self, d, role)

  def process(self, inport, msg):
    print 'received', inport, msg.data

    trigger = msg.data == True and not self.current_input
    if trigger:
        door_time = 20 # seconds
        self.send('open3workshop', door_time)
        self.send('open3office', door_time)
        print 'tried to open doors'

    self.current_input = msg.data
    self.ack(msg)
