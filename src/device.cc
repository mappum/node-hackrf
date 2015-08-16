#include "device.h"

using namespace v8;

class HackWorker : public Nan::AsyncWorker {
 public:
  HackWorker(Nan::Callback *callback, Device* d, int method, uint64_t arg)
    : Nan::AsyncWorker(callback), d(d), method(method), arg(arg) {}
  ~HackWorker() {}

  void Execute () {
    switch (method) {
      case 0:
      hackrf_set_freq(d->device, arg);
      break;

      case 1:
      hackrf_set_baseband_filter_bandwidth(d->device, arg);
      break;

      case 2:
      hackrf_set_sample_rate_manual(d->device, arg, 1);
      break;

      case 3:
      hackrf_stop_rx(d->device);
      break;

      case 4:
      hackrf_stop_tx(d->device);
      break;

      case 5:
      hackrf_close(d->device);
      break;
    }
  }

  void HandleOKCallback () {
    Nan::HandleScope scope;
    callback->Call(0, NULL);
  }

 private:
  Device* d;
  int method;
  uint64_t arg;
};

Device::Device() {};
Device::~Device() {};

Nan::Persistent<Function> Device::constructor;

void Device::Init() {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Device").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "setFrequency", SetFrequency);
  Nan::SetPrototypeMethod(tpl, "setBandwidth", SetBandwidth);
  Nan::SetPrototypeMethod(tpl, "setLNAGain", SetLNAGain);
  Nan::SetPrototypeMethod(tpl, "setVGAGain", SetVGAGain);
  Nan::SetPrototypeMethod(tpl, "setTxGain", SetTxGain);
  Nan::SetPrototypeMethod(tpl, "setAmpEnable", SetAmpEnable);
  Nan::SetPrototypeMethod(tpl, "setAntennaEnable", SetAntennaEnable);
  Nan::SetPrototypeMethod(tpl, "setSampleRate", SetSampleRate);
  Nan::SetPrototypeMethod(tpl, "getVersion", GetVersion);
  Nan::SetPrototypeMethod(tpl, "startRx", StartRx);
  Nan::SetPrototypeMethod(tpl, "stopRx", StopRx);
  Nan::SetPrototypeMethod(tpl, "startTx", StartTx);
  Nan::SetPrototypeMethod(tpl, "stopTx", StopTx);
  Nan::SetPrototypeMethod(tpl, "endTx", EndTx);
  Nan::SetPrototypeMethod(tpl, "endRx", EndRx);
  Nan::SetPrototypeMethod(tpl, "close", Close);

  constructor.Reset(tpl->GetFunction());
}

void Device::New(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* obj = new Device();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

Local<Object> Device::NewInstance(hackrf_device* hd) {
  Nan::EscapableHandleScope scope;

  Local<Function> cons = Nan::New<Function>(constructor);
  Local<Object> instance = cons->NewInstance(0, 0);

  Device* d = ObjectWrap::Unwrap<Device>(instance);
  d->device = hd;
  semaphore_init(&(d->semaphoreRx));
  semaphore_init(&(d->semaphoreTx));
  uv_async_init(uv_default_loop(), &d->asyncRx, Device::CallRxCallback);
  uv_async_init(uv_default_loop(), &d->asyncTx, Device::CallTxCallback);

  return scope.Escape(instance);
}

void Device::SetFrequency(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint64_t freq = uint64_t(info[0]->Uint32Value());
  Local<Function> callback = info[1].As<Function>();
  Nan::AsyncQueueWorker(new HackWorker(new Nan::Callback(callback), d, 0, freq));
  info.GetReturnValue().Set(info.Holder());
}

void Device::SetBandwidth(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint64_t bandwidth = uint64_t(info[0]->Uint32Value());
  Local<Function> callback = info[1].As<Function>();
  Nan::AsyncQueueWorker(new HackWorker(new Nan::Callback(callback), d, 1, bandwidth));
  info.GetReturnValue().Set(info.Holder());
}

void Device::SetSampleRate(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint64_t freq = uint64_t(info[0]->Uint32Value());
  Local<Function> callback = info[1].As<Function>();
  Nan::AsyncQueueWorker(new HackWorker(new Nan::Callback(callback), d, 2, freq));
  info.GetReturnValue().Set(info.Holder());
}

void Device::SetLNAGain(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint64_t level = uint64_t(info[0]->Uint32Value());
  hackrf_set_lna_gain(d->device, level);
  info.GetReturnValue().Set(info.Holder());
}

void Device::SetVGAGain(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint64_t level = uint64_t(info[0]->Uint32Value());
  hackrf_set_vga_gain(d->device, level);
  info.GetReturnValue().Set(info.Holder());
}

void Device::SetTxGain(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint64_t level = uint64_t(info[0]->Uint32Value());
  hackrf_set_txvga_gain(d->device, level);
  info.GetReturnValue().Set(info.Holder());
}

void Device::SetAmpEnable(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint8_t enable = uint64_t(info[0]->Uint32Value());
  hackrf_set_amp_enable(d->device, enable);
  info.GetReturnValue().Set(info.Holder());
}

void Device::SetAntennaEnable(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint8_t enable = uint64_t(info[0]->Uint32Value());
  hackrf_set_antenna_enable(d->device, enable);
  info.GetReturnValue().Set(info.Holder());
}

void Device::GetVersion(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint8_t len = 64;
  char* version = (char*) malloc(len);
  hackrf_version_string_read(d->device, version, len);
  info.GetReturnValue().Set(Nan::New(version).ToLocalChecked());
  free(version);
}

void Device::StartRx(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  d->onRx = new Nan::Callback(info[0].As<Function>());
  hackrf_start_rx(d->device, Device::OnRx, d);
  info.GetReturnValue().Set(info.Holder());
}

void Device::StartTx(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  d->onTx = new Nan::Callback(info[0].As<Function>());
  int res = hackrf_start_tx(d->device, Device::OnTx, d);
  info.GetReturnValue().Set(info.Holder());
}

void Device::StopRx(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  Local<Function> callback = info[0].As<Function>();
  Nan::AsyncQueueWorker(new HackWorker(new Nan::Callback(callback), d, 3, 0));
  info.GetReturnValue().Set(info.Holder());
}

void Device::StopTx(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  Local<Function> callback = info[0].As<Function>();
  Nan::AsyncQueueWorker(new HackWorker(new Nan::Callback(callback), d, 4, 0));
  info.GetReturnValue().Set(info.Holder());
}

void Device::Close(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  Local<Function> callback = info[0].As<Function>();
  Nan::AsyncQueueWorker(new HackWorker(new Nan::Callback(callback), d, 5, 0));
  info.GetReturnValue().Set(info.Holder());
}

void Device::EndRx(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  semaphore_signal(&(d->semaphoreRx));
  info.GetReturnValue().Set(info.Holder());
}

void Device::EndTx(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  hackrf_transfer* transfer = d->transfer;

  Local<Object> buffer = info[0].As<Object>();
  int len = node::Buffer::Length(buffer);
  char *data = node::Buffer::Data(buffer);

  memcpy(transfer->buffer, data, len);
  semaphore_signal(&(d->semaphoreTx));

  info.GetReturnValue().Set(info.Holder());
}

int Device::OnTx(hackrf_transfer* transfer) {
  Device* d = (Device*) transfer->tx_ctx;
  d->asyncTx.data = transfer;
  uv_async_send(&(d->asyncTx));
  semaphore_wait(&(d->semaphoreTx));
  return 0;
}

int Device::OnRx(hackrf_transfer* transfer) {
  Device* d = (Device*) transfer->rx_ctx;
  d->asyncRx.data = transfer;
  uv_async_send(&(d->asyncRx));
  semaphore_wait(&(d->semaphoreRx));
  return 0;
}

void Device::CallRxCallback(uv_async_t* async) {
  Nan::HandleScope scope;
  hackrf_transfer* transfer = (hackrf_transfer*) async->data;
  Device* d = (Device*) transfer->rx_ctx;
  d->transfer = transfer;
  Local<Object> buffer = Nan::CopyBuffer((const char*) transfer->buffer, transfer->valid_length).ToLocalChecked();
  Local<Value> argv[] = { buffer };
  d->onRx->Call(1, argv);
}

void Device::CallTxCallback(uv_async_t* async) {
  Nan::HandleScope scope;
  hackrf_transfer* transfer = (hackrf_transfer*) async->data;
  Device* d = (Device*) transfer->tx_ctx;
  d->transfer = transfer;
  Local<Number> n = Nan::New<Number>(transfer->valid_length);
  Local<Value> argv[] = { n };
  d->onTx->Call(1, argv);
}
