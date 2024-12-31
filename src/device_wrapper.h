#include <../asphodel-headers/asphodel.h>
#include <napi.h>

class DeviceWrapper : public Napi::ObjectWrap<DeviceWrapper>
{
public:
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
        this->device = dev;
    }

    static Napi::Function GetClass(Napi::Env env)
    {
        Napi::Function fun = DefineClass(env, "Device",
                                         {InstanceMethod("close", &DeviceWrapper::Close),
                                          InstanceMethod("supportsRadioCommands", &DeviceWrapper::asphodelSupportsRadioCommands),
                                          InstanceMethod("supportsRemoteCommands", &DeviceWrapper::asphodelSupportsRadioCommands),
                                          InstanceMethod("supportsBootloaderCommands", &DeviceWrapper::asphodelSupportsBootloaderCommands),
                                          InstanceMethod("supportsRFPowerCommands", &DeviceWrapper::asphodelSupportsRFCommands),
                                          InstanceMethod("getProtocalVersion", &DeviceWrapper::getProtocalV),
                                          InstanceMethod("getBoardInfo", &DeviceWrapper::getBoardInfo),
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

                                          InstanceMethod("getProtocalVersionString", &DeviceWrapper::getProtocalVstr)});

        return fun;
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
        return Napi::Number::From<uint16_t>(info.Env(), version);
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
        char buffer[128];
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
        this->device->close_device(this->device);
        this->device->free_device(this->device);
    }

private:
    AsphodelDevice_t *device;
};