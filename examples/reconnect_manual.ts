import { ChannelInfo, createDeviceDecoder, Device, DeviceDecoder, StreamAndChannels, StreamInfo, USBFindDevices, USBInit } from "../haptica-asphodel-js";

type DeviceInfo = {
    decoder: DeviceDecoder,
    stream_count: number,
    info_array: StreamAndChannels[],
    serial_number: string
}

function createDeviceInfo(device: Device):DeviceInfo {
    let stream_count = device.getStreamCount();
    let stream_infos: StreamAndChannels[] = [];
    for(let i = 0; i < stream_count.count; i++) {
        let stream = device.getStream(i);
        let channels: ChannelInfo[] = []
        let stream_info = stream.getInfo();
        for(let j = 0; j < stream_info.channel_count; j++) {
            let channel_index = stream_info.channel_index_list[j];
            channels.push(device.getChannel(channel_index))
        }
        stream_infos.push(new StreamAndChannels(i, stream, channels))
    }

    return {
        stream_count: stream_count.count,
        serial_number: device.getSerialNumber(),
        decoder: createDeviceDecoder(stream_infos,stream_count.filler_bits, stream_count.id_bits),
        info_array: stream_infos
    }
}

function main() {
    USBInit()

    while(true) {
        let devices = USBFindDevices(10);

        console.log(`Found ${devices.length} devices`);

        let device_info_array: DeviceInfo[] = [];
        for(let device of devices) {
            device.open();
            let device_info = createDeviceInfo(device);
            device_info_array.push(device_info);
        }
    }
}

main();