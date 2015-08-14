var SegfaultHandler = require('segfault-handler');
SegfaultHandler.registerHandler();

var hackrf = require('bindings')('hackrf')
module.exports = hackrf

d = hackrf.devices() // yolo gc fix

var frequency = 900e6
d.setFrequency(frequency)
d.setBandwidth(5e6)
d.setSampleRate(10e6)

/*d.startRx(function (data) {
  process.stdout.write(data)
})

setInterval(function () {
  frequency += 0.5e6
  d.setFrequency(frequency)
  console.error('tuning to ' + frequency)
}, 500)*/

var low = Infinity
var high = 0

d.startRx(function (data) {
  var total = 0
  for(var i = 0; i < data.length; i++) total += data[i]
  if (total <= low) low = total
  if (total >= high) high = total
  var a = (total - low) / (high - low)
  console.log(new Array(Math.floor((a || 0) * 80)).join('#'))
})
