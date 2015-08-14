var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var hackrf = require('bindings')('hackrf')
module.exports = hackrf

d = hackrf.devices() // yolo gc fix

console.log(d.getVersion())

d.setFrequency(1e9)
d.setBandwidth(1000)
d.startRx(function (data) {
  console.log(data)
})
