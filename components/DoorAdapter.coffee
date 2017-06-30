# To work in msgflo-nodejs source tree
try
  msgflo = require 'msgflo-nodejs'
catch e
  msgflo = require '..'

Participant = (client, role) ->

  definition =
    id: role
    component: 'DoorAdapter'
    icon: 'file-word-o'
    label: 'Convert door triggering to delay wanted'
    inports: [
      id: 'in'
      type: 'any'
    ]
    outports: [
      id: 'out'
      type: 'any'
    ]
  process = (inport, indata, callback) ->
    delay = 10
    return callback 'out', null, delay
  return new msgflo.participant.Participant client, definition, process, role

module.exports = Participant
