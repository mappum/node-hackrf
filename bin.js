#!/usr/bin/env node

var minimist = require('minimist')
var argv = minimist(process.argv.slice(2))
var hackrf = require('./')
var d = hackrf()

var pulse = 0
var low = Infinity
var high = 0

console.log('hackrf version is %s', d.getVersion())

function parse (f) {
  if (typeof f === 'number') return f
  f = f.replace(/M/i, '* 1000000')
  f = f.replace(/k/i, '* 1000')
  f = f.replace(/hz/, '')
  return eval('(' + f + ')') // yolo
}

if (argv.frequency) d.setFrequency(parse(argv.frequency))
if (argv.bandwidth) d.setBandwidth(parse(argv.bandwidth))
if (argv['sample-rate']) d.setSampleRate(parse(argv['sample-rate']))
if (argv.lnaGain) d.setLNAGain(argv.lnaGain)
if (argv.vgaGain) d.setVGAGain(argv.vgaGain)
if (argv.txGain) d.setTxGain(argv.txGain)

if (argv.startrx) {
  d.startRx(function (data, cb) {
    var total = 0
    for(var i = 0; i < data.length; i++) total += data[i]
    if (total <= low) low = total
    if (total >= high) high = total
    var a = (total - low) / (high - low)
    if (a) a = Math.pow(a, 4)
    a = Math.max(0, a)
    console.log(new Array(Math.floor((a || 0) * 80)).join('#'))
    cb()
  })
}

if (argv.startrx) {
  d.startRx(function (data, cb) {
    process.stdout.write(data)
    cb()
  })
}

if (argv.starttx) {
  d.startTx(function (b, cb) {
    for (var i = 0; i < b.length; i++) b[i] = pulse
    cb()
  })

  setInterval(function () {
    if (pulse === 0) pulse = 127
    else pulse = 0
    if (pulse) console.log('Sending pulse!')
    else console.log('Idling...')
  }, 200)
}
