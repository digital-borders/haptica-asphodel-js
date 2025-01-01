#include <napi.h>

//#include <../asphodel-headers/asphodel.h>
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


Napi::Value getErrorName(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }
    const char *result = asphodel_error_name(info[0].As<Napi::Number>().Int32Value());
    return Napi::String::New(info.Env(), result);
}



Napi::Value getUnitTYpeName(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }
    const char *result = asphodel_unit_type_name(info[0].As<Napi::Number>().Int32Value());
    return Napi::String::New(info.Env(), result);
}

Napi::Value getUnitTypeCount(const Napi::CallbackInfo &info)
{
    char result = asphodel_get_unit_type_count();
    return Napi::Number::From<int>(info.Env(), result);
}

Napi::Value getChannelTYpeName(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }
    const char *result = asphodel_channel_type_name(info[0].As<Napi::Number>().Uint32Value());
    return Napi::String::New(info.Env(), result);
}

Napi::Value getChannelTypeCount(const Napi::CallbackInfo &info)
{
    char result = asphodel_get_channel_type_count();
    return Napi::Number::From<int>(info.Env(), result);
}

Napi::Value getSettingTYpeName(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }
    const char *result = asphodel_setting_type_name(info[0].As<Napi::Number>().Uint32Value());
    return Napi::String::New(info.Env(), result);
}

Napi::Value getSettingTypeCount(const Napi::CallbackInfo &info)
{
    char result = asphodel_get_setting_type_count();
    return Napi::Number::From<int>(info.Env(), result);
}

Napi::Value InitTCP(const Napi::CallbackInfo &info)
{
    int result = asphodel_tcp_init();
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    return Napi::Value();
}

Napi::Value DeinitTCP(const Napi::CallbackInfo &info)
{
    asphodel_tcp_deinit();
    return Napi::Value();
}

Napi::Value TCPDevicesSupported(const Napi::CallbackInfo &info)
{
    int result = asphodel_tcp_devices_supported();
    return Napi::Boolean::From<bool>(info.Env(), result);
}


Napi::Value TCPFindDevices(const Napi::CallbackInfo &info)
{
    size_t n;
    int result = asphodel_tcp_find_devices(nullptr, &n);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    // to remove
    //n = 1;
    AsphodelDevice_t **devices = new AsphodelDevice_t *[n];
    // to remove
    //devices[0] = new AsphodelDevice_t();
    result = asphodel_tcp_find_devices(devices, &n);
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


Napi::Value TCPFindDevicesFilter(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }
    uint32_t filter = info[0].As<Napi::Number>().Uint32Value();
    size_t n;
    int result = asphodel_tcp_find_devices_filter(nullptr, &n, filter);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    // to remove
    //n = 1;
    AsphodelDevice_t **devices = new AsphodelDevice_t *[n];
    // to remove
    //devices[0] = new AsphodelDevice_t();
    result = asphodel_tcp_find_devices_filter(devices, &n, filter);
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

Napi::Value PollTCPDevices(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }
    int millis = info[0].As<Napi::Number>().Int32Value();
    int result = asphodel_tcp_poll_devices(millis);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    return Napi::Value();
}


Napi::Value TCPCreateDevice(const Napi::CallbackInfo &info)
{
    if (info.Length() != 4)
    {
        Napi::Error::New(info.Env(), "Expect 4 Argument").ThrowAsJavaScriptException();
    }

    Napi::String host = info[0].As<Napi::String>();
    uint16_t port = info[1].As<Napi::Number>().Uint32Value();
    int timeout = info[2].As<Napi::Number>().Int32Value();
    Napi::String serial = info[3].As<Napi::String>();
    AsphodelDevice_t *dev = nullptr;
    int result = asphodel_tcp_create_device(host.Utf8Value().c_str(), port, timeout, serial.Utf8Value().c_str(), &dev);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }

    Napi::Function dev_constr = DeviceWrapper::GetClass(info.Env());
    Napi::Object obj = Napi::Object::New(info.Env());
    obj.Set("dev", Napi::External<AsphodelDevice_t>::New(info.Env(), dev));
    return dev_constr.New({obj});
}



Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "USBDevicesSupported"), Napi::Function::New(env, USBDevicesSupported));
    exports.Set(Napi::String::New(env, "USBFindDevices"), Napi::Function::New(env, USBFindDevices));
    exports.Set(Napi::String::New(env, "USBGetBackendVersion"), Napi::Function::New(env, USBGetBackendVersion));
    exports.Set(Napi::String::New(env, "USBInit"), Napi::Function::New(env, InitUSB));
    exports.Set(Napi::String::New(env, "USBDeinit"), Napi::Function::New(env, DeinitUSB));
    exports.Set(Napi::String::New(env, "USBPollDevices"), Napi::Function::New(env, PollUSBDevices));

    exports.Set(Napi::String::New(env, "getErrorName"), Napi::Function::New(env, getErrorName));
    exports.Set(Napi::String::New(env, "getUnitTypeName"), Napi::Function::New(env, getUnitTYpeName));
    exports.Set(Napi::String::New(env, "getUnitTypeCount"), Napi::Function::New(env, getUnitTypeCount));
    exports.Set(Napi::String::New(env, "getSettingTypeName"), Napi::Function::New(env, getSettingTYpeName));
    exports.Set(Napi::String::New(env, "getSettingTypeCount"), Napi::Function::New(env, getSettingTypeCount));
    exports.Set(Napi::String::New(env, "getChannelTypeName"), Napi::Function::New(env, getChannelTYpeName));
    exports.Set(Napi::String::New(env, "getChannelTypeCount"), Napi::Function::New(env, getChannelTypeCount));

    exports.Set(Napi::String::New(env, "TCPInit"), Napi::Function::New(env, InitTCP));
    exports.Set(Napi::String::New(env, "TCPDevicesSupported"), Napi::Function::New(env, TCPDevicesSupported));
    exports.Set(Napi::String::New(env, "TCPDeinit"), Napi::Function::New(env, DeinitTCP));
    exports.Set(Napi::String::New(env, "TCPFindDevices"), Napi::Function::New(env, TCPFindDevices));
    exports.Set(Napi::String::New(env, "TCPFindDevicesFilter"), Napi::Function::New(env, TCPFindDevicesFilter));
    exports.Set(Napi::String::New(env, "TCPPollDevices"), Napi::Function::New(env, PollTCPDevices));
    exports.Set(Napi::String::New(env, "TCPCreateDevice"), Napi::Function::New(env, TCPCreateDevice));

    return exports;
}

NODE_API_MODULE(addon, Init)