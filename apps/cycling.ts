import { ChannelDecoder, ChannelInfo, createDeviceDecoder, Device, DeviceDecoder, getStreamingCounts, StreamAndChannels, StreamInfo, TCPDeinit, TCPFindDevices, TCPInit, UnitFormatter, USBDeInit, USBFindDevices, USBInit } from "../haptica-asphodel-js";

function init() {
    USBInit()
    TCPInit();
}

function deinit() {
    USBDeInit()
    TCPDeinit()
}

type ChannelClosure = {
    unit_formatter: UnitFormatter,
    counter_time_scale: number,
    sample_time_scale: number
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

    if (strinfo.rate != 0.0 && channel_decoder.getSamples() != 0) {
        channel_closure.counter_time_scale = 1 / (strinfo.rate)
        channel_closure.sample_time_scale = channel_closure.counter_time_scale / channel_decoder.getSamples()
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
            let channel_closure = createChannelClosure(serial_number, stream_info, channel_info, channel_decoder)
            channel_decoder.setConversionFactor(channel_closure.unit_formatter.getConversionScale(), channel_closure.unit_formatter.getConversionOffset())
            channel_decoder.setDecodeCallback((counter, data, samples, subchannels) => {
                for (let sample = 0; sample < samples; sample++) {
                    if (channel_closure.counter_time_scale == 0) {
                        console.log("channel_closure.counter_time_scale == 0", counter);
                    } else {
                        let time = counter * channel_closure.counter_time_scale + sample * channel_closure.sample_time_scale;
                        console.log("channel_closure.counter_time_scale != 0", time)
                    }
                    for (let sub = 0; sub < subchannels; sub++) {
                        console.log(data[sample * subchannels + sub])
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


async function checkSensorsConnected(device: Device) {
    let remote_devices: Device[] = [];
    if (device.supportsRadioCommands()) {
        console.log("scanning fo remotes devices")
        device.startRadioScan()
        await new Promise((resolve) => {
            setTimeout(() => {
                device.stopRadio()
                let serials = device.getRadioScanResults(255)
                let sorted = serials.sort();
                console.log("Radio Scan results: ", sorted)
                sorted.forEach((serial) => {
                    let remote = device.getRemoteDevice()
                    remote.open()
                    device.connectRadio(serial)

                    try {
                        remote.waitForConnect(1000);
                    } catch (e) {
                        console.error(e);
                        return
                    }
                    remote_devices.push(remote);
                })
                resolve(true)
            }, 1000)
        })
    }
    return remote_devices
}


function aquireData(device: Device, time: number) {
    var device_info = createDeviceInfo(device);


    let response_time = 0.100; // 100 milliseconds
    let buffer_time = 0.500; // 500 milliseconds
    let timeout = 1000; // 1000 milliseconds
    let streaming_counts = getStreamingCounts(device_info.info_array, response_time, buffer_time, timeout)

    console.log(`Enabling ${device_info.stream_count} streams from ${device_info.serial_number}`)

    console.log("Transfer count: ", streaming_counts.transfer_count);

    device.startStreamingPackets(streaming_counts.packet_count,
        streaming_counts.transfer_count,
        streaming_counts.timeout, (status, stream_data, packet_size, packet_count) => {
            if (status == 0) {
                for (let packet = 0; packet < packet_count; packet++) {
                    device_info.decoder.decode(stream_data.slice(packet * packet_size))
                }
            } else {
                console.log(`Bad status ${status} in streaming packet callback`);
            }
        });

    for (let j = 0; j < device_info.stream_count; j++) {
        device.enableStream(j, true);
    }


    //=================================

    let begin = Date.now();

    while(Date.now() - begin < time) {
        device.poll(1000)
    }


    console.log(`Disabling ${device_info.stream_count} streams from ${device_info.serial_number}`)
    for (let j = 0; j < device_info.stream_count; j++) {
        device.enableStream(j, false);
    }

    device.stopStreamingPackets();
    device.poll(10);
}

function checkAllConnectedReceivers() {
    return USBFindDevices(50).concat(TCPFindDevices(50));
}


async function main() {
    init()
    const devices = checkAllConnectedReceivers()
    //devices.forEach(async (element, pos) => {
    for(let i = 0;i < devices.length; i++) {
        let element = devices[i];
        element.open()
        console.log(`device ${i} has serial: ${element.getSerialNumber()}`)
        let sensors = await checkSensorsConnected(element)
        console.log(`sensors found: ${sensors} length ${sensors.length}`)

        console.log("sensor serail number: ", sensors[0].getSerialNumber())

        console.log("acquire dara.....")

        aquireData(sensors[0], 1000);

        element.close()
    }
    //});
    deinit()
}


main().then(()=>{
    console.log("main returnrd")
})

