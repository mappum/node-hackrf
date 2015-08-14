#include "device.h"

using namespace v8;

Device::Device() {};
Device::~Device() {};

Nan::Persistent<Function> Device::constructor;

void Device::Init() {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Device").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "setFrequency", SetFrequency);
  Nan::SetPrototypeMethod(tpl, "getVersion", GetVersion);
  Nan::SetPrototypeMethod(tpl, "startRx", StartRx);
  Nan::SetPrototypeMethod(tpl, "stopRx", StopRx);

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
  semaphore_init(&(d->semaphore));
  uv_async_init(uv_default_loop(), &d->async, Device::CallRxCallback);

  return scope.Escape(instance);
}

void Device::SetFrequency(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  uint64_t freq = uint64_t(info[0]->Uint32Value());
  hackrf_set_freq(d->device, freq);
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

void Device::StopRx(const Nan::FunctionCallbackInfo<Value>& info) {
  Device* d = ObjectWrap::Unwrap<Device>(info.Holder());
  hackrf_stop_rx(d->device);
  info.GetReturnValue().Set(info.Holder());
}

int Device::OnRx(hackrf_transfer* transfer) {
  Device* d = (Device*) transfer->rx_ctx;
  d->async.data = transfer;
  uv_async_send(&(d->async));
  semaphore_wait(&(d->semaphore));
  return 0;
}

void Device::CallRxCallback(uv_async_t* async) {
  Nan::HandleScope scope;
  hackrf_transfer* transfer = (hackrf_transfer*) async->data;
  Device* d = (Device*) transfer->rx_ctx;
  Local<Object> buffer = Nan::CopyBuffer((const char*) transfer->buffer, transfer->valid_length).ToLocalChecked();
  Local<Value> argv[] = { buffer };
  d->onRx->Call(1, argv);
  semaphore_signal(&(d->semaphore));
}
