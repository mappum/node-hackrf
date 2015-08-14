var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var hackrf = require('bindings')('hackrf')

module.exports = function () {
  var api = {}
  d = api.device = hackrf.devices() // leak on purpose to fix gc thing

  api.getVersion = function () {
    return api.device.getVersion()
  }

  api.setFrequency = function (n) {
    if (typeof n !== 'number') throw new Error('Frequency should be a number')
    api.device.setFrequency(n)
  }

  api.setBandwidth = function (n) {
    if (typeof n !== 'number') throw new Error('Bandwidth should be a number')
    api.device.setBandwidth(n)
  }

  api.setSampleRate = function (n) {
    if (typeof n !== 'number') throw new Error('Sample rate should be a number')
    api.device.setSampleRate(n)
  }

  api.setLNAGain = function (n) {
    if (typeof n !== 'number') throw new Error('LNA gain should be a number')
    api.device.setLNAGain(n)
  }

  api.setVGAGain = function (n) {
    if (typeof n !== 'number') throw new Error('VGA gain should be a number')
    api.device.setVGAGain(n)
  }

  api.setTxGain = function (n) {
    if (typeof n !== 'number') throw new Error('Tx gain should be a number')
    api.device.setTxGain(n)
  }

  api.startRx = function (cb) {
    api.device.startRx(function (data) {
      cb(data, function () {
        api.device.endRx()
      })
    })
  }

  api.stopRx = function () {
    api.device.stopRx()
  }

  api.startTx = function (cb) {
    var buf = new Buffer(0)
    api.device.startTx(function (max) {
      if (max > buf.length) buf = new Buffer(max)
      cb(max !== buf.length ? buf.slice(0, max) : buf, function () {
        api.device.endTx(buf)
      })
    })
  }

  api.stopTx = function () {
    api.device.stopTx()
  }

  return api
}
