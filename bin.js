#!/usr/bin/env node

var minimist = require('minimist')
var argv = minimist(process.argv.slice(2))
var hackrf = require('./')

var pulse = 0
var low = Infinity
var high = 0

d = hackrf.devices() // yolo gc fix

console.log('hackrf version is %s', d.getVersion())

function parse (f) {
  if (typeof f === 'number') return f
  f = f.replace(/M/i, '* 1000000')
  f = f.replace(/k/i, '* 1000')
  f = f.replace(/hz/, '')
  return eval('(' + f + ')') // yolo
}

console.log(parse(argv.frequency))
if (argv.frequency) d.setFrequency(parse(argv.frequency))
if (argv.bandwidth) d.setBandwidth(parse(argv.bandwidth))
if (argv['sample-rate']) d.setSampleRate(parse(argv['sample-rate']))

if (argv.startrx) {
  d.startRx(function (data) {
    var total = 0
    for(var i = 0; i < data.length; i++) total += data[i]
    if (total <= low) low = total
    if (total >= high) high = total
    var a = (total - low) / (high - low)
    console.log(new Array(Math.floor((a || 0) * 80)).join('#'))
  })
}

if (argv.starttx) {
  d.startTx(function (validBytes) {
    var b = new Buffer(validBytes)
    for (var i = 0; i < validBytes; i++) b[i] = pulse
    d.sendTx(b)
  })

  setInterval(function () {
    if (pulse === 0) pulse = 255
    else pulse = 0
    if (pulse) console.log('Sending pulse!')
    else console.log('Idling...')
  }, 1000)
}