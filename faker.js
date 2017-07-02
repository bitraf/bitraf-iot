#!/usr/bin/env node
var msgflo = require('msgflo');
var mqtt = require('mqtt');
var path = require('path');
var broker = process.env.MSGFLO_BROKER || 'mqtt://localhost';
// Ensure we have NPM .bin dir in path
process.env.PATH = process.env.PATH + ':' + path.resolve(__dirname, 'node_modules/.bin');

// Fake participants to register
var fakeSensors = [
  {"protocol": "discovery","command": "participant","payload": {"component": "bitraf/WindowSensor", "label": "", "icon": "", "id": "bitraf/windowsensor/workshop", "role": "bitraf/windowsensor/workshop", "outports": [{"id": "sensor1", "type": "boolean", "queue": "public/bitraf/windowsensor/workshop/sensor1"},{"id": "sensor2", "type": "boolean", "queue": "public/bitraf/windowsensor/workshop/sensor2"},{"id": "sensor3", "type": "boolean", "queue": "public/bitraf/windowsensor/workshop/sensor3"}],"inports": []}},
  {"protocol": "discovery","command": "participant","payload": {"component": "bitraf/PirMotionSensor", "label": "", "icon": "", "id": "public/motionsensor/lab", "role": "public/motionsensor/lab", "outports": [{"id": "motion", "type": "boolean", "queue": "public/motionsensor/lab/motion"}],"inports": []}},
  {"command": "participant", "payload": {"component": "DoorLock", "icon": "file-word-o", "id": "frontdoor", "inports": [{"id": "open", "queue": "/bitraf/door/frontdoor/open", "type": "int"}], "label": "Door lock for frontdoor", "outports": [{"id": "isopen", "queue": "/bitraf/door/frontdoor/isopen", "type": "boolean"}, {"id": "openuntil", "queue": "/bitraf/door/frontdoor/openuntil", "type": "int"}, {"id": "error", "queue": "/bitraf/door/frontdoor/error", "type": "string"}], "role": "frontdoor"}, "protocol": "discovery"},
  {"command": "participant", "payload": {"component": "DoorLock", "icon": "file-word-o", "id": "2floor", "inports": [{"id": "open", "queue": "/bitraf/door/2floor/open", "type": "int"}], "label": "Door lock for 2floor", "outports": [{"id": "isopen", "queue": "/bitraf/door/2floor/isopen", "type": "boolean"}, {"id": "openuntil", "queue": "/bitraf/door/2floor/openuntil", "type": "int"}, {"id": "error", "queue": "/bitraf/door/2floor/error", "type": "string"}], "role": "2floor"}, "protocol": "discovery"},
  {"command": "participant", "payload": {"component": "DoorLock", "icon": "file-word-o", "id": "4floor", "inports": [{"id": "open", "queue": "/bitraf/door/4floor/open", "type": "int"}], "label": "Door lock for 4floor", "outports": [{"id": "isopen", "queue": "/bitraf/door/4floor/isopen", "type": "boolean"}, {"id": "openuntil", "queue": "/bitraf/door/4floor/openuntil", "type": "int"}, {"id": "error", "queue": "/bitraf/door/4floor/error", "type": "string"}], "role": "4floor"}, "protocol": "discovery"},
  {"protocol": "discovery","command": "participant","payload": {"component": "bitraf/LedButton", "label": "", "icon": "", "id": "bitraf/guestbutton/lab", "role": "bitraf/guestbutton/lab", "outports": [{"id": "button", "type": "any", "queue": "bitraf/guestbutton/lab/button"}],"inports": [{"id": "led", "type": "boolean", "queue": "bitraf/guestbutton/lab/led"}]}},
  {"protocol":"discovery","command":"participant","payload":{"component":"TemperatureSensor1","label":"Measures temperature and humidity","icon":"thermometer-empty","inports":[],"outports":[{"queue":"bitraf/temperature/1","type":"number","description":"degrees Celcius","id":"temperature"},{"queue":"bitraf/humidity/1","type":"number","description":"Percentage","id":"humidity"}],"role":"templaboffice","id":"templaboffice"}},
  {"protocol":"discovery","command":"participant","payload":{"component":"TemperatureSensor2","label":"Measures temperature and humidity","icon":"thermometer-empty","inports":[],"outports":[{"queue":"bitraf/temperature/2/value","type":"number","description":"degrees Celcius","id":"temperature"},{"queue":"bitraf/humidity/2/value","type":"number","description":"Percentage","id":"humidity"}],"role":"temp4floor","id":"temp4floor"}},
  {"protocol":"discovery","command":"participant","payload":{"component":"TemperatureSensor3","label":"Measures temperature and humidity","icon":"thermometer-empty","inports":[],"outports":[{"queue":"bitraf/temperature/3/value","type":"number","description":"degrees Celcius","id":"temperature"}],"role":"tempoutside4th","id":"tempoutside4th"}}
]

var runtime = new msgflo.runtime.Runtime({
  broker: broker,
  host: 'localhost',
  port: 3569,
  ide: 'http://app.flowhub.io',
  library: 'package.json',
  componentdir: 'components',
  graph: 'graphs/main.json',
  runtimeId: 'bd43608-bf6e-495b-808d-96fdca7a7e89',
  ignore: [
    '2floor',
    '4floor',
    'frontdoor'
  ]
});

var connectFaker = function (callback) {
  var client = mqtt.connect(broker);
  client.once('connect', function () {
    callback(null, client);
  });
  client.once('error', function (err) {
    callback(err);
  });
};
var announceFakes = function (client) {
  fakeSensors.forEach(function (sensor) {
    client.publish('fbp', JSON.stringify(sensor));
  });
};

var previousSend = {};
var sendFakes = function (client) {
  fakeSensors.forEach(function (sensor) {
    sensor.payload.outports.forEach(function (port) {
      var prevKey = sensor.payload.id + ':' + port.id;
      var random = Math.random();
      if (port.type === 'boolean') {
        if (typeof previousSend[prevKey] === 'undefined') {
          previousSend[prevKey] = false;
        }
        var val = previousSend[prevKey];
        if (random < 0.3) {
          // 30% chance to flip the boolean
          val = val ? false : true;
        }
        client.publish(port.queue, JSON.stringify(val));
        previousSend[prevKey] = val;
        return;
      }
      if (port.type === 'int') {
        if (typeof previousSend[prevKey] === 'undefined') {
          previousSend[prevKey] = 20;
        }
        var val = previousSend[prevKey];
        if (random > 0.5) {
          val++
        } else {
          val--;
        }
        client.publish(port.queue, JSON.stringify(val));
        previousSend[prevKey] = val;
        return;
      }
      if (port.type === 'number') {
        if (typeof previousSend[prevKey] === 'undefined') {
          previousSend[prevKey] = 23.8;
        }
        var val = previousSend[prevKey];
        if (random > 0.5) {
          val += 0.01
        } else {
          val -= 0.01;
        }
        client.publish(port.queue, JSON.stringify(val));
        previousSend[prevKey] = val;
        return;
      }
    });
  });
};

connectFaker(function (err, client) {
  if (err) {
    console.log(err);
    process.exit(1);
  }
  setInterval(function () {
    sendFakes(client);
  }, 2000);
  setInterval(function () {
    announceFakes(client);
  }, 30000);
  setTimeout(function () {
    announceFakes(client);
  }, 1000);
  runtime.start(function (err, address, liveUrl) {
    if (err) {
      console.log(err);
      process.exit(1);
    }
    console.log('MsgFlo started on ' + address);
    console.log('Open in Flowhub: ' + liveUrl);
  });
});
