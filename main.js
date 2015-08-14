var hackrf = require('hackrf')
var radio = hackrf.devices().open(0)
radio.setFrequency(93.2e6) // 93.2mhz
radio.pipe(process.stdout)
