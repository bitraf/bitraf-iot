#!/usr/bin/env python

import sys, os, json, logging
sys.path.append(os.path.abspath("."))

import gevent
import msgflo

class Repeat(msgflo.Participant):
  def __init__(self, role):
    self.current_input = False

    d = {
      'component': 'GuestLogin',
      'label': 'Trigger door opening from using button',
      'inports': [
        { 'id': 'in', 'type': 'boolean', 'queue': 'bitraf/guestbutton/lab/button' },
      ],
      'outports': [
        { 'id': 'openfront', 'type': 'int', 'queue': '/bitraf/door/frontdoor/open' },
        { 'id': 'opendownstairs', 'type': 'int', 'queue': '/bitraf/door/2floor/open' },
        { 'id': 'guestarriving', 'type': 'bang', 'queue': 'bitraf/guestlogin/arriving' },
      ],
    }
    msgflo.Participant.__init__(self, d, role)

  def process(self, inport, msg):
    print 'received', inport, msg.data

    trigger = msg.data == True and not self.current_input
    if trigger:
        frontdoor_time = 10 # seconds
        innerdoor_time = 60 # seconds
        self.send('openfront', frontdoor_time)
        self.send('opendownstairs', innerdoor_time)
        self.send('guestarriving', '!')
        print 'tried to open doors'

    self.current_input = msg.data
    self.ack(msg)
