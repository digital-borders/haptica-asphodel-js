
import { ChannelDecoder, ChannelInfo, createDeviceDecoder, Device, DeviceDecoder, getStreamingCounts, StreamAndChannels, StreamInfo, UnitFormatter, USBDeInit, USBFindDevices, USBInit, USBPollDevices } from "../haptica-asphodel-js";

type ChannelClosure = {
    unit_formatter: UnitFormatter,
    counter_time_scale:number,
	sample_time_scale:number
}

type DeviceInfo = {
    decoder: DeviceDecoder,
    stream_count: number,
    info_array: StreamAndChannels[],
    serial_number: string
}

function createChannelClosure(
    serial_number: string,
    stream_info: StreamInfo,
    channel_info: ChannelInfo,
    channel_decoder: ChannelDecoder) {
        let strinfo = stream_info.getInfo();
        let chinfo = channel_info.getInfo();

        let channel_closure: ChannelClosure = {
            unit_formatter: new UnitFormatter(chinfo.unit_type, chinfo.minimum, chinfo.maximum, chinfo.resolution, true),
            counter_time_scale: 0.0,
            sample_time_scale: 0.0
        }

        if(strinfo.rate != 0.0 && channel_decoder.getSamples() != 0) {
            channel_closure.counter_time_scale = 1/(strinfo.rate)
            channel_closure.sample_time_scale = channel_closure.counter_time_scale/channel_decoder.getSamples()
        }

        return channel_closure;
}

function createDeviceInfo(device: Device): DeviceInfo {
    let stream_count = device.getStreamCount();
    let stream_infos: StreamAndChannels[] = [];
    for (let i = 0; i < stream_count.count; i++) {
        let stream = device.getStream(i);
        let channels: ChannelInfo[] = []
        let stream_info = stream.getInfo();

        if (stream_info.channel_count == 0) {
            throw new Error(`Error: stream ${i} has 0 channels`);
        }

        for (let j = 0; j < stream_info.channel_count; j++) {
            let channel_index = stream_info.channel_index_list[j];
            let channel = device.getChannel(channel_index);
            channels.push(channel)
        }
        stream_infos.push(new StreamAndChannels(i, stream, channels))
    }

    let decoder = createDeviceDecoder(stream_infos, stream_count.filler_bits, stream_count.id_bits);

    let serial_number = device.getSerialNumber();

    decoder.setUnknownIDCallback((id) => {
        console.log(`Unknown stream id ${id} on ${serial_number}`)
    })

    let decs = decoder.getDecoders();


    decs.forEach((dec, i) => {
        let stream_info = stream_infos[i].getStreamInfo();
        
        dec.setLostPacketCallback((current, last) => {
            console.log(`Lost ${current - last - 1} from ${serial_number} stream ${i} `)
        })

        dec.getDecoders().forEach((channel_decoder, j) => {
            let channel_info = stream_infos[i].getChannelInfos()[j];
            let channel_closure = createChannelClosure("", stream_info, channel_info, channel_decoder)
            channel_decoder.setConversionFactor(channel_closure.unit_formatter.getConversionScale(), channel_closure.unit_formatter.getConversionOffset())
            channel_decoder.setDecodeCallback((counter, data, samples, subchannels) => {
                for (let sample = 0; sample < samples; sample++) {
                    if(channel_closure.counter_time_scale == 0) {
                        console.log("channel_closure.counter_time_scale == 0", counter);
                    } else {
                        let time = counter*channel_closure.counter_time_scale+sample*channel_closure.sample_time_scale;
                        console.log("channel_closure.counter_time_scale != 0", time)
                    }
                    for(let sub = 0; sub < subchannels; sub++) {
                        console.log(channel_closure.unit_formatter.FormatBare(data[sample*subchannels+sub]))
                    }
                }
            })
        })
    })

    return {
        stream_count: stream_count.count,
        serial_number: device.getSerialNumber(),
        decoder: decoder,
        info_array: stream_infos
    }

}

async function main() {
        let devices = USBFindDevices(10);

        console.log(`Found ${devices.length} devices`);

        if (devices.length == 0) {
            console.log("No Devices Found!...\n");
            return
        }

        let device_info_array: DeviceInfo[] = [];
        for (let device of devices) {
            device.open();
            let device_info = createDeviceInfo(device);
            device_info_array.push(device_info);
        }

        for (let i = 0; i < device_info_array.length; i++) {
            let response_time = 0.100; // 100 milliseconds
            let buffer_time = 0.500; // 500 milliseconds
            let timeout = 1000; // 1000 milliseconds
            let streaming_counts = getStreamingCounts(device_info_array[i].info_array, response_time, buffer_time, timeout)

            console.log(`Enabling ${device_info_array[i].stream_count} streams from ${device_info_array[i].serial_number}`)

            console.log("Transfer count: ", streaming_counts.transfer_count);

            devices[i].startStreamingPackets(streaming_counts.packet_count,
                streaming_counts.transfer_count,
                streaming_counts.timeout, (status, stream_data, packet_size, packet_count) => {
                    if (status == 0) {
                        for (let packet = 0; packet < packet_count; packet++) {
                            device_info_array[i].decoder.decode(stream_data.slice(packet * packet_size))
                        }
                    } else {
                        console.log(`Bad status ${status} in streaming packet callback`);
                    }
                });

                for (let j = 0; j < device_info_array[i].stream_count; j++) {
                    devices[i].enableStream(j, true);
                }
        }

        for(let polls = 0; polls < 50; polls++){
            USBPollDevices(100)
        }

        for (let i = 0; i < devices.length; i++) {
            console.log(`Disabling ${device_info_array[i].stream_count} streams from ${device_info_array[i].serial_number}`)
            for (let j = 0; j < device_info_array[i].stream_count; j++) {
                devices[i].enableStream(j, false);
            }

            devices[i].stopStreamingPackets();
            devices[i].poll(10);
            devices[i].close();
        }
}

USBInit()
main().then(()=>{
    USBDeInit();
})