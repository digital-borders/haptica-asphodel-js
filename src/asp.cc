#include<napi.h>

#include"../asphodel-headers/asphodel.h"



using namespace Napi;


Napi::Value InitUSB(const Napi::CallbackInfo& info) {
  int result = asphodel_usb_init() ;
  if(result != 0) {
    Napi::Error(info.Env(), Napi::String::New(info.Env(), asphodel_error_name(result))).ThrowAsJavaScriptException();
  }
  return Napi::Value();
}

Napi::Value DeinitUSB(const Napi::CallbackInfo& info) {
  asphodel_usb_deinit();
  return Napi::Value();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "InitUSB"), Napi::Function::New(env, InitUSB));
  exports.Set(Napi::String::New(env, "DeinitUSB"), Napi::Function::New(env, DeinitUSB));
  
  return exports;
}

NODE_API_MODULE(addon, Init)