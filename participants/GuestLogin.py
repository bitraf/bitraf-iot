#!/usr/bin/env python

import sys, os, json, logging
sys.path.append(os.path.abspath("."))

import gevent
import msgflo

class Repeat(msgflo.Participant):
  def __init__(self, role):
    d = {
      'component': 'GuestLogin',
      'label': 'Trigger door opening from using button',
      'inports': [
        { 'id': 'in', 'type': 'bang', 'queue': 'bitraf/button/1' },
      ],
      'outports': [
        { 'id': 'openfront', 'type': 'int', 'queue': '/bitraf/door/frontdoor/open' },
        { 'id': 'opendownstairs', 'type': 'int', 'queue': '/bitraf/door/2floor/open' },
        { 'id': 'guestarriving', 'type': 'bang', 'queue': '/bitraf/guestlogin/arriving' },
      ],
    }
    msgflo.Participant.__init__(self, d, role)

  def process(self, inport, msg):
    print 'received', inport, msg.data

    frontdoor_time = 10 # seconds
    innerdoor_time = 60 # seconds

    self.send('openfront', frontdoor_time)
    self.send('opendownstairs', innerdoor_time)
    self.send('guestarriving', '!')
    print 'tried to open doors'

    self.ack(msg)
