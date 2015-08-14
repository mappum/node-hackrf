#ifndef DEVICE_H
#define DEVICE_H

#include <node.h>
#include <nan.h>
#include "hackrf.h"
#include "semaphore.h"

class Device : public Nan::ObjectWrap {
 public:
  static void Init();
  static v8::Local<v8::Object> NewInstance(hackrf_device* device);

 private:
  Device();
  ~Device();

  hackrf_device* device;
  Nan::Callback* onRx;
  Nan::Callback* onTx;
  uv_async_t asyncRx;
  uv_async_t asyncTx;
  bindings_sem_t semaphore;

  hackrf_transfer* transfer;

  static Nan::Persistent<v8::Function> constructor;
  static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void SetFrequency(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void SetBandwidth(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void SetSampleRate(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void GetVersion(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void StartRx(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void StopRx(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void StartTx(const Nan::FunctionCallbackInfo<v8::Value>& info);

  static void SendTx(const Nan::FunctionCallbackInfo<v8::Value>& info);

  static int OnRx(hackrf_transfer* transfer);
  static int OnTx(hackrf_transfer* transfer);
  static void CallRxCallback(uv_async_t* async);
  static void CallTxCallback(uv_async_t* async);
};

#endif
