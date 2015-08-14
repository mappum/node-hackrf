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
  uv_async_t async;
  bindings_sem_t semaphore;

  static Nan::Persistent<v8::Function> constructor;
  static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void SetFrequency(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void GetVersion(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void StartRx(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void StopRx(const Nan::FunctionCallbackInfo<v8::Value>& info);

  static int OnRx(hackrf_transfer* transfer);
  static void CallRxCallback(uv_async_t* async);
};

#endif
