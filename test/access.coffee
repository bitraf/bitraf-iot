
mqtt = require 'mqtt'
chai = require 'chai'
debug = require('debug')('bitraf-iot:test:access')

# Table of the different testcases
cases = [
  # Nothing should be able to open door except for keys for exactly this purpose
  { topic: '/bitraf/door/2floor/open', action: 'write', auth: 'door', permitted: true }
  { topic: '/bitraf/door/4floor/open', action: 'write', auth: 'door', permitted: true }
  { topic: '/bitraf/door/frontdoor/open', action: 'write', auth: 'door', permitted: true }

  { topic: '/bitraf/door/2floor/error', action: 'write', auth: 'door', permitted: true }
  { topic: '/bitraf/door/2floor/isopen', action: 'write', auth: 'door', permitted: true }
  { topic: '/bitraf/door/2floor/openuntil', action: 'write', auth: 'door', permitted: true }

  { topic: '/bitraf/door/2floor/open', action: 'write', auth: 'none', permitted: false }
  { topic: '/bitraf/door/4floor/open', action: 'write', auth: 'none', permitted: false }
  { topic: '/bitraf/door/frontdoor/open', action: 'write', auth: 'none', permitted: false }

  { topic: '/bitraf/door/2floor/error', action: 'write', auth: 'none', permitted: false }
  { topic: '/bitraf/door/2floor/isopen', action: 'write', auth: 'none', permitted: false }
  { topic: '/bitraf/door/2floor/openuntil', action: 'write', auth: 'none', permitted: false }

  # Disabled, currently prod unit sends all the time, confusing test
  # { topic: 'bitraf/guestbutton/lab/button', action: 'write', auth: 'none', permitted: false }
  # { topic: 'bitraf/guestbutton/lab/button', action: 'write', auth: 'door', permitted: true }

  # Msgflo discovery 
  # Disabled, currently prod unit sends all the time, confusing test
  #{ topic: 'fbp', action: 'write', auth: 'door', permitted: true }
  #{ topic: 'fbp', action: 'write', auth: 'none', permitted: true }

  # everyone should be able to use other topics
  { topic: '/public/fofofo', action: 'write', auth: 'none', permitted: true }
  { topic: 'role.PORT', action: 'write', auth: 'none', permitted: true } # msgflo default convention
]

# The authentication to use in tests
# Read in from BITRAF_IOT_AUTH_$NAME environment variable, on format user:pass
authentication =
  'none':
    password: undefined
    username: undefined
    skip: true
  'invalid':
    password: 'notapassword'
    username: 'invalidxxx'
    skip: true
  'door': null

importAuth = (auth) ->
  for name, v of auth
    continue if v?.skip
    envvar = "BITRAF_IOT_AUTH_#{name.toUpperCase()}"
    value = process.env[envvar]
    if value
      [ user, password ] = value.split(':')
      auth[name] =
        username: user
        password: password
    else
      throw new Error "Missing credentials for auth #{name}, set envvar #{envvar}"

  return auth

testAccess = (state, testcase) ->

  description = "#{testcase.action} to #{testcase.topic} with #{testcase.auth} auth"
  should = if testcase.permitted then "permitted" else "NOT permitted"
  describe description, ->
    client = null

    before (done) ->
      @timeout 4000
      auth = authentication[testcase.auth]
      options =
        username: auth.username
        password: auth.password      
      client = mqtt.connect brokerUrl, options
      connected = false
      client.once 'error', (err) ->
        debug 'client error', err
        return done err
      client.on 'connect', () ->
        done() if not connected
        connected = true

      # debugging
      client.on 'offline', () ->
        debug 'client offline'
      client.on 'close', () ->
        debug 'client close'
      client.on 'reconnect', () ->
        debug 'client reconnect'
      client.on 'error', () ->
        debug 'client error'

    after (done) ->
      client.end true, () ->
        return done()

    it should, (done) ->
      timeout = null
      gotReply = null
      runExpects = () ->
        chai.expect(gotReply).to.be.a 'boolean'
        if testcase.permitted
          chai.expect(gotReply, 'no reply').to.equal true
        else
          chai.expect(gotReply, 'got reply').to.equal false
        return done()

      sendMessage = 'XXX-' + description
      client.subscribe testcase.topic, (err) ->
        chai.expect(err).to.not.exist

        client.on 'message', (topic, msg) ->
          chai.expect(topic).to.equal testcase.topic
          chai.expect(msg.toString()).to.equal sendMessage
          clearTimeout timeout
          gotReply = true
          runExpects()

        timeout = setTimeout () ->
          gotReply = false
          runExpects()
        , 500

        client.publish testcase.topic, sendMessage, { qos: 0 }, (err) ->
          chai.expect(err).to.not.exist


brokerUrl = process.env.BITRAF_IOT_BROKER or 'mqtt://mqtt.bitraf.no'
describe 'Access control', () ->
  state =
    foo: null

  before () ->
    importAuth authentication

  after () ->
    return

  cases.forEach (c) ->
    testAccess state, c
