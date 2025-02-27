import { ChannelInfo, createDeviceDecoder, Device, DeviceDecoder, getStreamingCounts, StreamAndChannels, StreamInfo, USBDeInit, USBFindDevices, USBInit, USBPollDevices } from "../haptica-asphodel-js";

type DeviceInfo = {
    decoder: DeviceDecoder,
    stream_count: number,
    info_array: StreamAndChannels[],
    serial_number: string
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
    let decs = decoder.getDecoders();

    let serial_number = device.getSerialNumber();

    decs.forEach((dec, i) => {
        dec.setLostPacketCallback((current, last) => {
            console.log(`Lost ${current - last - 1} packets from ${serial_number} stream ${i} `)
        })
    })


    return {
        stream_count: stream_count.count,
        serial_number: serial_number,
        decoder: decoder,
        info_array: stream_infos
    }

}

async function main() {
        let devices = USBFindDevices();

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
main();
USBDeInit();