import { Job } from "bullmq";
import { ApdBuilder, ASPHODEL_PROTOCOL_TYPE_BOOTLOADER, ChannelDecoder, ChannelInfo, createDeviceDecoder, Device, DeviceDecoder, deviceToString, getStreamingCounts, StreamAndChannels, StreamInfo, TCPDeinit, TCPFindDevices, TCPInit, UnitFormatter, USBDeInit, USBFindDevices, USBInit } from "../haptica-asphodel-js";


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
    //serial_number: string
    channel: number
    streams: { counter: number, values: Float64Array[] }[][]
    unit_formatter: UnitFormatter

    constructor(channel: number) {
        this.streams = []
        this.channel = channel;
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
    //if (out) out.serial_number = serial_number;

    decoder.setUnknownIDCallback((id) => {
        console.log(`Unknown stream id ${id} on ${serial_number}`)
    })

    let decs = decoder.getDecoders();


    decs.forEach((dec, i) => {
        let stream_info = stream_infos[i].getStreamInfo();

        dec.setLostPacketCallback((current, last) => {
            console.log(`=================== Lost ${current - last - 1} packets from ${serial_number} stream ${i} ========================`)
        })

        dec.getDecoders().forEach((channel_decoder, j) => {
            if (out == null) return;
            let channel_info = stream_infos[i].getChannelInfos()[j];
            var chinfo = channel_info.getInfo()
            var unit_formatter = new UnitFormatter(chinfo.unit_type, chinfo.minimum, chinfo.maximum, chinfo.resolution, true);
            channel_decoder.setConversionFactor(unit_formatter.getConversionScale(), unit_formatter.getConversionOffset())
            out.unit_formatter = unit_formatter;
            channel_decoder.setDecodeCallback((counter, data, samples, subchannels) => {
                if (j == out.channel) {
                    let arr = []
                    make2DArray(data, samples, subchannels, arr);
                    out.streams[i].push({
                        counter: counter,
                        values: arr
                    });
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


function hasRadioScanPower(device: Device) {
    try {
        device.getRadioScanPower(new Uint32Array([0]))
        return true;
    } catch (e) {
        return false;
    }
}

function collectScanResults(device: Device) {
    var results = device.getRadioExtraScanResults(255)
    console.log(results)

    var scan_powers = new Map()
    if (hasRadioScanPower(device)) {

        var power_max_queries = Math.min(
            Math.floor(device.getMaxOutgoingParamLength() / 4),
            device.getMaxIncomingParamLength()
        )
        //console.log("device has radio scan power", power_max_queries)

        for (let i = 0; i < results.length; i++) {
            if (i == power_max_queries) break;
            var result_subset = results.slice(i);
            var serials: number[] = [];
            result_subset.forEach((r) => serials.push(r.serial_number))
            var powers = device.getRadioScanPower(new Uint32Array(serials))

            for (let sn = 0; sn < serials.length; sn++) {
                if (powers[sn] != 0x7f) {
                    scan_powers.set(serials[sn], powers[sn])
                }
            }
        }
    }
    console.log(scan_powers)

    var scans: {
        serial_number: number,
        bootloader: boolean,
        asphodel_type: number,
        device_mode: number,
        scan_strength: number,
    }[] = []
    results.forEach((r) => {
        var power = scan_powers.get(r.serial_number)
        scans.push({
            serial_number: r.serial_number,
            bootloader: (r.asphodel_type & ASPHODEL_PROTOCOL_TYPE_BOOTLOADER) != 0,
            asphodel_type: r.asphodel_type,
            device_mode: r.device_mode,
            scan_strength: power
        })
    })

    return scans
}

function doRadioScan(device: Device) {
    console.log("scanning fo remotes devices")
    device.startRadioScan()
    sleep(1000);
    //let serials = device.getRadioScanResults(255)
    //return serials.sort();
    var scans = collectScanResults(device)

    console.log(scans)

    var sensors: Device[] = []

    scans.forEach((scan) => {
        var remote = device.getRemoteDevice()
        remote.open();
        try {

            if (!scan.bootloader) {
                device.connectRadio(scan.serial_number)
            } else {
                device.connectRadioBoot(scan.serial_number)
            }

            remote.waitForConnect(1000)

            if (remote.getSerialNumber() == "") {
                console.log("No serial number when fetching device info");
                throw new Error("No serial number when fetching device info")
            }

            sensors.push(remote)

        } catch (e) {
            remote.close()
            remote.free();
            sensors.forEach((sen) => {
                sen.close();
                sen.free();
            })
            throw e
        }
    })
    return sensors
}

async function checkSensorsConnected(device: Device) {
    let remote_devices: Device[] = [];
    if (device.supportsRadioCommands()) {
        while (true) {
            try {
                remote_devices = doRadioScan(device)
                break
            } catch (e) {

            }
        }
    }

    return remote_devices
}


function sleep(millis: number) {
    var bgn = Date.now();
    while (Date.now() - bgn < millis) {

    }
}

function startStreams(device: Device, active_streams: number[]) {
    var stream_ids = active_streams.sort()
    stream_ids.forEach((id) => {
        device.warmUpStream(id, true)
    })
    var warm_up_time = 0;
    stream_ids.forEach((id) => {
        var stream = device.getStream(id).getInfo()
        if (stream.warm_up_delay > warm_up_time) {
            warm_up_time = stream.warm_up_delay;
        }
    })

    sleep(warm_up_time * 1000);

    stream_ids.forEach((id) => {
        device.enableStream(id, true);
        device.warmUpStream(id, false)
    })
}

function stopStreams(device: Device, active_streams: number[]) {
    active_streams.forEach((id) => {
        try {
            device.enableStream(id, false);
        } catch (e) { }
    })
}

async function aquireDataSaving(
    device: Device,
    time: number,
    schedule_id: string,
    job: Job,
    apd_path: string,
    channel: number
) {
    console.log("acquire for: ", time)
    var out = new DeviceData(channel);
    var device_info = createDeviceInfo(device, out);

    var streams_to_activate: number[] = []
    for (let i = 0; i < device_info.stream_count; i++) {
        streams_to_activate.push(i)
    }

    let response_time = 0.050; // 100 milliseconds
    let buffer_time = 0.500; // 500 milliseconds
    let timeout = 1000; // 1000 milliseconds
    let streaming_counts =
        getStreamingCounts(device_info.info_array, response_time, buffer_time, timeout)

    let apd = new ApdBuilder(
        device, streams_to_activate, streaming_counts, schedule_id,
        apd_path
    )

    device.startStreamingPackets(
        streaming_counts.packet_count,
        streaming_counts.transfer_count,
        streaming_counts.timeout, (status, stream_data, packet_size, packet_count) => {
            if (status == 0) {
                apd.update(stream_data)
                for (let packet = 0; packet < packet_count; packet++) {
                    device_info.decoder.decode(stream_data.slice(packet * packet_size))
                }
            } else if (status == -7) {
                console.log("==========TIMEOUT=======================")
            } else {
                console.log(`Bad status ${status} in streaming packet callback`);
            }
        });


    startStreams(device, streams_to_activate);


    await new Promise((resolve) => {
        let begin = Date.now();
        var progress = 0;
        function loop() {
            if (Date.now() - begin < time) {
                try {
                    device.poll(100)
                } catch (e) {
                    console.error(e)
                }
                setImmediate(loop)
                return;
            }
            device.stopStreamingPackets();
            device.poll(1000);
            stopStreams(device, streams_to_activate)
            resolve(true)
        }
        loop()
    })

    return { apd: apd, out: out }
}


function checkAllConnectedReceivers() {
    return USBFindDevices().concat(TCPFindDevices());
}


import * as fs from "fs"
import { DeviceConfig } from "./cycling";

function init() {
    USBInit()
    TCPInit();
}

function deinit() {
    USBDeInit()
    TCPDeinit()
}

function calculateMean(data: Float64Array) {
    return data.reduce((prev, curr) => prev + curr) / data.length;
}

function calculateVariance(data: Float64Array) {
    var mean = calculateMean(data)
    var sqdiff = data.map((value) => Math.pow(value - mean, 2))
    return calculateMean(sqdiff);
}

function calculateStandardDeviation(data: Float64Array) {
    return Math.sqrt(calculateVariance(data))
}

function calculateChannelMean(out: DeviceData) {
    var stream_means: any[] = []
    out.streams.forEach((stream) => {
        var means: any[] = [];
        stream.forEach((channel) => {
            //means.push({ counter: channel.counter, mean: calculateMean(channel.values) })
        })
        var channel_sum = 0;
        means.forEach((mean) => {
            channel_sum += mean.mean;
        })
        stream_means.push(channel_sum / means.length);
    })

    return stream_means
}

function calculateChannelStandardDeviation(out: DeviceData) {
    var stream_stdds: any[] = []
    out.streams.forEach((stream) => {
        var stdds: Float64Array = new Float64Array(stream.length);
        stream.forEach((channel, i) => {
            //var stdd = calculateStandardDeviation(channel.values);
            //stdds[i] = stdd;
        })
        var channel_sum = 0;
        stdds.forEach((stdd) => {
            channel_sum += stdd;
        })
        stream_stdds.push(channel_sum / stdds.length);
    })

    return stream_stdds
}

function closeSensors(sensors: Device[]) {
    sensors.forEach((sensor) => {
        console.log("closing sensor: ", sensor.getSerialNumber())
        sensor.close();
        sensor.free();
    })
}

async function doWorkOnDevice(job: any) {
    init()
    var config_device: DeviceConfig = job.data;

    var devices = checkAllConnectedReceivers();

    var actual_device = devices.find((dev) => {
        try {
            dev.open()
            return dev.getSerialNumber() == config_device.receiver
        } catch (e) {
            return false
        } finally {
            dev.close()
        }
    })
    if (!actual_device) throw new Error(`device ${config_device.receiver} not found.`);
    actual_device.open();
    var sensors: Device[] = [];

    try {
        sensors = await checkSensorsConnected(actual_device as Device);
    } catch (e) {
        actual_device.close();
        throw e;
    }

    sensors.forEach((sensor) => {
        console.log("found sensor: ", sensor.getSerialNumber())
    })
    var actual_sensor = sensors.find((sensor) => sensor.getSerialNumber() == config_device.sensor)

    if (actual_sensor == undefined) {
        closeSensors(sensors);
        actual_device.close()
        throw new Error(`sensor ${config_device.sensor} not connected to ${(actual_device as Device).getSerialNumber()}.`);
    }


    try {

        var results = await aquireDataSaving(
            actual_sensor as Device,
            config_device.duration, "demo", job, config_device.receiver + "-" + config_device.sensor, config_device.sensorChannel);


        for (var op of config_device.operations) {
            switch (op.operation.toLowerCase()) {
                case "save":
                    const params: {
                        "path": string, // The path to save the data
                        "type": "raw" | "processed" // The type of the data: raw is the .apd file with the raw data from sensor at full speed
                    } = op.params;

                    if (params.type == "raw") {
                        results.apd.final(op.path)
                    } else throw new Error(`processed output not implemented.`)
                    break
                case "stddev":
                    var stddev = calculateChannelStandardDeviation(results.out);
                    console.log("stddev: ", stddev)
                    break
                case "mean":
                    var mean = calculateChannelMean(results.out);
                    console.log("mean: ", mean)
                    break
                default: throw `unhandled operation: ${op.operation}.`
            }
        }

    } catch (e) {
        throw e
    } finally {
        closeSensors(sensors)
        actual_device.close()
    }
}


export default async (job) => {
    if (job) {
        console.log(`worker working on ${job.name}`)
        try {
            await doWorkOnDevice(job);
        } catch (e) {
            console.error(e)
            throw e;
        }
        console.log(`worker finished working on ${job.name}`)
    }
}
