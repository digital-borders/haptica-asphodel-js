#include <../asphodel-headers/asphodel.h>
#include <napi.h>
#include <optional>
#include <thread>

int cow = 0;

class DeviceWrapper : public Napi::ObjectWrap<DeviceWrapper>
{
public:
    Napi::FunctionReference transfer_callback;
    Napi::FunctionReference stream_callback;
    Napi::FunctionReference connect_callback;
    Napi::FunctionReference error_callback;

    static int errorCbTrigger(struct AsphodelDevice_t *device, int status, void *closure)
    {
        DeviceWrapper *wrapper = static_cast<DeviceWrapper *>(closure);
        if (!wrapper->error_callback.IsEmpty())
        {
            wrapper->error_callback.Call({
            Napi::Number::From<int>(wrapper->error_callback.Env(), status),
            });
        }
        return 0;
    }

    static void connectTrigger(int status, int connected, void *closure)
    {
        DeviceWrapper *wrapper = static_cast<DeviceWrapper *>(closure);
        if (!wrapper->connect_callback.IsEmpty())
        {
            wrapper->connect_callback.Call({
                Napi::Number::From<int>(wrapper->connect_callback.Env(), status),
                Napi::Number::From<int>(wrapper->connect_callback.Env(), connected),

            });
        }
    }

    static void streamingCallbackTrigger(int status, const uint8_t *stream_data, size_t packet_size, size_t packet_count, void *closure)
    {
        DeviceWrapper *wrapper = static_cast<DeviceWrapper *>(closure);
        if (!wrapper->stream_callback.IsEmpty())
        {
            Napi::Uint8Array arr = Napi::Uint8Array::New(wrapper->stream_callback.Env(), packet_size * packet_count);
            memcpy(arr.Data(), stream_data, packet_size * packet_count);
            wrapper->stream_callback.Call({Napi::Number::From<int>(wrapper->stream_callback.Env(), status),
                                            arr,
                                            Napi::Number::From<int>(wrapper->stream_callback.Env(), packet_size),
                                            Napi::Number::From<int>(wrapper->stream_callback.Env(), packet_count)});
        }
    }

    static void transferCallBacktrigger(int status, const uint8_t *params, size_t param_length, void *closure)
    {
        DeviceWrapper *wrapper = static_cast<DeviceWrapper *>(closure);
        if (!wrapper->transfer_callback.IsEmpty())
        {
            Napi::Uint8Array arr = Napi::Uint8Array::New(wrapper->transfer_callback.Env(), param_length);
            memcpy(arr.Data(), params, param_length);
            wrapper->transfer_callback.Call({Napi::Number::From<int>(wrapper->transfer_callback.Env(), status), arr});
        }
    }

    DeviceWrapper(const Napi::CallbackInfo &info) : Napi::ObjectWrap<DeviceWrapper>(info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Requires device argument").ThrowAsJavaScriptException();
        }

        AsphodelDevice_t *dev = info[0]
                                    .As<Napi::Object>()
                                    .Get("dev")
                                    .As<Napi::External<AsphodelDevice_t>>()
                                    .Data();
        this->transfer_callback  = Napi::FunctionReference();
        this->connect_callback = Napi::FunctionReference();
        this->stream_callback = Napi::FunctionReference();
        this->error_callback = Napi::FunctionReference();
        dev->error_closure = this;
        dev->error_callback = errorCbTrigger;
        this->device = dev;
    }

    static Napi::Function GetClass(Napi::Env env)
    {
        Napi::Function fun = DefineClass(env, "Device",
                                         {
                                             InstanceMethod("close", &DeviceWrapper::Close),
                                             InstanceMethod("supportsRadioCommands", &DeviceWrapper::asphodelSupportsRadioCommands),
                                             InstanceMethod("supportsRemoteCommands", &DeviceWrapper::asphodelSupportsRadioCommands),
                                             InstanceMethod("supportsBootloaderCommands", &DeviceWrapper::asphodelSupportsBootloaderCommands),
                                             InstanceMethod("supportsRFPowerCommands", &DeviceWrapper::asphodelSupportsRFCommands),
                                             InstanceMethod("getProtocalVersion", &DeviceWrapper::getProtocalV),
                                             InstanceMethod("getProtocalVersionString", &DeviceWrapper::getProtocalVstr),
                                             InstanceMethod("getBoardInfo", &DeviceWrapper::getBoardInfo),
                                             InstanceMethod("getBuildInfo", &DeviceWrapper::getBuildInfo),
                                             InstanceMethod("getBuildDate", &DeviceWrapper::getBuildDate),
                                             InstanceMethod("getCommitID", &DeviceWrapper::getCommitId),
                                             InstanceMethod("getRepoBranch", &DeviceWrapper::getRepoBranch),
                                             InstanceMethod("getChipFamily", &DeviceWrapper::getChipFamily),
                                             InstanceMethod("getChipModel", &DeviceWrapper::getChipModel),
                                             InstanceMethod("eraseNVM", &DeviceWrapper::eraseNvm),
                                             InstanceMethod("getNVMSize", &DeviceWrapper::getNVMsize),
                                             InstanceMethod("getChipID", &DeviceWrapper::getChipID),
                                             InstanceMethod("writeNVMRaw", &DeviceWrapper::writeNVMRaw),
                                             InstanceMethod("writeNVMSection", &DeviceWrapper::writeNVMSection),
                                             InstanceMethod("readNVMRaw", &DeviceWrapper::readNvmRaw),
                                             InstanceMethod("readNVMSection", &DeviceWrapper::readNvmSection),
                                             InstanceMethod("readUserTagString", &DeviceWrapper::readUserTagString),
                                             InstanceMethod("getNVMModified", &DeviceWrapper::readUserTagString),
                                             InstanceMethod("getNVMHash", &DeviceWrapper::getNVMHash),
                                             InstanceMethod("getSettingHash", &DeviceWrapper::getSettingHash),
                                             InstanceMethod("reset", &DeviceWrapper::reset),
                                             InstanceMethod("getBootloaderInfo", &DeviceWrapper::getBootloaderInfo),
                                             InstanceMethod("bootloaderJump", &DeviceWrapper::bjump),
                                             InstanceMethod("getResetFlag", &DeviceWrapper::getResetFlag),
                                             InstanceMethod("clearResetFlag", &DeviceWrapper::clearResetFlag),
                                             InstanceMethod("getRGBCount", &DeviceWrapper::getRgbCount),
                                             InstanceMethod("getRGBValues", &DeviceWrapper::getRgbValues),
                                             InstanceMethod("setRGBValues", &DeviceWrapper::setRgbValues),
                                             InstanceMethod("setRGBValuesHex", &DeviceWrapper::setRgbValuesHex),
                                             InstanceMethod("getLEDCount", &DeviceWrapper::getLedCount),
                                             InstanceMethod("getLEDValue", &DeviceWrapper::getLEDValue),
                                             InstanceMethod("setLEDValue", &DeviceWrapper::setLEDValue),
                                             InstanceMethod("getDeviceMode", &DeviceWrapper::getDevMode),
                                            // InstanceMethod("setLEDValue", &DeviceWrapper::setLEDValue),
                                             InstanceMethod("setDeviceMode", &DeviceWrapper::setDevMode),
                                             InstanceMethod("getProtocalVersionString", &DeviceWrapper::getProtocalVstr),
                                             InstanceMethod("open", &DeviceWrapper::open),
                                             InstanceMethod("getProtocalType", &DeviceWrapper::getProtocalType),
                                             InstanceMethod("getLocationString", &DeviceWrapper::getLocationString),
                                             InstanceMethod("getSerialNumber", &DeviceWrapper::getSerialNumber),
                                             InstanceMethod("getDeviceMode", &DeviceWrapper::getDevMode),
                                             InstanceMethod("setDeviceMode", &DeviceWrapper::setDevMode),
                                             InstanceMethod("doTranfer", &DeviceWrapper::doTranfer),
                                             InstanceMethod("doTranferReset", &DeviceWrapper::doTranferReset),
                                             InstanceMethod("startStreamingPackets", &DeviceWrapper::startStreamingPs),
                                             InstanceMethod("stopStreamingPackets", &DeviceWrapper::stopStreamingPackets),
                                             InstanceMethod("getStreamPackets", &DeviceWrapper::getStreamPackets),
                                             InstanceMethod("getStreamPacketLength", &DeviceWrapper::getSPL),
                                             InstanceMethod("getMaxIncomingParamLength", &DeviceWrapper::getMaxIPL),
                                             InstanceMethod("getMaxOutgoingParamLength", &DeviceWrapper::getMaxOPL),
                                             InstanceMethod("poll", &DeviceWrapper::poll),
                                             InstanceMethod("setConnectCallback", &DeviceWrapper::setConCb),
                                             InstanceMethod("waitForConnect", &DeviceWrapper::waitForcon),
                                             InstanceMethod("getRemoteDevice", &DeviceWrapper::getRemoteDevice),
                                             InstanceMethod("reconnectDevice", &DeviceWrapper::reconnectDevice),
                                             InstanceMethod("reconnectDeviceBootloader", &DeviceWrapper::reconnectDeviceBl),
                                             InstanceMethod("reconnectDeviceApplication", &DeviceWrapper::reconnectDeviceApp),
                                             InstanceMethod("setErrorCallback", &DeviceWrapper::setErrCb),
                                             InstanceMethod("getTransportType", &DeviceWrapper::getTranspType),
                                         });

        return fun;
    }

    Napi::Value reconnectDeviceApp(const Napi::CallbackInfo &info)
    {
        AsphodelDevice_t *device = nullptr;
        int result = this->device->reconnect_device_application(this->device, &device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        if (device == this->device)
        {
            return this->Value();
        }

        Napi::Function dev_constr = DeviceWrapper::GetClass(info.Env());
        Napi::Object obj = Napi::Object::New(info.Env());
        obj.Set("dev", Napi::External<AsphodelDevice_t>::New(info.Env(), device));
        return dev_constr.New({obj});
    }

    Napi::Value reconnectDeviceBl(const Napi::CallbackInfo &info)
    {
        AsphodelDevice_t *device = nullptr;
        int result = this->device->reconnect_device_bootloader(this->device, &device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        if (device == this->device)
        {
            return this->Value();
        }

        Napi::Function dev_constr = DeviceWrapper::GetClass(info.Env());
        Napi::Object obj = Napi::Object::New(info.Env());
        obj.Set("dev", Napi::External<AsphodelDevice_t>::New(info.Env(), device));
        return dev_constr.New({obj});
    }

    Napi::Value setErrCb(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Function call_back = info[0].As<Napi::Function>(); 
        this->error_callback = Napi::Persistent(call_back);
        return Napi::Value();
    }

    Napi::Value reconnectDevice(const Napi::CallbackInfo &info)
    {
        AsphodelDevice_t *device = nullptr;
        int result = this->device->reconnect_device(this->device, &device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        if (device == this->device)
        {
            return this->Value();
        }

        Napi::Function dev_constr = DeviceWrapper::GetClass(info.Env());
        Napi::Object obj = Napi::Object::New(info.Env());
        obj.Set("dev", Napi::External<AsphodelDevice_t>::New(info.Env(), device));
        return dev_constr.New({obj});
    }

    Napi::Value getRemoteDevice(const Napi::CallbackInfo &info)
    {
        AsphodelDevice_t *device = nullptr;
        int result = this->device->get_remote_device(this->device, &device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Function dev_constr = DeviceWrapper::GetClass(info.Env());
        Napi::Object obj = Napi::Object::New(info.Env());
        obj.Set("dev", Napi::External<AsphodelDevice_t>::New(info.Env(), device));
        return dev_constr.New({obj});
    }

    Napi::Value waitForcon(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }

        uint32_t millis = info[0].As<Napi::Number>().Uint32Value();
        int result = this->device->wait_for_connect(this->device, millis);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value setConCb(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Function call_back = info[0].As<Napi::Function>();
        this->connect_callback = Napi::Persistent(call_back);
        int result = this->device->set_connect_callback(this->device, connectTrigger, this);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value poll(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }

        int millis = info[0].As<Napi::Number>().Int32Value();
        int comp = 0;
        int result = this->device->poll_device(this->device, millis, &comp);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<int>(info.Env(), comp);
    }

    Napi::Value getSPL(const Napi::CallbackInfo &info)
    {
        return Napi::Number::From<size_t>(info.Env(), this->device->get_stream_packet_length(this->device));
    }

    Napi::Value getMaxOPL(const Napi::CallbackInfo &info)
    {
        return Napi::Number::From<size_t>(info.Env(), this->device->get_max_outgoing_param_length(this->device));
    }

    Napi::Value getMaxIPL(const Napi::CallbackInfo &info)
    {
        return Napi::Number::From<size_t>(info.Env(), this->device->get_max_incoming_param_length(this->device));
    }

    Napi::Value getStreamPackets(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }

        int count = info[0].As<Napi::Number>().Int32Value();
        uint32_t timeout = info[1].As<Napi::Number>().Int32Value();
        Napi::Uint8Array arr = Napi::Uint8Array::New(info.Env(), count);
        int result = this->device->get_stream_packets_blocking(this->device, arr.Data(), &count, timeout);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return arr;
    }

    Napi::Value stopStreamingPackets(const Napi::CallbackInfo &info)
    {
        this->device->stop_streaming_packets(this->device);
        return Napi::Value();
    }

    Napi::Value startStreamingPs(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 4)
        {
            Napi::Error::New(info.Env(), "Expects 4 arguments").ThrowAsJavaScriptException();
        }

        int packet_count = info[0].As<Napi::Number>().Int32Value();
        int transfer_count = info[1].As<Napi::Number>().Int32Value();
        uint32_t timeout = info[2].As<Napi::Number>().Uint32Value();
        Napi::Function call_back = info[3].As<Napi::Function>();
        this->stream_callback = Napi::Persistent(call_back);
        int result = this->device->start_streaming_packets(this->device, packet_count, transfer_count, timeout, streamingCallbackTrigger, this);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value doTranferReset(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 3)
        {
            Napi::Error::New(info.Env(), "Expects 3 arguments").ThrowAsJavaScriptException();
        }

        uint8_t command = info[0].As<Napi::Number>().Int32Value();
        Napi::Uint8Array params = info[1].As<Napi::Uint8Array>();
        Napi::Function call_back = info[2].As<Napi::Function>();
        this->transfer_callback = Napi::Persistent(call_back);
        int result = this->device->do_transfer_reset(this->device, command, params.Data(), params.ByteLength(), transferCallBacktrigger, this);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value doTranfer(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 3)
        {
            Napi::Error::New(info.Env(), "Expects 3 arguments").ThrowAsJavaScriptException();
        }

        uint8_t command = info[0].As<Napi::Number>().Int32Value();
        Napi::Uint8Array params = info[1].As<Napi::Uint8Array>();
        Napi::Function call_back = info[2].As<Napi::Function>();
        this->transfer_callback = Napi::Persistent(call_back);
        int result = this->device->do_transfer(this->device, command, params.Data(), params.ByteLength(), transferCallBacktrigger, this);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getSerialNumber(const Napi::CallbackInfo &info)
    {
        char buffer[129];
        memset(buffer, 0, sizeof(buffer));
        int result = this->device->get_serial_number(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        return Napi::String::New(info.Env(), buffer);
    }

       Napi::Value getTranspType(const Napi::CallbackInfo &info)
    {
        return Napi::String::New(info.Env(), this->device->transport_type);
    }


    Napi::Value getLocationString(const Napi::CallbackInfo &info)
    {
        return Napi::String::New(info.Env(), this->device->location_string);
    }

    Napi::Value getProtocalType(const Napi::CallbackInfo &info)
    {
        return Napi::Number::From<int>(info.Env(), this->device->protocol_type);
    }

    Napi::Value open(const Napi::CallbackInfo &info)
    {
        int result = this->device->open_device(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getDevMode(const Napi::CallbackInfo &info)
    {

        uint8_t mode;
        int result = asphodel_get_device_mode_blocking(this->device, &mode);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<uint8_t>(info.Env(), mode);
    }

    Napi::Value setDevMode(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        uint8_t mode = info[0].As<Napi::Number>().Int32Value();
        int result = asphodel_set_device_mode_blocking(this->device, mode);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value setLEDValue(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        uint8_t buf = info[1].As<Napi::Number>().Uint32Value();
        int result = asphodel_set_led_value_blocking(this->device, index, buf, info[2].As<Napi::Number>().Int32Value());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getLEDValue(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        uint8_t buf;
        int result = asphodel_get_led_value_blocking(this->device, index, &buf);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<uint8_t>(info.Env(), buf);
    }

    Napi::Value getLedCount(const Napi::CallbackInfo &info)
    {
        int version;
        int result = asphodel_get_led_count_blocking(this->device, &version);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<int>(info.Env(), version);
    }
    Napi::Value setRgbValuesHex(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        uint32_t buf = info[1].As<Napi::Number>().Uint32Value();
        int result = asphodel_set_rgb_values_hex_blocking(this->device, index, buf, info[2].As<Napi::Number>().Int32Value());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value setRgbValues(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        Napi::Uint8Array buf = info[1].As<Napi::Uint8Array>();
        int result = asphodel_set_rgb_values_blocking(this->device, index, buf.Data(), info[2].As<Napi::Number>().Int32Value());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getRgbValues(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        Napi::Uint8Array buf = Napi::Uint8Array::New(info.Env(), 3);
        int result = asphodel_get_rgb_values_blocking(this->device, index, buf.Data());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        return buf;
    }

    Napi::Value getRgbCount(const Napi::CallbackInfo &info)
    {
        int version;
        int result = asphodel_get_rgb_count_blocking(this->device, &version);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<int>(info.Env(), version);
    }

    Napi::Value clearResetFlag(const Napi::CallbackInfo &info)
    {
        int result = asphodel_clear_reset_flag_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getResetFlag(const Napi::CallbackInfo &info)
    {
        uint8_t version;
        int result = asphodel_get_reset_flag_blocking(this->device, &version);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<uint8_t>(info.Env(), version);
    }

    Napi::Value bjump(const Napi::CallbackInfo &info)
    {
        int result = asphodel_bootloader_jump_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getBootloaderInfo(const Napi::CallbackInfo &info)
    {

        char buffer[129];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_bootloader_info_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value reset(const Napi::CallbackInfo &info)
    {
        int result = asphodel_reset_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getSettingHash(const Napi::CallbackInfo &info)
    {

        char buffer[129];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_setting_hash_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getNVMHash(const Napi::CallbackInfo &info)
    {

        char buffer[129];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_nvm_hash_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getNVMModified(const Napi::CallbackInfo &info)
    {
        uint8_t version;
        int result = asphodel_get_nvm_modified_blocking(this->device, &version);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<uint8_t>(info.Env(), version);
    }

    Napi::Value readUserTagString(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        size_t offset = info[0].As<Napi::Number>().Int64Value();
        char buffer[129];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_read_user_tag_string_blocking(this->device, offset, 128, buffer);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }
    //===========================
    Napi::Value readNvmSection(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        size_t start_address = info[0].As<Napi::Number>().Int64Value();
        size_t length = info[1].As<Napi::Number>().Int64Value();
        Napi::Uint8Array arr = Napi::Uint8Array::New(info.Env(), length);
        int result = asphodel_read_nvm_section_blocking(this->device, start_address, arr.Data(), length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return arr;
    }

    Napi::Value readNvmRaw(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        size_t start_address = info[0].As<Napi::Number>().Int64Value();
        size_t length = info[1].As<Napi::Number>().Int64Value();
        uint8_t *buf = new uint8_t[length];
        int result = asphodel_read_nvm_raw_blocking(this->device, start_address, buf, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Uint8Array arr = Napi::Uint8Array::New(info.Env(), length);
        memcpy(arr.Data(), buf, length);
        delete[] buf;
        return arr;
    }

    Napi::Value writeNVMSection(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }

        size_t start_address = info[0].As<Napi::Number>().Int64Value();
        Napi::Uint8Array arr = info[1].As<Napi::Uint8Array>();
        uint8_t *data = arr.Data();

        // for(size_t i = 0; i < arr.ByteLength(); i++) {
        //     printf("[%ld] %d\n", i, data[i]);
        // }

        int result = asphodel_write_nvm_section_blocking(this->device, start_address, data, arr.ByteLength());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        return Napi::Value();
    }

    Napi::Value writeNVMRaw(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }

        size_t start_address = info[0].As<Napi::Number>().Int64Value();
        Napi::Uint8Array arr = info[1].As<Napi::Uint8Array>();
        uint8_t *data = arr.Data();

        // for(size_t i = 0; i < arr.ByteLength(); i++) {
        //     printf("[%ld] %d\n", i, data[i]);
        // }

        int result = asphodel_write_nvm_raw_blocking(this->device, start_address, data, arr.ByteLength());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        return Napi::Value();
    }

    Napi::Value eraseNvm(const Napi::CallbackInfo &info)
    {
        int result = asphodel_erase_nvm_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getNVMsize(const Napi::CallbackInfo &info)
    {
        size_t version;
        int result = asphodel_get_nvm_size_blocking(this->device, &version);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<size_t>(info.Env(), version);
    }

    Napi::Value getChipID(const Napi::CallbackInfo &info)
    {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_chip_id_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getChipModel(const Napi::CallbackInfo &info)
    {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_chip_model_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getChipFamily(const Napi::CallbackInfo &info)
    {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_chip_family_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getRepoBranch(const Napi::CallbackInfo &info)
    {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_repo_branch_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getCommitId(const Napi::CallbackInfo &info)
    {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_commit_id_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getBuildDate(const Napi::CallbackInfo &info)
    {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_build_date_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getBuildInfo(const Napi::CallbackInfo &info)
    {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_build_info_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getUserTagLocs(const Napi::CallbackInfo &info)
    {
        size_t locs[6];
        int result = asphodel_get_user_tag_locations_blocking(this->device, locs);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        Napi::BigUint64Array arr = Napi::BigUint64Array::New(info.Env(), 6);

        for (int i = 0; i < 6; i++)
        {
            arr[i] = locs[i];
        }
        return arr;
    }

    Napi::Value getBoardInfo(const Napi::CallbackInfo &info)
    {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));
        uint8_t rev;
        int result = asphodel_get_board_info_blocking(this->device, &rev, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("info", Napi::String::New(info.Env(), buffer));
        ob.Set("rev", Napi::Number::From<uint8_t>(info.Env(), rev));
        return ob;
    }

    Napi::Value getProtocalVstr(const Napi::CallbackInfo &info)
    {
        char buffer[129];
        memset(buffer, 0, sizeof(buffer));
        int result = asphodel_get_protocol_version_string_blocking(this->device, buffer, 128);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer);
    }

    Napi::Value getProtocalV(const Napi::CallbackInfo &info)
    {
        uint16_t version;
        int result = asphodel_get_protocol_version_blocking(this->device, &version);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<uint16_t>(info.Env(), version);
    }

    Napi::Value asphodelSupportsRadioCommands(const Napi::CallbackInfo &info)
    {
        int result = asphodel_supports_radio_commands(this->device);
        return Napi::Boolean::From<bool>(info.Env(), result);
    }

    Napi::Value asphodelSupportsRemoteCommands(const Napi::CallbackInfo &info)
    {
        int result = asphodel_supports_remote_commands(this->device);
        return Napi::Boolean::From<bool>(info.Env(), result);
    }

    Napi::Value asphodelSupportsRFCommands(const Napi::CallbackInfo &info)
    {
        int result = asphodel_supports_rf_power_commands(this->device);
        return Napi::Boolean::From<bool>(info.Env(), result);
    }

    Napi::Value asphodelSupportsBootloaderCommands(const Napi::CallbackInfo &info)
    {
        int result = asphodel_supports_bootloader_commands(this->device);
        return Napi::Boolean::From<bool>(info.Env(), result);
    }

    Napi::Value Close(const Napi::CallbackInfo &)
    {
        this->device->close_device(this->device);
        return Napi::Value();
    }

    ~DeviceWrapper()
    {
        //this->errorCbTrigger(this->device, 78, this);
        //printf("done triggering error callback\n");
        //this->device->close_device(this->device);
        //this->device->free_device(this->device);
    }

private:
    AsphodelDevice_t *device;
};