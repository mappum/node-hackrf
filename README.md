# node-hackrf

Control a HackRF device (e.g. Jawbreaker, HackRF One, or Rad1o) from node

```
npm install hackrf
```

## API

#### `var device = hackrf()`

Returns the first device found

#### `version = device.getVersion()`

Returns the `hackrf` native version.

#### `device.setFrequency(hz)`

Set the frequency. `hz` should be an integer

#### `device.setBandwidth(hz)`

Set the bandwidth. `hz` should be an integer

#### `device.setSampleRate(hz)`

Set the sample rate. `hz` should be an integer

#### `device.setFrequency(hz)`

Set the frequency. `hz` should be an integer

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

#### `device.stopRx()`

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

#### `device.stopTx()`

Stop transmitting data.

## Licence

MIT
