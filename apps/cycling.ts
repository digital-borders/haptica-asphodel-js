import { channel } from "diagnostics_channel";

import { Job, Queue, Worker } from 'bullmq';
import IORedis from 'ioredis';
import { setInterval } from "timers/promises";
import * as path from "path";
import { fileURLToPath } from "url";

import * as fs from "fs"

import * as proc from "./proc"

export type DeviceConfig = {
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

export type Config = {
    mqtt: {
        host: string, // The host of the mqtt broker
        port: number, // The port of the mqtt broker
        username: string, // The username of the mqtt broker
        password: string, // The password of the mqtt broker
        baseTopic: string // The base topic to publish the data to
    },
    devices: DeviceConfig[]
}

const path_to_config = "apps/config.json";

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
    }
    var processor_file = "/home/melo/Desktop/haptica-asphodel-js/apps/proc.js"
    var worker = new Worker("aquire-data", processor_file, {
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
