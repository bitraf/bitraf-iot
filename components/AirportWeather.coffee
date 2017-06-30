msgflo = require 'msgflo-nodejs'
MetarFetcher = require('metar-taf').MetarFetcher
metarFetcher = new MetarFetcher
metarParser = require('metar')

getWeather = (station, callback) ->
  metarFetcher.getData(station)
  .then (data) ->
    clean = data.split("\n")[1]
    callback null, metarParser clean
  , (err) ->
    callback err

Participant = (client, role) ->
  station = null
  definition =
    id: role
    component: 'AirportWeather'
    icon: 'plane'
    label: 'Fetch weather data for an airport'
    inports: [
      id: 'icao'
      type: 'string'
      hidden: false
    ,
      id: 'fetch'
      type: 'bang'
      hidden: false
    ,
      id: 'temperature'
      type: 'float'
      hidden: true
    ,
      id: 'pressure'
      type: 'float'
      hidden: true
    ]
    outports: [
      id: 'temperature'
      type: 'float'
      hidden: false
    ,
      id: 'pressure'
      type: 'float'
      hidden: false
    ,
      id: 'error'
      type: 'object'
      hidden: false
    ]
  process = (inport, indata, callback) ->
    if inport in ['temperature', 'pressure']
      # Forward to outport
      return callback inport, null, indata
    unless inport in ['icao', 'fetch']
      return callback 'error', new Error "Unknown port name"
    if inport is 'icao'
      station = indata
    if inport is 'fetch' and station is null
      return callback 'error', new Error "No weather station provided"
    getWeather station, (err, weather) ->
      return callback 'error', err if err
      participant.send 'pressure', weather.altimeterInHpa
      callback 'temperature', null, weather.temperature

  participant = new msgflo.participant.Participant client, definition, process, role
  return participant

module.exports = Participant
