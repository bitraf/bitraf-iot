# To work in msgflo-nodejs source tree
try
  msgflo = require 'msgflo-nodejs'
catch e
  msgflo = require '..'

Participant = (client, role) ->

  definition =
    id: role
    component: 'WelcomeMessage'
    icon: 'file-word-o'
    label: 'Prepare a nice message for the person logging in'
    inports: [
      id: 'name'
      type: 'string'
    ]
    outports: [
      id: 'message'
      type: 'string'
    ]
  process = (inport, indata, callback) ->
    name = indata
    message = "Welcome to Bitraf, #{name}!"
    return callback 'message', null, message

  return new msgflo.participant.Participant client, definition, process, role

module.exports = Participant
