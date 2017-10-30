#include <nan.h>
#include <node.h>
#include "hackrf.h"
#include "device.h"

using namespace v8;

void OpenDevice(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  int index = info[0]->Uint32Value();

  hackrf_device_list_t* list = hackrf_device_list();
  if (index >= list->devicecount) return Nan::ThrowError("Invalid device index");

  hackrf_device* device;
  hackrf_device_list_open(list, index, &device);
  hackrf_device_list_free(list);

  info.GetReturnValue().Set(Device::NewInstance(device));
}

void Devices(const FunctionCallbackInfo<Value>& info) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  hackrf_device_list_t* list = hackrf_device_list();

  Local<Array> devices = Nan::New<Array>(list->devicecount);
  for(int i = 0; i < list->devicecount; i++) {
    Local<Object> device = Nan::New<Object>();
    enum hackrf_usb_board_id usb_board_id = list->usb_board_ids[i];
    const char* name = hackrf_usb_board_id_name(usb_board_id);
    device->Set(Nan::New("name").ToLocalChecked(), Nan::New<String>(name).ToLocalChecked());
    device->Set(Nan::New("usbIndex").ToLocalChecked(), Nan::New(list->usb_device_index[i]));
    device->Set(Nan::New("usbBoardId").ToLocalChecked(), Nan::New(usb_board_id));
    device->Set(Nan::New("serialNumber").ToLocalChecked(), Nan::New<String>(list->serial_numbers[i]).ToLocalChecked());

    devices->Set(i, device);
  }

  Nan::SetMethod(devices, "_open", OpenDevice);
  hackrf_device_list_free(list);
  info.GetReturnValue().Set(devices);
}

void InitAll(Handle<Object> exports) {
  hackrf_init();
  Device::Init();

  NODE_SET_METHOD(exports, "devices", Devices);
}

NODE_MODULE(hackrf, InitAll)
