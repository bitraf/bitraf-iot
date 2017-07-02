#!/usr/bin/env node
var msgflo = require('msgflo');
var path = require('path');
// Ensure we have NPM .bin dir in path
process.env.PATH = process.env.PATH + ':' + path.resolve(__dirname, 'node_modules/.bin');

var runtime = new msgflo.runtime.Runtime({
  broker: 'mqtt://localhost',
  host: 'localhost',
  port: 3569,
  ide: 'http://app.flowhub.io',
  library: 'package.json',
  componentdir: 'components',
  graph: 'graphs/main.json',
  ignore: [
    '2floor',
    '4floor',
    'frontdoor'
  ]
});
runtime.start(function (err, address, liveUrl) {
  if (err) {
    console.log(err);
    process.exit(1);
  }
  console.log('MsgFlo started on ' + address);
  console.log('Open in Flowhub: ' + liveUrl);
});
