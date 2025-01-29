import { Device, getStreamingCounts, StreamAndChannels, TCPDeinit, TCPFindDevices, TCPInit, USBDeInit, USBFindDevices, USBInit } from "../haptica-asphodel-js";

function init() {
    USBInit()
    TCPInit();
}

function deinit() {
    USBDeInit()
    TCPDeinit()
}


function checkAllConnectedReceivers() {
    let usb_devices = USBFindDevices(50);
    let tcp_devices = TCPFindDevices(50);
    return {
        tcp_devices: tcp_devices,
        usb_devices: usb_devices
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


function aquireData(device: Device) {
    let response_time = 0.100; // 100 milliseconds
    let buffer_time = 0.500; // 500 milliseconds
    let timeout = 1000; // 1000 milliseconds

    let stream_and_channel = new StreamAndChannels()

    let streaming_counts = getStreamingCounts(device_info_array[i].info_array, response_time, buffer_time, timeout)

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


function main() {
    init()
    const devices = checkAllConnectedReceivers()
    deinit()
}




