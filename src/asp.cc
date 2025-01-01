#include <napi.h>

#include <../asphodel-headers/asphodel.h>
#include "device_wrapper.h"

using namespace Napi;

Napi::Value InitUSB(const Napi::CallbackInfo &info)
{
    int result = asphodel_usb_init();
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    return Napi::Value();
}

Napi::Value DeinitUSB(const Napi::CallbackInfo &info)
{
    asphodel_usb_deinit();
    return Napi::Value();
}

Napi::Value PollUSBDevices(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }
    int millis = info[0].As<Napi::Number>().Int32Value();
    int result = asphodel_usb_poll_devices(millis);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    return Napi::Value();
}

Napi::Value USBGetBackendVersion(const Napi::CallbackInfo &info)
{
    const char *result = asphodel_usb_get_backend_version();
    return Napi::String::New(info.Env(), result);
}

Napi::Value USBFindDevices(const Napi::CallbackInfo &info)
{
    size_t n;
    int result = asphodel_usb_find_devices(nullptr, &n);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    // to remove
    //n = 1;
    AsphodelDevice_t **devices = new AsphodelDevice_t *[n];
    // to remove
    //devices[0] = new AsphodelDevice_t();
    result = asphodel_usb_find_devices(devices, &n);
    // to remove
    //n = 1;
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }


    Napi::Array arr = Napi::Array::New(info.Env(), n);

    for (size_t i = 0; i < n; i++)
    {
        Napi::Function dev_constr = DeviceWrapper::GetClass(info.Env());
        Napi::Object obj = Napi::Object::New(info.Env());
        obj.Set("dev", Napi::External<AsphodelDevice_t>::New(info.Env(), devices[i]));
        arr[i] = dev_constr.New({obj});
    }
    delete[] devices;
    return arr;
}

Napi::Value USBDevicesSupported(const Napi::CallbackInfo &info)
{
    int result = asphodel_usb_devices_supported();
    return Napi::Boolean::From<bool>(info.Env(), result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "USBDevicesSupported"), Napi::Function::New(env, USBDevicesSupported));
    exports.Set(Napi::String::New(env, "USBFindDevices"), Napi::Function::New(env, USBFindDevices));
    exports.Set(Napi::String::New(env, "USBGetBackendVersion"), Napi::Function::New(env, USBGetBackendVersion));
    exports.Set(Napi::String::New(env, "USBInit"), Napi::Function::New(env, InitUSB));
    exports.Set(Napi::String::New(env, "USBDeinit"), Napi::Function::New(env, DeinitUSB));
    exports.Set(Napi::String::New(env, "USBPollDevices"), Napi::Function::New(env, PollUSBDevices));

    return exports;
}

NODE_API_MODULE(addon, Init)