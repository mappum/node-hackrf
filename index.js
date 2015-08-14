var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var hackrf = require('bindings')('hackrf')
module.exports = hackrf

d = hackrf.devices() // yolo gc fix

console.log(d.getVersion())

d.setFrequency(1e9)
d.startTx(function (max) {
  var buf = new Buffer(max)
  for (var i = 0; i < max; i++) buf[i] = 255
  console.log('sending',buf)
  d.sendTx(buf)
})
