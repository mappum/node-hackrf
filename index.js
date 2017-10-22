if (process.env.HACKRF_DEBUG) require('segfault-handler').registerHandler()

var hackrf = require('node-gyp-build')(__dirname)

module.exports = function () {
  var devices = hackrf.devices()
  devices.open = function (i) {
    var device = devices._open(i)
    return new API(device)
  }
  return devices
}

var API = function (device) {
  this.device = device
}

API.prototype.getVersion = function () {
  return this.device.getVersion()
}

API.prototype.setFrequency = function (n, cb) {
  if (typeof n !== 'number') throw new Error('Frequency should be a number')
  this.device.setFrequency(n, cb || noop)
}

API.prototype.setBandwidth = function (n, cb) {
  if (typeof n !== 'number') throw new Error('Bandwidth should be a number')
  this.device.setBandwidth(n, cb || noop)
}

API.prototype.setSampleRate = function (n, cb) {
  if (typeof n !== 'number') throw new Error('Sample rate should be a number')
  this.device.setSampleRate(n, cb || noop)
}

API.prototype.setLNAGain = function (n) {
  if (typeof n !== 'number') throw new Error('LNA gain should be a number')
  this.device.setLNAGain(n)
}

API.prototype.setAmpEnable = function (n) {
  this.device.setAmpEnable(n)
}

API.prototype.setAntennaEnable = function (n) {
  this.device.setAntennaEnable(n)
}

API.prototype.setVGAGain = function (n) {
  if (typeof n !== 'number') throw new Error('VGA gain should be a number')
  this.device.setVGAGain(n)
}

API.prototype.setTxGain = function (n) {
  if (typeof n !== 'number') throw new Error('Tx gain should be a number')
  this.device.setTxGain(n)
}

API.prototype.startRx = function (cb) {
  var self = this
  this.device.startRx(function (data) {
    cb(data, function () {
      self.device.endRx()
    })
  })
}

API.prototype.stopRx = function (cb) {
  this.device.stopRx(cb || noop)
}

API.prototype.startTx = function (cb) {
  var self = this
  var buf = new Buffer(0)
  this.device.startTx(function (max) {
    if (max > buf.length) buf = new Buffer(max)
    cb(max !== buf.length ? buf.slice(0, max) : buf, function () {
      self.device.endTx(buf)
    })
  })
}

API.prototype.stopTx = function (cb) {
  this.device.stopTx(cb || noop)
}

API.prototype.close = function (cb) {
  this.device.close(cb)
}

function noop () {}
