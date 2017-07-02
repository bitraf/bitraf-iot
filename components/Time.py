#!/usr/bin/env python
import msgflo
import gevent
import datetime

class Time(msgflo.Participant):
  def __init__(self, role):
    d = {
      'component': 'Time',
      'label': 'Send makerspace time every minute',
      'icon': 'clock-o',
      'inports': [
        { 'id': 'start', 'type': 'bang' },
      ],
      'outports': [
        { 'id': 'time', 'type': 'string' },
      ],
    }
    self.running = False
    msgflo.Participant.__init__(self, d, role)

  def process(self, inport, msg):
    if self.running:
      self.running = False
      self.ack(msg)
      return
    self.running = True
    gevent.Greenlet.spawn(self.loop)
    self.ack(msg)
    
  def loop(self):
    while self.running == True:
      self.send('time', datetime.datetime.now().isoformat())
      gevent.sleep(60)

if __name__ == '__main__':
  msgflo.main(Time)
