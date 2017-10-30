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
    int result = HACKRF_SUCCESS;
    uint8_t board_id = BOARD_ID_INVALID;
    Local<Object> device = Nan::New<Object>();
    hackrf_device* cdevice = NULL;

    result = hackrf_device_list_open(list, i, &cdevice);
    if (result != HACKRF_SUCCESS) {
      return Nan::ThrowError("Could not open board");
    }

    result = hackrf_board_id_read(cdevice, &board_id);
    if (result != HACKRF_SUCCESS) {
      return Nan::ThrowError("Could not read board id");
    }
    device->Set(Nan::New("boardId").ToLocalChecked(), Nan::New(board_id));

    result = hackrf_close(cdevice);
    if (result != HACKRF_SUCCESS) {
      return Nan::ThrowError("Could not close board");
    }

    device->Set(Nan::New("usbIndex").ToLocalChecked(), Nan::New(list->usb_device_index[i]));
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
