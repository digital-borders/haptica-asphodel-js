#include <napi.h>

// #include <../asphodel-headers/asphodel.h>
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
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }

    size_t n = info[0].As<Napi::Number>().Uint32Value();

    AsphodelDevice_t **devices = new AsphodelDevice_t *[n];
    // to remove
    // devices[0] = new AsphodelDevice_t();
    int result = asphodel_usb_find_devices(devices, &n);
    // to remove
    // n = 1;
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

    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expect one Argument").ThrowAsJavaScriptException();
    }

    size_t n = info[0].As<Napi::Number>().Uint32Value();

    // to remove
    // n = 1;
    AsphodelDevice_t **devices = new AsphodelDevice_t *[n];
    // to remove
    // devices[0] = new AsphodelDevice_t();
    int result = asphodel_tcp_find_devices(devices, &n);
    // to remove
    // n = 1;
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
    if (info.Length() != 2)
    {
        Napi::Error::New(info.Env(), "Expect 2 Argument").ThrowAsJavaScriptException();
    }
    uint32_t filter = info[0].As<Napi::Number>().Uint32Value();
    size_t n = info[1].As<Napi::Number>().Uint32Value();

    // to remove
    // n = 1;
    AsphodelDevice_t **devices = new AsphodelDevice_t *[n];
    // to remove
    // devices[0] = new AsphodelDevice_t();
    int result = asphodel_tcp_find_devices_filter(devices, &n, filter);
    // to remove
    // n = 1;
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

Napi::Value createStreamDecoder(const Napi::CallbackInfo &info)
{
    if (info.Length() != 2)
    {
        Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
    }

    StreamAndChannels *sc = StreamAndChannels::Unwrap(info[0].As<Napi::Object>());

    uint8_t sbo = info[1].As<Napi::Number>().Uint32Value();
    AsphodelStreamDecoder_t *decoder = nullptr;
    int result = asphodel_create_stream_decoder(&sc->strAndCh, sbo, &decoder);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    Napi::Function constr = StreamDecoder::getClass(info.Env());
    Napi::Object ob = Napi::Object::New(info.Env());
    ob.Set("decoder", Napi::External<AsphodelStreamDecoder_t>::New(info.Env(), decoder));
    return constr.New({ob});
}

Napi::Value createDeviceDecoder(const Napi::CallbackInfo &info)
{
    if (info.Length() != 3)
    {
        Napi::Error::New(info.Env(), "Expects 3 arguments").ThrowAsJavaScriptException();
    }

    Napi::Array scs = info[0].As<Napi::Array>();

    AsphodelStreamAndChannels_t *sc = new AsphodelStreamAndChannels_t[scs.Length()];

    for (size_t i = 0; i < scs.Length(); i++)
    {
        StreamAndChannels *scclass = StreamAndChannels::Unwrap(scs.Get(i).As<Napi::Object>());
        sc[i] = scclass->strAndCh;
    }

    uint8_t fb = info[1].As<Napi::Number>().Uint32Value();
    uint8_t ib = info[2].As<Napi::Number>().Uint32Value();

    AsphodelDeviceDecoder_t *decoder = nullptr;
    int result = asphodel_create_device_decoder(sc, scs.Length(), fb, ib, &decoder);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    Napi::Function constr = DeviceDecoder::getClass(info.Env());
    Napi::Object ob = Napi::Object::New(info.Env());
    ob.Set("decoder", Napi::External<AsphodelDeviceDecoder_t>::New(info.Env(), decoder));
    delete[] sc;
    return constr.New({ob});
}

Napi::Value createChannelDecoder(const Napi::CallbackInfo &info)
{
    if (info.Length() != 2)
    {
        Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
    }
    AsphodelChannelDecoder_t *dec = nullptr;

    ChannelInfo *ch = ChannelInfo::Unwrap(info[0].As<Napi::Object>());

    uint16_t channel_bit_offset = info[1].As<Napi::Number>().Uint32Value();
    int result = asphodel_create_channel_decoder(ch->channel_info, channel_bit_offset, &dec);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }
    Napi::Function constr = ChannelDecoder::getClass(info.Env());
    Napi::Object ob = Napi::Object::New(info.Env());
    ob.Set("decoder", Napi::External<AsphodelChannelDecoder_t>::New(info.Env(), dec));
    return constr.New({ob});
}

Napi::Value getStreamingCounts(const Napi::CallbackInfo &info)
{
    if (info.Length() != 3)
    {
        Napi::Error::New(info.Env(), "Expects 3 arguments").ThrowAsJavaScriptException();
    }

    Napi::Array scs = info[0].As<Napi::Array>();

    AsphodelStreamAndChannels_t *sc = new AsphodelStreamAndChannels_t[scs.Length()];

    for (size_t i = 0; i < scs.Length(); i++)
    {
        StreamAndChannels *scclass = StreamAndChannels::Unwrap(scs.Get(i).As<Napi::Object>());
        sc[i] = scclass->strAndCh;
    }

    double rt = info[1].As<Napi::Number>().DoubleValue();
    double bt = info[2].As<Napi::Number>().Uint32Value();

    int packet_count;
    int transfer_count;
    unsigned int timeout;

    int result = asphodel_get_streaming_counts(sc, scs.Length(), rt, bt, &packet_count, &transfer_count, &timeout);
    if (result != 0)
    {
        Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
    }

    Napi::Object ob = Napi::Object::New(info.Env());

    ob.Set("packet_count", packet_count);
    ob.Set("transfer_count", transfer_count);
    ob.Set("timeout", timeout);

    delete[] sc;
    return ob;
}

class UnitFormatter : public Napi::ObjectWrap<UnitFormatter>
{
public:
    AsphodelUnitFormatter_t *formatter;
    UnitFormatter(const Napi::CallbackInfo &info) : Napi::ObjectWrap<UnitFormatter>(info)
    {
        if (info.Length() != 5)
        {
            Napi::Error::New(info.Env(), "Expects 5 arguments").ThrowAsJavaScriptException();
        }

        uint8_t unit_type = info[0].As<Napi::Number>().Uint32Value();
        double minimum = info[1].As<Napi::Number>().DoubleValue();
        double maximum = info[2].As<Napi::Number>().DoubleValue();
        double resolution = info[3].As<Napi::Number>().DoubleValue();
        int use_metric = info[4].As<Napi::Boolean>().Value();

        this->formatter = NULL;
        this->formatter = asphodel_create_unit_formatter(unit_type, minimum, maximum, resolution, use_metric);

        if (this->formatter == NULL)
        {
            Napi::Error::New(info.Env(), "Failed to create unit formatter").ThrowAsJavaScriptException();
        }
    }

    static Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        Napi::Function func = DefineClass(env, "UnitFormatter", {
                                                                    InstanceMethod("FormatBare", &UnitFormatter::formatBare),
                                                                    InstanceMethod("FormatAscii", &UnitFormatter::formatAscii),
                                                                    InstanceMethod("FormatHtml", &UnitFormatter::formatHtml),

                                                                });

        Napi::Object *ob = env.GetInstanceData<Napi::Object>();
        Napi::FunctionReference *ctor = new Napi::FunctionReference();
        *ctor = Napi::Persistent(func);
        ob->Set("UnitFormatterConstructor", Napi::External<Napi::FunctionReference>::New(env, ctor, [](Napi::Env, Napi::FunctionReference *ref)
                                                                                         { delete ref; }));

        exports.Set("UnitFormatter", func);
        return exports;
    }

    Napi::Value formatBare(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        size_t buffer_size = info[0].As<Napi::Number>().Uint32Value();
        double value = info[1].As<Napi::Number>().DoubleValue();

        Napi::Int8Array buf = Napi::Int8Array::New(info.Env(), buffer_size);

        int result = this->formatter->format_bare(this->formatter, (char *)buf.Data(), buf.ByteLength(), value);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value formatAscii(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        size_t buffer_size = info[0].As<Napi::Number>().Uint32Value();
        double value = info[1].As<Napi::Number>().DoubleValue();

        Napi::Int8Array buf = Napi::Int8Array::New(info.Env(), buffer_size);

        int result = this->formatter->format_ascii(this->formatter, (char *)buf.Data(), buf.ByteLength(), value);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value formatHtml(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        size_t buffer_size = info[0].As<Napi::Number>().Uint32Value();
        double value = info[1].As<Napi::Number>().DoubleValue();

        Napi::Int8Array buf = Napi::Int8Array::New(info.Env(), buffer_size);

        int result = this->formatter->format_html(this->formatter, (char *)buf.Data(), buf.ByteLength(), value);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }
};

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::Object *s = new Napi::Object();
    *s = Napi::Object::New(env);
    env.SetInstanceData<Napi::Object>(s);

    StreamAndChannels::Init(env, exports);

    exports.Set(Napi::String::New(env, "getStreamingCounts"), Napi::Function::New(env, getStreamingCounts));

    UnitFormatter::Init(env, exports);

    exports.Set(Napi::String::New(env, "createChannelDecoder"), Napi::Function::New(env, createChannelDecoder));
    exports.Set(Napi::String::New(env, "createDeviceDecoder"), Napi::Function::New(env, createDeviceDecoder));
    exports.Set(Napi::String::New(env, "createStreamDecoder"), Napi::Function::New(env, createStreamDecoder));

    exports.Set(Napi::String::New(env, "USBDevicesSupported"), Napi::Function::New(env, USBDevicesSupported));
    exports.Set(Napi::String::New(env, "USBFindDevices"), Napi::Function::New(env, USBFindDevices));
    exports.Set(Napi::String::New(env, "USBGetBackendVersion"), Napi::Function::New(env, USBGetBackendVersion));
    exports.Set(Napi::String::New(env, "USBInit"), Napi::Function::New(env, InitUSB));
    exports.Set(Napi::String::New(env, "USBDeInit"), Napi::Function::New(env, DeinitUSB));
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
    exports.Set(Napi::String::New(env, "TCPDeInit"), Napi::Function::New(env, DeinitTCP));
    exports.Set(Napi::String::New(env, "TCPFindDevices"), Napi::Function::New(env, TCPFindDevices));
    exports.Set(Napi::String::New(env, "TCPFindDevicesFilter"), Napi::Function::New(env, TCPFindDevicesFilter));
    exports.Set(Napi::String::New(env, "TCPPollDevices"), Napi::Function::New(env, PollTCPDevices));
    exports.Set(Napi::String::New(env, "TCPCreateDevice"), Napi::Function::New(env, TCPCreateDevice));

    return exports;
}

NODE_API_MODULE(addon, Init)