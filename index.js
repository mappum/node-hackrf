var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var hackrf = require('bindings')('hackrf')
module.exports = hackrf

d = hackrf.devices() // yolo gc fix

console.log(d.getVersion())

d.startRx(function (data) {
  console.log(data)
})
