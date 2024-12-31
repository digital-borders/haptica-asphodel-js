#include <../asphodel-headers/asphodel.h>
#include<napi.h>

class DeviceWrapper: public Napi::ObjectWrap<DeviceWrapper> {
    public:
    DeviceWrapper(const Napi::CallbackInfo& info): Napi::ObjectWrap<DeviceWrapper>(info){
        if(info.Length()!=1){
            Napi::Error::New(info.Env(), "Requires device argument").ThrowAsJavaScriptException();
        }

        AsphodelDevice_t *dev = info[0]
        .As<Napi::Object>()
        .Get("dev").As<Napi::External<AsphodelDevice_t>>()
        .Data();
        this->device = dev;
    }

    static Napi::Function GetClass(Napi::Env env) {
        Napi::Function fun  = DefineClass(env,"Device", {
            InstanceMethod("close", &DeviceWrapper::Close),
        });

        return fun;
    }

    Napi::Value Close(const Napi::CallbackInfo&) {
        this->device->close_device(this->device);
        return Napi::Value();
    }

    ~DeviceWrapper() {
        this->device->close_device(this->device);
        this->device->free_device(this->device);
    }

    private:
    AsphodelDevice_t *device;
};