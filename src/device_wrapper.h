#include <../asphodel-headers/asphodel.h>
#include <napi.h>
#include "decoders.h"

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
        this->transfer_callback = Napi::FunctionReference();
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
                                             InstanceMethod("getTransportType", &DeviceWrapper::getTranspType),
                                             InstanceMethod("tcpGetAdvertisement", &DeviceWrapper::getTcpAdV),

                                             InstanceMethod("stopRadio", &DeviceWrapper::stopRadio),
                                             InstanceMethod("startRadioScan", &DeviceWrapper::startRadioScan),
                                             InstanceMethod("getRawRadioScanResults", &DeviceWrapper::getRawRadioScanResults),
                                             InstanceMethod("getRadioScanResults", &DeviceWrapper::getRadioScanResults),
                                             InstanceMethod("getRawRadioExtraScanResults", &DeviceWrapper::getRawRadioExtraScanResults),
                                             InstanceMethod("getRadioExtraScanResults", &DeviceWrapper::getRadioExtraScanResults),
                                             InstanceMethod("getRadioScanPower", &DeviceWrapper::getRadioScanPower),
                                             InstanceMethod("connectRadio", &DeviceWrapper::connectRadio),
                                             InstanceMethod("getRadioStatus", &DeviceWrapper::getRadioStatus),
                                             InstanceMethod("getRadioCtrlVars", &DeviceWrapper::getRadioCtrlVars),
                                             InstanceMethod("getRadioDefaultSerial", &DeviceWrapper::getRadioDefaultSerial),
                                             InstanceMethod("startRadioScanBoot", &DeviceWrapper::startRadioScanBoot),
                                             InstanceMethod("connectRadioBoot", &DeviceWrapper::connectRadioBoot),
                                             InstanceMethod("stopRemote", &DeviceWrapper::stopRemote),
                                             InstanceMethod("restartRemote", &DeviceWrapper::restartRemote),
                                             InstanceMethod("restartRemoteApp", &DeviceWrapper::restartRemoteApp),
                                             InstanceMethod("restartRemoteBoot", &DeviceWrapper::restartRemoteBoot),
                                             InstanceMethod("getRemoteStatus", &DeviceWrapper::getRemoteStatus),

                                             InstanceMethod("getStreamCount", &DeviceWrapper::getStreamCount),
                                             InstanceMethod("getStream", &DeviceWrapper::getStream),
                                             InstanceMethod("getStreamChannels", &DeviceWrapper::getStreamChannels),
                                             InstanceMethod("getStreamFormat", &DeviceWrapper::getStreamFormat),
                                             InstanceMethod("enableStream", &DeviceWrapper::enableStream),
                                             InstanceMethod("warmUpStream", &DeviceWrapper::warmupStream),
                                             InstanceMethod("getStreamStatus", &DeviceWrapper::getStreamStatus),
                                             InstanceMethod("getStreamRateInfo", &DeviceWrapper::getStreamRateInfo),
                                             InstanceMethod("getChannelCount", &DeviceWrapper::getChannelCount),
                                             InstanceMethod("getChannel", &DeviceWrapper::getChannel),
                                             InstanceMethod("getChannelName", &DeviceWrapper::getChannelName),
                                             InstanceMethod("getChannelInfo", &DeviceWrapper::getChannelInfo),
                                             InstanceMethod("getChannelCoefficients", &DeviceWrapper::getChannelCoefficients),
                                             InstanceMethod("getChannelChunk", &DeviceWrapper::getChannelChunk),
                                             InstanceMethod("getChannelSpecific", &DeviceWrapper::getChannelSpecific),
                                             InstanceMethod("getChannelCalibration", &DeviceWrapper::getChannelCallib),

                                             InstanceMethod("bootloaderStartProgram", &DeviceWrapper::bootloaderStartProgram),
                                             InstanceMethod("getBootloaderPageInfo", &DeviceWrapper::getBootloaderPageInfo),
                                             InstanceMethod("getBootloaderBlockSizes", &DeviceWrapper::getBootloaderBlockSizes),
                                             InstanceMethod("startBootloaderPage", &DeviceWrapper::startBootloaderPage),
                                             InstanceMethod("writeBootloaderCodeBlock", &DeviceWrapper::writeBootloaderCodeBlock),
                                             InstanceMethod("writeBootloaderPage", &DeviceWrapper::writeBootloaderPage),
                                             InstanceMethod("finishBootloaderPage", &DeviceWrapper::finishBootloaderPage),
                                             InstanceMethod("verifyBootloaderPage", &DeviceWrapper::finishBootloaderPage),
                                             InstanceMethod("setStrainOutputs", &DeviceWrapper::setStrainOutputs),
                                             InstanceMethod("enableAccelSelfTest", &DeviceWrapper::enableAccelSelfTest),

                                             InstanceMethod("getCtrlVarCount", &DeviceWrapper::getCtrlVarCount),
                                             InstanceMethod("getCtrlVarName", &DeviceWrapper::getCtrlVarName),
                                             InstanceMethod("getCtrlVarInfo", &DeviceWrapper::getCtrlVarInfo),
                                             InstanceMethod("getCtrlVar", &DeviceWrapper::getCtrlVar),
                                             InstanceMethod("setCtrlVar", &DeviceWrapper::setCtrlVar),

                                             InstanceMethod("enableRfPower", &DeviceWrapper::enableRfPower),
                                             InstanceMethod("getRfPowerStatus", &DeviceWrapper::getRfPowerStatus),
                                             InstanceMethod("getRfPowerCtlVars", &DeviceWrapper::getRfPowerCtlVars),
                                             InstanceMethod("resetRfPowerTimeout", &DeviceWrapper::resetRfPowerTimeout),

                                             InstanceMethod("getSupplyCount", &DeviceWrapper::getSupplyCount),
                                             InstanceMethod("getSupplyName", &DeviceWrapper::getSupplyName),
                                             InstanceMethod("getSupplyInfo", &DeviceWrapper::getSupplyInfo),
                                             InstanceMethod("checkSupply", &DeviceWrapper::checkSupply),

                                         });

        return fun;
    }


        Napi::Value checkSupply(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        int tries = info[1].As<Napi::Number>().Int32Value();
        int32_t measurements = 0;
        uint8_t res = 0;
        int result = asphodel_check_supply_blocking(this->device, index, &measurements, &res, tries);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("measurement", measurements);
        ob.Set("result", res);
        return ob;
    }

        Napi::Value getSupplyInfo(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        AsphodelSupplyInfo_t in;
        int result = asphodel_get_supply_info_blocking(this->device, index, &in);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());

        ob.Set("is_battery", in.is_battery);
        ob.Set("nominal", in.nominal);
        ob.Set("offset", in.offset);
        ob.Set("scale", in.scale);
        ob.Set("unit_type", in.unit_type);
        ob.Set("name", Napi::String::New(info.Env(), (char *)in.name, in.name_length));

        return ob;
    }

    Napi::Value getSupplyName(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        uint8_t length = 128;
        char name[129];
        int result = asphodel_get_supply_name_blocking(this->device, index, name, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::String s = Napi::String::New(info.Env(), name, length);
        return s;
    }

    Napi::Value getSupplyCount(const Napi::CallbackInfo &info)
    {
        int count = 0;
        int result = asphodel_get_supply_count_blocking(this->device, &count);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::New(info.Env(), count);
    }

    Napi::Value resetRfPowerTimeout(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        uint32_t timeout = info[0].As<Napi::Number>().Uint32Value();
        int result = asphodel_reset_rf_power_timeout_blocking(this->device, timeout);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getRfPowerCtlVars(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        uint8_t length = info[0].As<Napi::Number>().Uint32Value();
        Napi::Uint8Array arr = Napi::Uint8Array::New(info.Env(), length);
        int result = asphodel_get_rf_power_ctrl_vars_blocking(this->device, arr.Data(), &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("result", arr);
        ob.Set("length", length);
        return ob;
    }

    Napi::Value getRfPowerStatus(const Napi::CallbackInfo &info)
    {
        int enable = 0;
        int result = asphodel_get_rf_power_status_blocking(this->device, &enable);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::New(info.Env(), enable);
    }

    Napi::Value enableRfPower(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int enable = info[0].As<Napi::Boolean>().Value();
        int result = asphodel_enable_rf_power_blocking(this->device, enable);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value setCtrlVar(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        int32_t value = info[1].As<Napi::Number>().Int32Value();
        int result = asphodel_set_ctrl_var_blocking(this->device, index, value);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getCtrlVar(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        int32_t value;
        int result = asphodel_get_ctrl_var_blocking(this->device, index, &value);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::New(info.Env(), value);
    }

    Napi::Value getCtrlVarInfo(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        AsphodelCtrlVarInfo_t in;
        int result = asphodel_get_ctrl_var_info_blocking(this->device, index, &in);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("maximum", in.maximum);
        ob.Set("minimum", in.minimum);
        ob.Set("offset", in.offset);
        ob.Set("scale", in.scale);
        ob.Set("unit_type", in.unit_type);
        ob.Set("name", Napi::String::New(info.Env(), (char *)in.name, in.name_length));
        return ob;
    }

    Napi::Value getCtrlVarName(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        uint8_t length = 128;
        char name[129];
        int result = asphodel_get_ctrl_var_name_blocking(this->device, index, name, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::String s = Napi::String::New(info.Env(), name, length);
        return s;
    }

    Napi::Value getCtrlVarCount(const Napi::CallbackInfo &info)
    {
        int count = 0;
        int result = asphodel_get_ctrl_var_count_blocking(this->device, &count);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::New(info.Env(), count);
    }

    Napi::Value enableAccelSelfTest(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int channel_index = info[0].As<Napi::Number>().Int32Value();
        int enable = info[1].As<Napi::Boolean>().Value();

        int result = asphodel_enable_accel_self_test_blocking(this->device, channel_index, enable);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value setStrainOutputs(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 4)
        {
            Napi::Error::New(info.Env(), "Expects 4 arguments").ThrowAsJavaScriptException();
        }

        int channel_index = info[0].As<Napi::Number>().Int32Value();
        int bridge_index = info[1].As<Napi::Number>().Int32Value();
        int positive_side = info[2].As<Napi::Number>().Int32Value();
        int negative_side = info[3].As<Napi::Number>().Int32Value();

        int result = asphodel_set_strain_outputs_blocking(this->device, channel_index, bridge_index, positive_side, negative_side);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value verifyBootloaderPage(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Uint8Array mac_tag = info[0].As<Napi::Uint8Array>();
        int result = asphodel_verify_bootloader_page_blocking(this->device, mac_tag.Data(), mac_tag.ByteLength());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value finishBootloaderPage(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Uint8Array mac_tag = info[0].As<Napi::Uint8Array>();
        int result = asphodel_finish_bootloader_page_blocking(this->device, mac_tag.Data(), mac_tag.ByteLength());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value writeBootloaderPage(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        Napi::Uint8Array data = info[0].As<Napi::Uint8Array>();
        Napi::Uint16Array blocksizes = info[1].As<Napi::Uint16Array>();

        int result = asphodel_write_bootloader_page_blocking(this->device, data.Data(), data.ByteLength(), blocksizes.Data(), blocksizes.ElementLength());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value writeBootloaderCodeBlock(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Uint8Array data = info[0].As<Napi::Uint8Array>();
        int result = asphodel_write_bootloader_code_block_blocking(this->device, data.Data(), data.ByteLength());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value startBootloaderPage(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        uint32_t pagenumber = info[0].As<Napi::Number>().Uint32Value();
        Napi::Uint8Array nonce = info[1].As<Napi::Uint8Array>();
        int result = asphodel_start_bootloader_page_blocking(this->device, pagenumber, nonce.Data(), nonce.ByteLength());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getBootloaderBlockSizes(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        uint8_t length = info[0].As<Napi::Number>().Uint32Value();
        uint16_t *block_sizes = new uint16_t[length];
        int result = asphodel_get_bootloader_block_sizes_blocking(this->device, block_sizes, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Uint16Array arr = Napi::Uint16Array::New(info.Env(), length);
        memcpy(arr.Data(), block_sizes, sizeof(uint16_t) * length);
        delete[] block_sizes;
        return arr;
    }

    Napi::Value getBootloaderPageInfo(const Napi::CallbackInfo &info)
    {
        uint32_t pageinfo;
        uint8_t length;
        int result = asphodel_get_bootloader_page_info_blocking(this->device, &pageinfo, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("page_info", pageinfo);
        ob.Set("length", length);
        return ob;
    }

    Napi::Value bootloaderStartProgram(const Napi::CallbackInfo &info)
    {
        int result = asphodel_bootloader_start_program_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getChannelCallib(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        int available = 0;
        AsphodelChannelCalibration_t c;
        int result = asphodel_get_channel_calibration_blocking(this->device, index, &available, &c);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("available", available);
        Napi::Object calibr = Napi::Object::New(info.Env());
        calibr.Set("base_setting_index", c.base_setting_index);
        calibr.Set("maximum", c.maximum);
        calibr.Set("minimum", c.minimum);
        calibr.Set("offset", c.offset);
        calibr.Set("resolution_setting_index", c.resolution_setting_index);
        calibr.Set("scale", c.scale);
        ob.Set("calibration", calibr);
        return ob;
    }

    Napi::Value getChannelSpecific(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 3)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        Napi::Uint8Array data = info[1].As<Napi::Uint8Array>();
        uint8_t length = info[2].As<Napi::Number>().Uint32Value();
        Napi::Uint8Array reply = Napi::Uint8Array::New(info.Env(), length);
        int result = asphodel_channel_specific_blocking(this->device, index, data.Data(), data.ByteLength(), reply.Data(), &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("result", reply);
        ob.Set("length", length);
        return ob;
    }

    Napi::Value getChannelChunk(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 3)
        {
            Napi::Error::New(info.Env(), "Expects 3 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        uint8_t chunk_number = info[1].As<Napi::Number>().Uint32Value();
        uint8_t length = info[2].As<Napi::Number>().Uint32Value();

        Napi::Uint8Array buf = Napi::Uint8Array::New(info.Env(), length);
        int result = asphodel_get_channel_chunk_blocking(this->device, index, chunk_number, buf.Data(), &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("result", buf);
        ob.Set("length", length);
        return ob;
    }

    Napi::Value getChannelCoefficients(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        uint8_t length = info[1].As<Napi::Number>().Uint32Value();
        Napi::Float32Array buf = Napi::Float32Array::New(info.Env(), length);
        int result = asphodel_get_channel_coefficients_blocking(this->device, index, buf.Data(), &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("result", buf);
        ob.Set("length", length);
        return ob;
    }

    Napi::Value getChannelInfo(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        AsphodelChannelInfo_t *channel = new AsphodelChannelInfo_t();
        int result = asphodel_get_channel_info_blocking(this->device, index, channel);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Function constr = ChannelInfo::GetClass(info.Env());
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("info", Napi::External<AsphodelChannelInfo_t>::New(info.Env(), channel));
        ob.Set("tofree", false);
        return constr.New({ob});
    }

    Napi::Value getChannelName(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        char buffer[129];
        memset(buffer, 0, sizeof(buffer));
        uint8_t length = 128;
        int result = asphodel_get_channel_name_blocking(this->device, index, buffer, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::String::New(info.Env(), buffer, length);
    }

    Napi::Value getChannel(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        AsphodelChannelInfo_t *channel = nullptr;
        int result = asphodel_get_channel_blocking(this->device, index, &channel);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Function constr = ChannelInfo::GetClass(info.Env());
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("info", Napi::External<AsphodelChannelInfo_t>::New(info.Env(), channel));
        ob.Set("tofree", true);
        return constr.New({ob});
    }

    Napi::Value getChannelCount(const Napi::CallbackInfo &info)
    {
        int count;
        int result = asphodel_get_channel_count_blocking(this->device, &count);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From<int>(info.Env(), count);
    }

    Napi::Value getStreamRateInfo(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        int available, chi, invert;
        float scale, offset;
        int result = asphodel_get_stream_rate_info_blocking(this->device, index, &available, &chi, &invert, &scale, &offset);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        Napi::Object ob = Napi::Object::New(info.Env());

        ob.Set("available", available);
        ob.Set("channel_index", chi);
        ob.Set("invert", invert);
        ob.Set("scale", scale);
        ob.Set("offset", offset);

        return ob;
    }

    Napi::Value getStreamStatus(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        int enable, warmup;
        int result = asphodel_get_stream_status_blocking(this->device, index, &enable, &warmup);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("enable", enable);
        ob.Set("warmup", warmup);
        return ob;
    }

    Napi::Value warmupStream(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        bool enable = info[1].As<Napi::Boolean>().Value();
        int result = asphodel_warm_up_stream_blocking(this->device, index, enable);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value enableStream(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int index = info[0].As<Napi::Number>().Int32Value();
        bool enable = info[1].As<Napi::Boolean>().Value();
        int result = asphodel_enable_stream_blocking(this->device, index, enable);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getStreamFormat(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }

        int index = info[0].As<Napi::Number>().Int32Value();
        AsphodelStreamInfo_t *stream = new AsphodelStreamInfo_t();
        int result = asphodel_get_stream_format_blocking(this->device, index, stream);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Function constr = StreamInfo::GetClass(info.Env());
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("info", Napi::External<AsphodelStreamInfo_t>::New(info.Env(), stream));
        ob.Set("tofree", true);
        return constr.New({ob});
    }

    Napi::Value getStreamChannels(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }

        int index = info[0].As<Napi::Number>().Int32Value();

        uint8_t length = info[1].As<Napi::Number>().Int32Value();
        Napi::Uint8Array buf = Napi::Uint8Array::New(info.Env(), length);

        int result = asphodel_get_stream_channels_blocking(this->device, index, buf.Data(), &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("result", buf);
        ob.Set("length", length);
        return ob;
    }

    Napi::Value getStream(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        AsphodelStreamInfo_t *stream = nullptr;
        int result = asphodel_get_stream_blocking(this->device, info[0].As<Napi::Number>().Int32Value(), &stream);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Function constr = StreamInfo::GetClass(info.Env());
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("info", Napi::External<AsphodelStreamInfo_t>::New(info.Env(), stream));
        ob.Set("tofree", false);
        return constr.New({ob});
    }

    Napi::Value getStreamCount(const Napi::CallbackInfo &info)
    {
        int count;
        uint8_t fb;
        uint8_t ib;
        int result = asphodel_get_stream_count_blocking(this->device, &count, &fb, &ib);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("count", count);
        ob.Set("filler_bits", fb);
        ob.Set("id_bits", ib);
        return ob;
    }

    Napi::Value restartRemoteBoot(const Napi::CallbackInfo &info)
    {
        int result = asphodel_restart_remote_boot_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value restartRemoteApp(const Napi::CallbackInfo &info)
    {
        int result = asphodel_restart_remote_app_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getRemoteStatus(const Napi::CallbackInfo &info)
    {
        int connected;
        uint32_t serial;
        uint8_t proty;
        int result = asphodel_get_remote_status_blocking(this->device, &connected, &serial, &proty);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("connected", connected);
        ob.Set("serial", serial);
        ob.Set("protocol_type", proty);
        return ob;
    }

    Napi::Value restartRemote(const Napi::CallbackInfo &info)
    {
        int result = asphodel_restart_remote_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value stopRemote(const Napi::CallbackInfo &info)
    {
        int result = asphodel_stop_remote_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value connectRadioBoot(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int result = asphodel_connect_radio_boot_blocking(this->device, info[0].As<Napi::Number>().Uint32Value());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value startRadioScanBoot(const Napi::CallbackInfo &info)
    {
        int result = asphodel_start_radio_scan_boot_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getRadioDefaultSerial(const Napi::CallbackInfo &info)
    {
        uint32_t serial;
        int result = asphodel_get_radio_default_serial_blocking(this->device, &serial);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::From(info.Env(), serial);
    }

    Napi::Value getRadioCtrlVars(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }

        uint8_t length = info[0].As<Napi::Number>().Int32Value();
        Napi::Uint8Array buf = Napi::Uint8Array::New(info.Env(), length);

        int result = asphodel_get_radio_ctrl_vars_blocking(this->device, buf.Data(), &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }

        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("result", buf);
        ob.Set("length", length);
        return ob;
    }

    Napi::Value getRadioStatus(const Napi::CallbackInfo &info)
    {
        int connected;
        uint32_t serial;
        uint8_t proty;
        int scannin;
        int result = asphodel_get_radio_status_blocking(this->device, &connected, &serial, &proty, &scannin);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("connected", connected);
        ob.Set("serial", serial);
        ob.Set("protocol_type", proty);
        ob.Set("scanning", scannin);
        return ob;
    }

    Napi::Value connectRadio(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        uint32_t serial = info[0].As<Napi::Number>().Uint32Value();
        int result = asphodel_connect_radio_blocking(this->device, serial);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getRadioScanPower(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Uint32Array serials = info[0].As<Napi::Uint32Array>();
        Napi::Int8Array powers = Napi::Int8Array::New(info.Env(), serials.ElementLength());
        int result = asphodel_get_radio_scan_power_blocking(this->device, serials.Data(), powers.Data(), powers.ByteLength());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return powers;
    }
    //===========================================

    Napi::Value getRawRadioExtraScanResults(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        size_t length = info[0].As<Napi::Number>().Int64Value();
        AsphodelExtraScanResult_t *arr = new AsphodelExtraScanResult_t[length];
        int result = asphodel_get_raw_radio_extra_scan_results_blocking(this->device, arr, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Array buf = Napi::Array::New(info.Env(), length);

        for (size_t i = 0; i < length; i++)
        {
            Napi::Object ob = Napi::Object::New(info.Env());

            ob.Set("asphodel_type", arr[i].asphodel_type);
            ob.Set("device_mode", arr[i].device_mode);
            ob.Set("serial_number", arr[i].serial_number);
        }

        delete[] arr;
        return buf;
    }

    Napi::Value getRadioExtraScanResults(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        size_t length = info[0].As<Napi::Number>().Int64Value();
        AsphodelExtraScanResult_t *arr = nullptr;
        int result = asphodel_get_radio_extra_scan_results_blocking(this->device, &arr, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Array buf = Napi::Array::New(info.Env(), length);

        for (size_t i = 0; i < length; i++)
        {
            Napi::Object ob = Napi::Object::New(info.Env());

            ob.Set("asphodel_type", arr[i].asphodel_type);
            ob.Set("device_mode", arr[i].device_mode);
            ob.Set("serial_number", arr[i].serial_number);
        }

        // memcpy(buf.Data(), arr, length * (sizeof(uint32_t)));
        asphodel_free_radio_extra_scan_results(arr);
        return buf;
    }

    Napi::Value getRadioScanResults(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        size_t length = info[0].As<Napi::Number>().Int64Value();
        uint32_t *arr = nullptr;
        int result = asphodel_get_radio_scan_results_blocking(this->device, &arr, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Uint32Array buf = Napi::Uint32Array::New(info.Env(), length);
        memcpy(buf.Data(), arr, length * (sizeof(uint32_t)));
        asphodel_free_radio_scan_results(arr);
        return buf;
    }

    Napi::Value getRawRadioScanResults(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        size_t length = info[0].As<Napi::Number>().Int64Value();
        uint32_t *arr = new uint32_t[length];
        int result = asphodel_get_raw_radio_scan_results_blocking(this->device, arr, &length);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Uint32Array buf = Napi::Uint32Array::New(info.Env(), length);
        memcpy(buf.Data(), arr, length * (sizeof(uint32_t)));
        delete[] arr;
        return buf;
    }

    Napi::Value startRadioScan(const Napi::CallbackInfo &info)
    {
        int result = asphodel_start_radio_scan_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value stopRadio(const Napi::CallbackInfo &info)
    {
        int result = asphodel_stop_radio_blocking(this->device);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Value();
    }

    Napi::Value getTcpAdV(const Napi::CallbackInfo &info)
    {
        Asphodel_TCPAdvInfo_t *t = asphodel_tcp_get_advertisement(this->device);
        Napi::Object ob = Napi::Object::New(info.Env());
        if (t != nullptr)
        {
            ob.Set("tcp_version", Napi::Number::From<uint8_t>(info.Env(), t->tcp_version));
            ob.Set("connected", Napi::Number::From<uint8_t>(info.Env(), t->connected));
            ob.Set("max_incoming_param_length", Napi::Number::From<size_t>(info.Env(), t->max_incoming_param_length));
            ob.Set("max_outgoing_param_length", Napi::Number::From<size_t>(info.Env(), t->max_outgoing_param_length));
            ob.Set("stream_packet_length", Napi::Number::From<size_t>(info.Env(), t->stream_packet_length));
            ob.Set("protocol_type", Napi::Number::From<int>(info.Env(), t->protocol_type));
            ob.Set("serial_number", Napi::String::New(info.Env(), t->serial_number));
            ob.Set("board_rev", Napi::Number::From<uint8_t>(info.Env(), t->board_rev));
            ob.Set("board_type", Napi::String::New(info.Env(), t->board_type));
            ob.Set("build_info", Napi::String::New(info.Env(), t->build_info));
            ob.Set("build_date", Napi::String::New(info.Env(), t->build_date));
            ob.Set("user_tag1", Napi::String::New(info.Env(), t->user_tag1));
            ob.Set("user_tag2", Napi::String::New(info.Env(), t->user_tag2));
            ob.Set("remote_stream_packet_length", Napi::Number::From<size_t>(info.Env(), t->remote_stream_packet_length));
            ob.Set("remote_max_outgoing_param_length", Napi::Number::From<size_t>(info.Env(), t->remote_max_incoming_param_length));
            ob.Set("remote_max_incoming_param_length", Napi::Number::From<size_t>(info.Env(), t->remote_max_outgoing_param_length));
        }
        return ob;
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
        // this->errorCbTrigger(this->device, 78, this);
        // printf("done triggering error callback\n");
        // this->device->close_device(this->device);
        // this->device->free_device(this->device);
    }

private:
    AsphodelDevice_t *device;
};