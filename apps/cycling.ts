import { channel } from "diagnostics_channel";
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
    streams: { counter: number, values: Float64Array }[][]

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
        dec.setLostPacketCallback((current, last) => {
            console.log(`${current} ${last}=================== Lost ${current - last - 1} packets from ${serial_number} stream ${i} ========================`)
        })

        //var channel_decoders = dec.getDecoders();
        //let  ch = 0;
        //for(let channel_decoder of channel_decoders) {
        //    channel_decoder.setDecodeCallback((counter, data, samples, subchannels)=>{
        //        console.log(`[${ch}] counter: ${counter}, samples: ${samples}, subchannels: ${subchannels}`)
        //    })
        //    ch++
        //}

        dec.getDecoders().forEach((channel_decoder, j) => {
            if (out == null) return;
            channel_decoder.setDecodeCallback((counter, data, samples, subchannels) => {
                if (j == out.channel) {
                    //let arr = []
                    //make2DArray(data, samples, subchannels, arr);
                    //out.streams[i].push({
                    //    counter: counter,
                    //    values: data
                    //});
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
    job: Job
) {
    console.log("acquire for: ", time)
    var device_info = createDeviceInfo(device, null);

    var streams_to_activate: number[] = []
    for (let i = 0; i < device_info.stream_count; i++) {
        streams_to_activate.push(i)
    }

    let response_time = 0.050; // 100 milliseconds
    let buffer_time = 0.500; // 500 milliseconds
    let timeout = 1000; // 1000 milliseconds
    let streaming_counts =
        getStreamingCounts(device_info.info_array, response_time, buffer_time, timeout)

    let apd = new ApdBuilder(device, streams_to_activate, streaming_counts, schedule_id)
    device.startStreamingPackets(
        streaming_counts.packet_count,
        streaming_counts.transfer_count,
        streaming_counts.timeout, (status, stream_data, packet_size, packet_count) => {
            if (status == 0) {
                apd.update(stream_data)
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
            stopStreams(device, streams_to_activate)
            device.stopStreamingPackets();
            device.poll(10);
            resolve(true)
        }
        loop()
    })

    return apd
}


async function aquireData(device: Device, time: number, channel: number) {
    var out = new DeviceData(channel);
    var device_info = createDeviceInfo(device, out);

    var streams_to_activate: number[] = []
    for (let i = 0; i < device_info.stream_count; i++) {
        streams_to_activate.push(i)
    }
    
    let response_time = 0.0500; // 100 milliseconds
    let buffer_time = 0.500; // 500 milliseconds
    let timeout = 1000; // 1000 milliseconds
    let streaming_counts = getStreamingCounts(device_info.info_array, response_time, buffer_time, timeout)
    
    device.startStreamingPackets(streaming_counts.packet_count,
        streaming_counts.transfer_count,
        streaming_counts.timeout, (status, stream_data, packet_size, packet_count) => {
            if (status == 0) {
                for (let packet = 0; packet < packet_count; packet++) {
                    device_info.decoder.decode(stream_data.slice(packet * packet_size))
                }
            } else if (status == -7) {
                console.log("==========TIMEOUT=======================")
            } else {
                console.log(`Bad status ${status} in streaming packet callback`);
            }
        });

    startStreams(device, streams_to_activate)
    await new Promise((resolve) => {
        let begin = Date.now();
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
            stopStreams(device, streams_to_activate)
            device.stopStreamingPackets();
            device.poll(10);
            resolve(true)
        }
        loop()
    })

    return out
}


function checkAllConnectedReceivers() {
    return USBFindDevices().concat(TCPFindDevices());
}


const path_to_config = "apps/config.json";

type DeviceConfig = {
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

type Config = {
    mqtt: {
        host: string, // The host of the mqtt broker
        port: number, // The port of the mqtt broker
        username: string, // The username of the mqtt broker
        password: string, // The password of the mqtt broker
        baseTopic: string // The base topic to publish the data to
    },
    devices: DeviceConfig[]
}

import * as fs from "fs"

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
            means.push({ counter: channel.counter, mean: calculateMean(channel.values) })
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
            var stdd = calculateStandardDeviation(channel.values);
            stdds[i] = stdd;
        })
        var channel_sum = 0;
        stdds.forEach((stdd) => {
            channel_sum += stdd;
        })
        stream_stdds.push(channel_sum / stdds.length);
    })

    return stream_stdds
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
    var sensors = await checkSensorsConnected(actual_device as Device);

    sensors.forEach((sensor) => {
        console.log("found sensor: ", sensor.getSerialNumber())
    })

    console.log("========================1")

    var actual_sensor = sensors.find((sensor) => sensor.getSerialNumber() == config_device.sensor)

    console.log("========================2")


    if (actual_sensor == undefined) throw new Error(`sensor ${config_device.sensor} not connected to ${(actual_device as Device).getSerialNumber()}.`);
    console.log("========================3")

    for (var op of config_device.operations) {
        switch (op.operation.toLowerCase()) {
            case "save":
                const params: {
                    "path": string, // The path to save the data
                    "type": "raw" | "processed" // The type of the data: raw is the .apd file with the raw data from sensor at full speed
                } = op.params;

                if (params.type == "raw") {
                    var apd = await aquireDataSaving(
                        actual_sensor as Device,
                        config_device.duration, "demo", job);
                    apd.finalFile(params.path);
                } else throw `processed output not implemented.`
                break
            case "stddev":
                var out = await aquireData(actual_sensor as Device, config_device.duration, config_device.sensorChannel);
                var stddev = calculateChannelStandardDeviation(out);
                console.log("stddev: ", stddev)
                break
            case "mean":
                console.log("========================4")

                var out = await aquireData(actual_sensor as Device, config_device.duration, config_device.sensorChannel);
                var mean = calculateChannelMean(out);
                console.log("mean: ", mean)
                break
            default: throw `unhandled operation: ${op.operation}.`
        }
    }

    sensors.forEach((sensor) => {
        console.log("closing sensor: ", sensor.getSerialNumber())
        //sensor.close();
        //sensor.free();
    })

    //actual_device.close()
}

import { Job, Queue, Worker } from 'bullmq';
import IORedis from 'ioredis';
import { setInterval } from "timers/promises";

function log(message: any) {
    fs.appendFileSync("log.txt", `${message}`);
}

async function main() {
    const connection = new IORedis({ maxRetriesPerRequest: null });
    const work_q = new Queue('aquire-data');


    var config_str: string = fs.readFileSync(path_to_config).toString();
    var config: Config = JSON.parse(config_str);


    for (let config_device of config.devices) {
        await work_q.add(config_device.receiver + "-" + config_device.sensor, config_device, {
            repeat: {
                pattern: config_device.cron_start
            }
        })
//
        //await doWorkOnDevice({data: config_device})

        //})
        //.catch((e) => {
        //        console.log(`Error ${e.toString()}: waiting for ${config_device.failure_delay} before retrying...`)
        //        var bgin = Date.now();
        //        while(Date.now() - bgin < config_device.failure_delay){};
        //})
        //})

    }

    var worker = new Worker("aquire-data", async (job) => {
        if (job) {
            console.log(`worker working on ${job.name}`)
            await doWorkOnDevice(job);
            console.log(`worker finished working on ${job.name}`)
        }
    }, {
        connection
    })

    worker.on("stalled", (jobId) => {
        console.log(`[[[ job [id ${jobId}] failed ]]]`)

    })

    worker.on("completed", (job) => {
        console.log(`[[[ job ${job.name} completed ]]]`)
    })

    worker.on("failed", (job) => {
        if (job) {
            console.log(`[[[ job ${job.name} failed return ${job.returnvalue}]]]`)
        }
    })

    if (worker.isRunning() == false) {
        worker.run().catch((e) => {
            console.log("failed to start worker", e)
        })
    }


    //setInterval(1000, async ()=>{
    //    console.log("---tick---")
    //    log("=========work status=====")
    //    log("   =========completed========")
    //    var completed = await work_q.getCompleted();
    //    if(completed.length > 0) {
    //        completed.forEach(element => {
    //            log(element.name)
    //        });
    //    } else {
    //        log("None");
    //    }
    //    log("   =========completed========")
    //    log("   =========failed========")
    //    var failed = await work_q.getFailed();
    //    if(failed.length > 0) {
    //        failed.forEach(element => {
    //            log(element.name)
    //        });
    //    } else {
    //        log("None");
    //    }
    //    log("   =========completed========")
    //    log("   =========waiting========")
    //    var waiting = await work_q.getWaiting();
    //    if(waiting.length > 0) {
    //        waiting.forEach(element => {
    //            log(element.name)
    //        });
    //    } else {
    //        log("None");
    //    }
    //    log("   =========waiting========")
    //    log("=========work status=====")
    //})
}


function main2() {
    //var devs = USBFindDevices();
    //var device = devs[0];
    //device.open();
    //var apd = aquireDataSaving(device, 60000, "sample");
    //apd.finalFile("../out.raw")
    //device.close();
}



function main3() {
    //var devs = USBFindDevices();
    //var device = devs[0];
    //device.open();
    //var out = aquireData(device, 1000, 0);
    //console.log(calculateChannelMean(out))
    //console.log(calculateChannelStandardDeviation(out))
    //device.close();
}


//init()
//main3();
//deinit()
//main().then(() => {
//        console.log("main returnrd")
//    deinit()
//})


main().then(() => {
    console.log("main exited...")
})
