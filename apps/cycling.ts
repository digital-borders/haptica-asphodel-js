import { ApdBuilder, ChannelDecoder, ChannelInfo, createDeviceDecoder, Device, DeviceDecoder, deviceToString, getStreamingCounts, StreamAndChannels, StreamInfo, TCPDeinit, TCPFindDevices, TCPInit, UnitFormatter, USBDeInit, USBFindDevices, USBInit } from "../haptica-asphodel-js";

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

function make2DArray(array: Float64Array, rows: number, columns: number, out: Float64Array[]) {
    if (array.length != rows * columns) throw "length mismatch"
    for (let i = 0; i < columns; i++) {
        let beg = i * rows;
        out.push(array.slice(beg, beg + rows))
    }
    return out
}

class DeviceData {
    serial_number: string
    streams: { channel: number, values: Float64Array[] }[][]

    constructor() {
        this.streams = []
    }
}

function createDeviceInfo(device: Device, out: DeviceData | null): DeviceInfo {
    let stream_count = device.getStreamCount();
    let stream_infos: StreamAndChannels[] = [];
    for (let i = 0; i < stream_count.count; i++) {
        let stream = device.getStream(i);
        let channels: ChannelInfo[] = []
        let stream_info = stream.getInfo();

        if (out) {
            out.streams[i] = []
        }

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
    if (out) out.serial_number = serial_number;

    decoder.setUnknownIDCallback((id) => {
        console.log(`Unknown stream id ${id} on ${serial_number}`)
    })

    let decs = decoder.getDecoders();


    decs.forEach((dec, i) => {
        dec.setLostPacketCallback((current, last) => {
            console.log(`Lost ${current - last - 1} from ${serial_number} stream ${i} `)
        })

        dec.getDecoders().forEach((channel_decoder, j) => {
            if (out) channel_decoder.setDecodeCallback((counter, data, samples, subchannels) => {
                let arr = []
                make2DArray(data, samples, subchannels, arr);
                out.streams[i].push({
                    channel: j, values: arr
                });
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


async function checkSensorsConnected(device: Device, timeout: number) {
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
                    remote.waitForConnect(1000);
                    remote_devices.push(remote);
                })
                resolve(true)
            }, timeout)
        })
    }
    return remote_devices
}


function aquireData(device: Device, time: number) {


    var samples = new DeviceData();
    var device_info = createDeviceInfo(device, samples);


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

    while (Date.now() - begin < time) {
        try {
            device.poll(1000)
        } catch (e) {
            console.error(e)
        }
    }


    console.log(`Disabling ${device_info.stream_count} streams from ${device_info.serial_number}`)
    for (let j = 0; j < device_info.stream_count; j++) {
        device.enableStream(j, false);
    }

    device.stopStreamingPackets();
    device.poll(10);

    return samples
}

function aquireDataSaving(device: Device, time: number, schedule_id: string) {
    var device_info = createDeviceInfo(device, null);

    var streams_to_activate: number[] = []
    for (let i = 0; i < device_info.stream_count; i++) {
        streams_to_activate.push(i)
    }

    let response_time = 0.100; // 100 milliseconds
    let buffer_time = 0.500; // 500 milliseconds
    let timeout = 1000; // 1000 milliseconds
    let streaming_counts = getStreamingCounts(device_info.info_array, response_time, buffer_time, timeout)

    let apd = new ApdBuilder(device, streams_to_activate, streaming_counts, schedule_id)

    console.log(`Enabling ${device_info.stream_count} streams from ${device_info.serial_number}`)

    console.log("Transfer count: ", streaming_counts.transfer_count);

    device.startStreamingPackets(streaming_counts.packet_count,
        streaming_counts.transfer_count,
        streaming_counts.timeout, (status, stream_data) => {
            if (status == 0) {
                apd.update(stream_data)
            } else {
                console.log(`Bad status ${status} in streaming packet callback`);
            }
        });

    for (let j = 0; j < device_info.stream_count; j++) {
        device.enableStream(j, true);
    }

    let begin = Date.now();

    while (Date.now() - begin < time) {
        console.log("polling data...")
        try {
            device.poll(1000)
        } catch (e) {
            //console.error(e)
        }
    }


    console.log(`Disabling ${device_info.stream_count} streams from ${device_info.serial_number}`)
    for (let j = 0; j < device_info.stream_count; j++) {
        console.log("   disable: ", j)
        device.enableStream(j, false);
    }

    device.stopStreamingPackets();
    device.poll(10);
    console.log("=============== done aquiring data ================")
    return apd
}

function checkAllConnectedReceivers() {
    return USBFindDevices().concat(TCPFindDevices());
}


const path_to_config = "";

type Config = {
    mqtt: {
        host: string, // The host of the mqtt broker
        port: number, // The port of the mqtt broker
        username: string, // The username of the mqtt broker
        password: string, // The password of the mqtt broker
        baseTopic: string // The base topic to publish the data to
    },
    devices: [
        {
            type: string, // The type of the device
            receiver: string, // The id of the receiver the sensor is connected to
            sensor: string, // The id of the sensor
            sensorChannel: number, // The channel of the sensor
            machine: string, // The id of the machine the sensor is attached to (physical location)
            duration: number, // The duration of the acquisition in seconds
            failure_delay: number, // The delay in seconds before retrying the acquisition in case of failure
            cron_start: string, // The cron expression for the start of the acquisition
            operations: [
                // The operations to perform on the data
                any
            ],
            mqttTopic: string // The mqtt topic to publish the data to
        }
    ]
}

import * as fs from "fs"

async function main() {
    const devices = checkAllConnectedReceivers();

    var config_str: string = fs.readFileSync(path_to_config).toString();
    var config: Config = JSON.parse(config_str);

    for (let config_device of config.devices) {
        var actual_device = devices.find((dev) => (dev.getSerialNumber() == config_device.receiver))
        if (!actual_device) throw `device ${config_device.receiver} not found.`;
        actual_device.open();

        //var error_interval = setInterval(() => {
        var sensors = await checkSensorsConnected(actual_device as Device, 1000);
                //.then((sensors) => {
                    var actual_sensor = sensors.find((sensor) => sensor.getSerialNumber() == config_device.sensor)
                    if (actual_sensor == undefined) throw `sensor ${config_device.sensor} not connected to ${(actual_device as Device).getSerialNumber()}.`;
                    //clearInterval(error_interval)

                    config_device.operations.forEach((op)=>{
                        switch(op.operation) {
                            case "save":
                                const params:  {
                                    "path": string, // The path to save the data
                                    "type": "raw"|"processed" // The type of the data: raw is the .apd file with the raw data from sensor at full speed
                                  } = op.params;

                                if(params.type == "raw") {
                                    var apd = aquireDataSaving(
                                        actual_sensor as Device,
                                        config_device.duration, "");
                                    apd.finalFile(params.path);
                                } else throw `processed output not implemented.`
                                break
                            default: throw `unhandled operation: ${op.operation}.`
                        }
                    })
                //})
                //.catch((e) => {
                //        console.log(`Error ${e.toString()}: waiting for ${config_device.failure_delay} before retrying...`)
                //        var bgin = Date.now();
                //        while(Date.now() - bgin < config_device.failure_delay){};
                //})
        //})

    }
}


init()
main().then(() => {
    console.log("main returnrd")
    deinit()
})

