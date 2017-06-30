# Integrate fbp-spec (.yaml files) with Mocha test runner
fbpspec = require('fbp-spec')
runtime = 
  label: 'MsgFlo'
  description: ''
  type: 'msgflo'
  protocol: 'websocket'
  address: 'ws://localhost:3569'
  secret: 'not-secret-3335'
  id: 'e9c9fca9-c378-437b-98e4-97b556edbec1'
options = starttimeout: 15 * 1000
describe 'MsgFlo Participants', ->
  fbpspec.mocha.run runtime, './spec', options
  return
