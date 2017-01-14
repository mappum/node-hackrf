# node-hackrf

Control a HackRF device (e.g. Jawbreaker, HackRF One, or Rad1o) from node

This is a low-level binding to `libhackrf`. If you'd like a nice stream interface, try [`hackrf-stream`](https://github.com/mappum/hackrf-stream).

```
npm install hackrf
```

## API

#### `var devices = hackrf()`

Returns an array containing information about the connected HackRF devices. If no devices are connected, an empty array will be returned.

#### `var device = devices.open(index)`

Opens and returns the device in the array of devices with index `index`.

#### `device.getVersion()`

Returns the `hackrf` firmware version.

#### `device.setFrequency(hz, [callback])`

Set the frequency. `hz` should be an integer

#### `device.setBandwidth(hz, [callback])`

Set the bandwidth. `hz` should be an integer

#### `device.setSampleRate(hz, [callback])`

Set the sample rate. `hz` should be an integer

#### `device.setLNAGain(val)`

Set the lna gain. Should be between `0-40`

#### `device.setVGAGain(val)`

Set the vna gain. Should be between `0-62`

#### `device.setTxGain(val)`

Set the tx gain. Should be between `0-47`

#### `device.startRx(onrx)`

Receive data. `onrx` is called with `(data, cb)`.

``` js
device.startRx(function (data, cb) {
  console.log('read', data)
  cb() // done
})
```

#### `device.stopRx([callback])`

Stop receiving data.

#### `device.startTx(ontx)`

Transmit data. `ontx` is called with `(data, cb)`.
You should write the data you want to send to `data`.

``` js
device.startTx(function (data, cb) {
  for (var i = 0; i < data.length; i++) data[i] = 127
  cb() // write the data
})
```

#### `device.stopTx([callback])`

Stop transmitting data.

#### `device.close([callback])`

Release the resources for the device (allowing it to be used by another process).

## License

MIT
