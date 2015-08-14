var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var hackrf = require('bindings')('hackrf')
module.exports = hackrf

//console.log(hackrf.devices().getVersion())
console.log(hackrf.devices().startRx(function (data) {
  console.log(data)
}))

setInterval(function(){}, 10000000)
