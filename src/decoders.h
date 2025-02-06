#include <napi.h>
#include <../asphodel-headers/asphodel.h>

class ChannelDecoder : public Napi::ObjectWrap<ChannelDecoder>
{
    AsphodelChannelDecoder_t *decoder;
    Napi::FunctionReference decode_callback;

public:
    ChannelDecoder(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ChannelDecoder>(info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        AsphodelChannelDecoder_t *decoder = info[0].As<Napi::Object>().Get("decoder").As<Napi::External<AsphodelChannelDecoder_t>>().Data();
        this->decode_callback = Napi::FunctionReference();
        this->decoder = decoder;
    }

    static Napi::Function getClass(Napi::Env env)
    {
        return DefineClass(env, "ChannelDecoder", {
                                                      InstanceMethod("decode", &ChannelDecoder::decode),
                                                      InstanceMethod("setConversionFactor", &ChannelDecoder::setConversionFactor),
                                                      InstanceMethod("reset", &ChannelDecoder::reset),
                                                      InstanceMethod("getChannelBitOffset", &ChannelDecoder::getChannelBitOffset),
                                                      InstanceMethod("getSamples", &ChannelDecoder::getSamples),
                                                      InstanceMethod("getSubChannels", &ChannelDecoder::getSubchannels),
                                                      InstanceMethod("getSubChannelNames", &ChannelDecoder::getSubchannelNames),
                                                      InstanceMethod("getChannelName", &ChannelDecoder::getChannelName),
                                                      InstanceMethod("setDecodeCallback", &ChannelDecoder::setDecodeCallback),
                                                  });
    }

    Napi::Value decode(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        int64_t counter = info[0].As<Napi::Number>().Int64Value();
        Napi::Uint8Array buffer = info[1].As<Napi::Uint8Array>();
        this->decoder->decode(this->decoder, counter, buffer.Data());
        return Napi::Value();
    }

    Napi::Value setConversionFactor(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 2)
        {
            Napi::Error::New(info.Env(), "Expects 2 arguments").ThrowAsJavaScriptException();
        }
        double scale = info[0].As<Napi::Number>().DoubleValue();
        double offset = info[1].As<Napi::Number>().DoubleValue();
        this->decoder->set_conversion_factor(this->decoder, scale, offset);
        return Napi::Value();
    }

    Napi::Value reset(const Napi::CallbackInfo &info)
    {
        this->decoder->reset(this->decoder);
        return Napi::Value();
    }

        Napi::Value getChannelName(const Napi::CallbackInfo &info)
    {
        return Napi::String::New(info.Env(), this->decoder->channel_name);
    }

    Napi::Value getChannelBitOffset(const Napi::CallbackInfo &info)
    {
        return Napi::Number::From<uint16_t>(info.Env(), this->decoder->channel_bit_offset);
    }

    Napi::Value getSamples(const Napi::CallbackInfo &info)
    {
        return Napi::Number::From<size_t>(info.Env(), this->decoder->samples);
    }

    Napi::Value getSubchannels(const Napi::CallbackInfo &info)
    {
        return Napi::Number::From<size_t>(info.Env(), this->decoder->subchannels);
    }

    Napi::Value getSubchannelNames(const Napi::CallbackInfo &info)
    {
        Napi::Array arr = Napi::Array::New(info.Env(), this->decoder->subchannels);
        for (size_t i = 0; i < this->decoder->subchannels; i++)
        {
            arr[i] = Napi::String::New(info.Env(), this->decoder->subchannel_names[i]);
        }
        return arr;
    }

    Napi::Value setDecodeCallback(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }

        Napi::Function cb = info[0].As<Napi::Function>();

        if (!this->decode_callback.IsEmpty())
        {
            this->decode_callback.Reset();
        }

        this->decode_callback = Napi::Persistent(cb);
        this->decoder->callback = decodeCallback;
        this->decoder->closure = this;
        return Napi::Value();
    }

    static void decodeCallback(uint64_t counter, double *data, size_t samples, size_t subchannels, void *closure)
    {
        ChannelDecoder *dec = static_cast<ChannelDecoder *>(closure);
        if (!dec->decode_callback.IsEmpty())
        {
            Napi::Float64Array f = Napi::Float64Array::New(dec->decode_callback.Env(), samples * subchannels);
            memcpy(f.Data(), data, (samples * subchannels) * sizeof(double));
            dec->decode_callback.Call({Napi::Number::From<uint64_t>(dec->decode_callback.Env(), counter),
                                       f,
                                       Napi::Number::From<size_t>(dec->decode_callback.Env(), samples),
                                       Napi::Number::From<size_t>(dec->decode_callback.Env(), subchannels)});
        }
    }

    ~ChannelDecoder()
    {
        if (!this->decode_callback.IsEmpty())
        {
            this->decode_callback.Reset();
        }
        this->decoder->free_decoder(this->decoder);
    }
};

class StreamDecoder : public Napi::ObjectWrap<StreamDecoder>
{
    AsphodelStreamDecoder_t *decoder;
    Napi::FunctionReference lost_packet_callback;

public:
    StreamDecoder(const Napi::CallbackInfo &info) : Napi::ObjectWrap<StreamDecoder>(info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        AsphodelStreamDecoder_t *decoder = info[0].As<Napi::Object>().Get("decoder").As<Napi::External<AsphodelStreamDecoder_t>>().Data();
        this->lost_packet_callback = Napi::FunctionReference();
        this->decoder = decoder;
    }

    static Napi::Function getClass(Napi::Env env)
    {
        return DefineClass(env, "StreamDecoder", {
                                                     InstanceMethod("decode", &StreamDecoder::decode),
                                                     InstanceMethod("reset", &StreamDecoder::reset),
                                                     InstanceMethod("getLastCount", &StreamDecoder::getLastCount),
                                                     InstanceMethod("getCounterByteOffset", &StreamDecoder::getCBO),
                                                     InstanceMethod("getChannels", &StreamDecoder::getChannels),
                                                     InstanceMethod("setLostPacketCallback", &StreamDecoder::setLostPacketCallback),
                                                     InstanceMethod("getDecoders", &StreamDecoder::getDecoders),
                                                     InstanceMethod("getUsedBits", &StreamDecoder::getUsedBits),
                                                 });
    }

    Napi::Value getUsedBits(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), this->decoder->used_bits);
    }

    Napi::Value getLastCount(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), this->decoder->last_count);
    }

    Napi::Value getCBO(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), this->decoder->counter_byte_offset);
    }

    Napi::Value getChannels(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), this->decoder->channels);
    }

    Napi::Value reset(const Napi::CallbackInfo &info)
    {
        this->decoder->reset(this->decoder);
        return Napi::Value();
    }

    Napi::Value getDecoders(const Napi::CallbackInfo &info)
    {
        Napi::Array arr = Napi::Array::New(info.Env(), this->decoder->channels);
        for (size_t i = 0; i < this->decoder->channels; i++)
        {
            Napi::Function constr = ChannelDecoder::getClass(info.Env());
            Napi::Object ob = Napi::Object::New(info.Env());
            ob.Set("decoder", Napi::External<AsphodelChannelDecoder_t>::New(info.Env(), this->decoder->decoders[i]));
            arr[i] = constr.New({ob});
        }
        return arr;
    }

    Napi::Value decode(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Uint8Array buffer = info[0].As<Napi::Uint8Array>();
        this->decoder->decode(this->decoder, buffer.Data());
        return Napi::Value();
    }

    Napi::Value setLostPacketCallback(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }

        Napi::Function cb = info[0].As<Napi::Function>();

        if (!this->lost_packet_callback.IsEmpty())
        {
            this->lost_packet_callback.Reset();
        }

        this->lost_packet_callback = Napi::Persistent(cb);
        this->decoder->lost_packet_callback = lostPacketCallback;
        this->decoder->lost_packet_closure = this;
        return Napi::Value();
    }

    static void lostPacketCallback(uint64_t current, uint64_t last, void *closure)
    {
        StreamDecoder *dec = static_cast<StreamDecoder *>(closure);
        if (!dec->lost_packet_callback.IsEmpty())
        {
            dec->lost_packet_callback.Call({
                Napi::Number::New(dec->lost_packet_callback.Env(), current),
                Napi::Number::New(dec->lost_packet_callback.Env(), last),
            });
        }
    }

    ~StreamDecoder()
    {
        if (!this->lost_packet_callback.IsEmpty())
        {
            this->lost_packet_callback.Reset();
        }
        this->decoder->free_decoder(this->decoder);
    }
};

class DeviceDecoder : public Napi::ObjectWrap<DeviceDecoder>
{
    AsphodelDeviceDecoder_t *decoder;
    Napi::FunctionReference unknown_id_callback;

public:
    DeviceDecoder(const Napi::CallbackInfo &info) : Napi::ObjectWrap<DeviceDecoder>(info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        AsphodelDeviceDecoder_t *decoder = info[0].As<Napi::Object>().Get("decoder").As<Napi::External<AsphodelDeviceDecoder_t>>().Data();
        this->unknown_id_callback = Napi::FunctionReference();
        this->decoder = decoder;
    }

    static Napi::Function getClass(Napi::Env env)
    {
        return DefineClass(env, "StreamDecoder", {
                                                     InstanceMethod("decode", &DeviceDecoder::decode),
                                                     InstanceMethod("reset", &DeviceDecoder::reset),
                                                     InstanceMethod("getIDByteOffset", &DeviceDecoder::getCBO),
                                                     InstanceMethod("getStreams", &DeviceDecoder::getStreams),
                                                     InstanceMethod("setUnknownIDCallback", &DeviceDecoder::setUnknownIDCallback),
                                                     InstanceMethod("getDecoders", &DeviceDecoder::getDecoders),
                                                     InstanceMethod("getUsedBits", &DeviceDecoder::getUsedBits),
                                                     InstanceMethod("getStreamIDs", &DeviceDecoder::getStreamIDs),
                                                 });
    }

    Napi::Value getUsedBits(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), this->decoder->used_bits);
    }

    Napi::Value getStreamIDs(const Napi::CallbackInfo &info)
    {
        Napi::Uint8Array arr = Napi::Uint8Array::New(info.Env(), this->decoder->streams);
        memcpy(arr.Data(), this->decoder->stream_ids, this->decoder->streams);
        return arr;
    }

    Napi::Value getCBO(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), this->decoder->id_byte_offset);
    }

    Napi::Value getStreams(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), this->decoder->streams);
    }

    Napi::Value reset(const Napi::CallbackInfo &info)
    {
        this->decoder->reset(this->decoder);
        return Napi::Value();
    }

    Napi::Value getDecoders(const Napi::CallbackInfo &info)
    {
        Napi::Array arr = Napi::Array::New(info.Env(), this->decoder->streams);
        for (size_t i = 0; i < this->decoder->streams; i++)
        {
            Napi::Function constr = StreamDecoder::getClass(info.Env());
            Napi::Object ob = Napi::Object::New(info.Env());
            ob.Set("decoder", Napi::External<AsphodelStreamDecoder_t>::New(info.Env(), this->decoder->decoders[i]));
            arr[i] = constr.New({ob});
        }
        return arr;
    }

    Napi::Value decode(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Uint8Array buffer = info[0].As<Napi::Uint8Array>();
        this->decoder->decode(this->decoder, buffer.Data());
        return Napi::Value();
    }

    Napi::Value setUnknownIDCallback(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }

        Napi::Function cb = info[0].As<Napi::Function>();

        if (!this->unknown_id_callback.IsEmpty())
        {
            this->unknown_id_callback.Reset();
        }

        this->unknown_id_callback = Napi::Persistent(cb);
        this->decoder->unknown_id_callback = unknownIdCallback;
        this->decoder->unknown_id_closure = this;
        return Napi::Value();
    }

    static void unknownIdCallback(uint8_t id, void *closure)
    {
        DeviceDecoder *dec = static_cast<DeviceDecoder *>(closure);
        if (!dec->unknown_id_callback.IsEmpty())
        {
            dec->unknown_id_callback.Call({Napi::Number::New(dec->unknown_id_callback.Env(), id)});
        }
    }

    ~DeviceDecoder()
    {
        if (!this->unknown_id_callback.IsEmpty())
        {
            this->unknown_id_callback.Reset();
        }
        this->decoder->free_decoder(this->decoder);
    }
};

class ChannelInfo : public Napi::ObjectWrap<ChannelInfo>
{
public:
    AsphodelChannelInfo_t *channel_info;
    bool to_free;

    ChannelInfo(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ChannelInfo>(info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Object ob = info[0].As<Napi::Object>();
        AsphodelChannelInfo_t *chinfo = ob.Get("info").As<Napi::External<AsphodelChannelInfo_t>>().Data();
        this->to_free = ob.Get("tofree").As<Napi::Boolean>().Value();
        this->channel_info = chinfo;
    }

    static Napi::Function GetClass(Napi::Env env)
    {
        return DefineClass(env, "ChannelInfo", {
                                                   InstanceMethod("getInfo", &ChannelInfo::getInfo),
                                                   InstanceMethod("checkAccelSelfTest", &ChannelInfo::checkAccelSelfTest),
                                                   InstanceMethod("getAccelSelfTestLimits", &ChannelInfo::getAccelSelfTestLimits),
                                                   InstanceMethod("checkStrainResistances", &ChannelInfo::checkStrainResistances),
                                                   InstanceMethod("getStrainBridgeValues", &ChannelInfo::getStrainBridgeValues),
                                                   InstanceMethod("getStrainBridgeSubchannel", &ChannelInfo::getStrainBridgeSubchannel),
                                                   InstanceMethod("getStrainBridgeCount", &ChannelInfo::getStrainBridgeCount),
                                                
                                               });
    }

        Napi::Value checkAccelSelfTest(const Napi::CallbackInfo &info)
    {

        double disabled = 0;
        double enabled = 0;
        int passed = 0;

        int result = asphodel_check_accel_self_test(this->channel_info, &disabled, &enabled, &passed);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("disabled", disabled);
        ob.Set("enabled", enabled);
        ob.Set("passed", passed);
        return ob;
    }

    Napi::Value getAccelSelfTestLimits(const Napi::CallbackInfo &info)
    {
        Napi::Float32Array arr = Napi::Float32Array::New(info.Env(), 6);
        int result = asphodel_get_accel_self_test_limits(this->channel_info, arr.Data());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return arr;
    }

    Napi::Value checkStrainResistances(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 4)
        {
            Napi::Error::New(info.Env(), "Expects 4 arguments").ThrowAsJavaScriptException();
        }
        int bridge_index = info[0].As<Napi::Number>().Int32Value();
        double baseline = info[1].As<Napi::Number>().DoubleValue();
        double positive_high = info[2].As<Napi::Number>().DoubleValue();
        double negative_high = info[3].As<Napi::Number>().DoubleValue();

        double positive_resistance = 0;
        double negative_resistance = 0;
        int passed = 0;

        int result = asphodel_check_strain_resistances(this->channel_info, bridge_index, baseline, positive_high, negative_high, &positive_resistance, &negative_resistance, &passed);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("positive_resistance", positive_resistance);
        ob.Set("negative_resistance", negative_resistance);
        ob.Set("passed", passed);
        return ob;
    }

    Napi::Value getStrainBridgeValues(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int bridge_index = info[0].As<Napi::Number>().Int32Value();
        Napi::Float32Array arr = Napi::Float32Array::New(info.Env(), 6);
        int result = asphodel_get_strain_bridge_values(this->channel_info, bridge_index, arr.Data());
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return arr;
    }

    Napi::Value getStrainBridgeSubchannel(const Napi::CallbackInfo &info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        int bridge_index = info[0].As<Napi::Number>().Int32Value();
        size_t index = 0;
        int result = asphodel_get_strain_bridge_subchannel(this->channel_info, bridge_index, &index);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::New(info.Env(), index);
    }

    Napi::Value getStrainBridgeCount(const Napi::CallbackInfo &info)
    {
        int count = 0;
        int result = asphodel_get_strain_bridge_count(this->channel_info, &count);
        if (result != 0)
        {
            Napi::Error::New(info.Env(), asphodel_error_name(result)).ThrowAsJavaScriptException();
        }
        return Napi::Number::New(info.Env(), count);
    }

    Napi::Value getInfo(const Napi::CallbackInfo &info)
    {
        auto channel = this->channel_info;
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("bits_per_sample", channel->bits_per_sample);
        ob.Set("channel_type", channel->channel_type);
        //Napi::Array chunks = Napi::Array::New(info.Env(), channel->chunk_count);
        //for (int i = 0; i < channel->chunk_count; i++)
        //{
            //asphodel_get_channel_chunk_blocking()
            //printf("============================= %d %d\n", i, channel->chunk_lengths);
            //Napi::Uint8Array chunk = Napi::Uint8Array::New(info.Env(), channel->chunk_lengths[i]);
            //memcpy(chunk.Data(), channel->chunks[i], channel->chunk_lengths[i]);
            //chunks[i] = chunk;
        //}
        //ob.Set("chunks", chunks);
        //Napi::Float32Array coefs = Napi::Float32Array::New(info.Env(), channel->coefficients_length);
        //for (int i = 0; i < channel->coefficients_length; i++)
        //{
        //    coefs[i] = channel->coefficients[i];
        //}
        
        //ob.Set("coefficients", coefs);
        ob.Set("data_bits", channel->data_bits);
        ob.Set("filler_bits", channel->filler_bits);
        ob.Set("maximum", channel->maximum);
        ob.Set("minimum", channel->minimum);
        ob.Set("resolution", channel->resolution);
        ob.Set("samples", channel->samples);
        ob.Set("unit_type", channel->unit_type);
        ob.Set("chunk_count", channel->chunk_count);
        //ob.Set("name", Napi::String::New(info.Env(), (char *)channel->name, channel->name_length));
        return ob;
    }

    ~ChannelInfo()
    {
        if (this->to_free)
        {
            asphodel_free_channel(this->channel_info);
        }
        else
        {
            delete this->channel_info;
        }
    }
};

class StreamInfo : public Napi::ObjectWrap<StreamInfo>
{
public:
    AsphodelStreamInfo_t *stream_info;
    bool to_free;

    StreamInfo(const Napi::CallbackInfo &info) : Napi::ObjectWrap<StreamInfo>(info)
    {
        if (info.Length() != 1)
        {
            Napi::Error::New(info.Env(), "Expects 1 arguments").ThrowAsJavaScriptException();
        }
        Napi::Object ob = info[0].As<Napi::Object>();
        AsphodelStreamInfo_t *chinfo = ob.Get("info").As<Napi::External<AsphodelStreamInfo_t>>().Data();
        this->to_free = ob.Get("tofree").As<Napi::Boolean>().Value();
        this->stream_info = chinfo;
    }

    static Napi::Function GetClass(Napi::Env env)
    {
        return DefineClass(env,"StreamInfo", {InstanceMethod("getInfo", &StreamInfo::getInfo)});
    }

    Napi::Value getInfo(const Napi::CallbackInfo &info)
    {
        AsphodelStreamInfo_t *stream = this->stream_info;
        Napi::Object ob = Napi::Object::New(info.Env());
        ob.Set("channel_count", stream->channel_count);
        auto a = Napi::Uint8Array::New(info.Env(), stream->channel_count);
        memcpy(a.Data(), stream->channel_index_list, stream->channel_count);
        ob.Set("channel_index_list", a);
        ob.Set("counter_bits", stream->counter_bits);
        ob.Set("filler_bits", stream->filler_bits);
        ob.Set("rate", stream->rate);
        ob.Set("rate_error", stream->rate_error);
        ob.Set("warm_up_delay", stream->warm_up_delay);
        return ob;
    }

    ~StreamInfo()
    {
        if (this->to_free)
        {
            asphodel_free_stream(this->stream_info);
        }
        else
        {
            delete this->stream_info;
        }
    }
};

class StreamAndChannels : public Napi::ObjectWrap<StreamAndChannels>
{
public:
    AsphodelStreamAndChannels_t strAndCh = {};

    Napi::Reference<Napi::Object> stream_info;
    Napi::Reference<Napi::Array> channel_infos;
    

    StreamAndChannels(const Napi::CallbackInfo &info) : Napi::ObjectWrap<StreamAndChannels>(info)
    {
        if (info.Length() != 3)
        {
            Napi::Error::New(info.Env(), "Expects 3 arguments").ThrowAsJavaScriptException();
        }

        uint8_t id = info[0].As<Napi::Number>().Uint32Value();
        Napi::Object stream_info = info[1].As<Napi::Object>();

        this->stream_info = Napi::Persistent(stream_info);

        auto *stream_info_instance = StreamInfo::Unwrap(stream_info)->stream_info;

        Napi::Array channel_infos = info[2].As<Napi::Array>();

        this->channel_infos = Napi::Persistent(channel_infos);

        AsphodelChannelInfo_t **channel_info_instances = new AsphodelChannelInfo_t *[channel_infos.Length()];

        for (uint32_t i = 0; i < channel_infos.Length(); i++)
        {
            Napi::Object ob = channel_infos.Get(i).As<Napi::Object>();
            channel_info_instances[i] = ChannelInfo::Unwrap(ob)->channel_info;
        }
        

        this->strAndCh = (AsphodelStreamAndChannels_t){
            .stream_id = id,
            .stream_info = stream_info_instance,
            .channel_info = channel_info_instances,
        };
    }

    Napi::Value laugh(const Napi::CallbackInfo &info)
    {
        printf("hahahahhahah!!!!!!!\n");
        return Napi::Value();
    }

    static Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        Napi::Function f = DefineClass(env, "StreamAndChannels", {
            InstanceMethod("laugh", &StreamAndChannels::laugh),
            InstanceMethod("getStreamInfo", &StreamAndChannels::getStreamInfo),
            InstanceMethod("getChannelInfos", &StreamAndChannels::getChannelInfos),

            });

        Napi::Object *ob = env.GetInstanceData<Napi::Object>();
        Napi::FunctionReference *ctor = new Napi::FunctionReference();
        *ctor = Napi::Persistent(f);
        ob->Set("streamAndChannelConstructor", Napi::External<Napi::FunctionReference>::New(env, ctor, [](Napi::Env, Napi::FunctionReference *ref)
                                                                                            { delete ref; }));
        exports.Set("StreamAndChannels", f);
        return exports;
    }

    Napi::Value getStreamInfo(const Napi::CallbackInfo &info) {
        return this->stream_info.Value();
    }

    Napi::Value getChannelInfos(const Napi::CallbackInfo &info) {
        return this->channel_infos.Value();
    }

    ~StreamAndChannels()
    {
        this->channel_infos.Unref();
        this->stream_info.Unref();
        delete[] this->strAndCh.channel_info;
    }
};