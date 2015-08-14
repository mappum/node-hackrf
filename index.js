var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var hackrf = require('bindings')('hackrf')
module.exports = hackrf

var d = hackrf.devices()

console.log(d.getVersion())
console.log(d.startRx(function (data) {
  console.log(data)
}))

setInterval(function(){}, 10000000)
