import { version } from "os"
import { CHANNEL_TYPE_COMPOSITE_STRAIN, CHANNEL_TYPE_FAST_STRAIN, CHANNEL_TYPE_LINEAR_ACCEL, CHANNEL_TYPE_PACKED_ACCEL, CHANNEL_TYPE_SLOW_ACCEL, CHANNEL_TYPE_SLOW_STRAIN, ChannelInfo, createDeviceDecoder, Device, getChannelTypeName, getLibraryBuildDate, getLibraryBuildInfo, getLibraryProtocalVersion, getLibraryProtocalVersionString, getUnitTypeName, StreamAndChannels, TCPDeinit, TCPFindDevices, TCPInit, USBDeInit, USBFindDevices, USBGetBackendVersion, USBInit } from "../haptica-asphodel-js"



function printBootloaderInfo(dev: Device) {
    if (dev.supportsBootloaderCommands()) {
        console.log("Bootloader commands supported")

        let info = dev.getBootloaderPageInfo()
        console.log("Bootloader info: ", info.page_info.slice(0, info.length))

        let b = dev.getBootloaderBlockSizes(255)
        console.log("Bootloader blocksizes: ", b)
    }
}

function printRemoteInfo(dev: Device) {
    if (dev.supportsRemoteCommands()) {
        console.log("Remote commands supported")
        let st = dev.getRemoteStatus()
        console.log("Remotes status: ", st)
    }
}

function printRadioInfo(dev: Device) {
    if (dev.supportsRadioCommands()) {
        console.log("Radio commands supported")
        let st = dev.getRadioStatus()
        console.log("Radio status: ", st)

        let vs = dev.getRadioCtrlVars(255)
        console.log("radio control variables: ", vs.result.slice(0, vs.length))

        let ser = dev.getRadioDefaultSerial()
        console.log("default_serial: ", ser)
    }
}

function printRfPowerInfo(dev: Device) {
    if (dev.supportsRFPowerCommands()) {
        console.log("Rf power commands supported")
        let status = dev.getRfPowerStatus()
        console.log("Power enabled: ", status)

        let vs = dev.getRfPowerCtlVars(255)
        console.log("Rf power control variables: ", vs.result.slice(0, vs.length))
    }
}

function printLowLevelInfo(dev: Device) {
    let count1 = dev.getGPIOPortCount()
    console.log("GPIO port count: ", count1)

    for (let i = 0; i < count1; i++) {
        console.log("GPIO port: ", i)
        let name = dev.getGPIOPortName(i)
        console.log("name: ", name)

        let info = dev.getGPIOPortInfo(i)
        console.log("    input_pins=", info.input_pins);
        console.log("    output_pins=", info.output_pins);
        console.log("    floating_pins=", info.floating_pins);
        console.log("    loaded_pins=", info.loaded_pins);
        console.log("    overridden_pins=", info.overridden_pins);


        let values = dev.getGPIOPortValues(i)
        console.log("pin_values: ", values)
    }

    let count = dev.getInfoRegionCount()
    console.log("info region counts: ", count)

    for (let i = 0; i < count; i++) {
        console.log("info region: ", i)

        let name = dev.getInfoRegionName(i)
        console.log("name: ", name)

        let info = dev.getInfoRegion(i, 255)
        console.log("values: ", info.result.slice(0, info.length))
    }

    let info = dev.getStackInfo()
    console.log("Stack info: ", info)
}

function printSettingCategoryInfo(dev: Device) {
    let count = dev.getSettingCategoryCount()
    console.log("Setting category count: ", count);

    for (let i = 0; i < count; i++) {
        console.log("setting category: ", i)
        let name = dev.getSettingCategoryName(i)
        console.log("Name: ", name)

        let res = dev.getSettingCategorySettings(i, 255)
        console.log("settings: ", res.result.slice(0, res.length))
    }
}

function printCustomEnumInfo(dev: Device) {
    let counts = dev.getCustomEnumCounts(255)
    console.log("Custom enum count: ", counts.length)

    for (let i = 0; i < counts.length; i++) {
        console.log("Custom enum: ", i)

        for (let j = 0; j < counts.result[i]; j++) {
            let name = dev.getCustomEnumValueName(i, j)
            console.log("Name: ", name)
        }
    }
}

function printSettingInfo(dev: Device) {
    let count = dev.getSettingCount()
    console.log("Settings Count: ", count)


    for (let i = 0; i < count; i++) {
        let name = dev.getSettingName(i)
        console.log("name: ", name)

        let info = dev.getSettingInfo(i)
        switch (info.getType()) {
            default:
        }

        let def = dev.getSettingDefault(i, 255)
        console.log("Default bytes: ", def)
    }
}

function printCtrlVarInf0(dev: Device) {
    let count = dev.getCtrlVarCount()
    console.log("Control variable count: ", count)
    for (let i = 0; i < count; i++) {
        console.log("constrol variable: ", i)
        let name = dev.getCtrlVarName(i)
        console.log("Name: ", name)

        let info = dev.getCtrlVarInfo(i)
        console.log(`unit_type=${info.unit_type}`)
        console.log(`minimum=${info.minimum}, maximum=${info.maximum}`)
        console.log(`scale=${info.scale} offset=${info.offset}`)

    }
}

function printSupplyInfo(dev: Device) {
    let count = dev.getSupplyCount()
    console.log("Supply count: ", count)

    for (let i = 0; i < count; i++) {
        let name = dev.getSupplyName(i)
        console.log("name: ", name)

        let info = dev.getSupplyInfo(i)
        console.log(`unit type=${info.unit_type} (${getUnitTypeName(info.unit_type)})`)
        console.log(`is_battery: ${info.is_battery}, nominal: ${info.nominal}`)
        console.log(`scale: ${info.scale}, offset: ${info.offset}`)

        let val = dev.checkSupply(i, 20)
        console.log(val)
    }
}

function printDecoderInfo(device: Device) {
    let count = device.getStreamCount()
    let streamchannel: StreamAndChannels[] = []

    for (let i = 0; i < count.count; i++) {
        let stream = device.getStream(i)
        let channels: ChannelInfo[] = []
        let stream_info = stream.getInfo();
        if (stream_info.channel_count == 0) {
            throw new Error(`Error: stream ${i} has 0 channels!`)
        } else {
            let channel_ndexes = stream_info.channel_index_list;
            for (let j = 0; j < stream_info.channel_count; j++) {
                channels.push(device.getChannel(channel_ndexes[j]))
            }
        }
        streamchannel.push(new StreamAndChannels(i, stream, channels))
    }

    let device_decoder = createDeviceDecoder(streamchannel, count.filler_bits, count.id_bits)
    console.log(`Device decoder: streams=${device_decoder.getStreams()}, id_byte_offset=${device_decoder.getIDByteOffset()}`)
    let decoders = device_decoder.getDecoders();
    let ids = device_decoder.getStreamIDs();
    decoders.forEach((d, i) => {
        console.log("Stream decoder: ", i);
        console.log(`id=${ids[i]}, counter_byte_offset=${d.getCounterByteOffset()}, channels=${d.getChannels()}`)

        d.getDecoders().forEach((cd, j) => {
            console.log("Channel decoder: ", j);
            console.log(`name: ${cd.getChannelName()}`);
            console.log(`channel_bit_offset=${cd.getChannelBitOffset()}, samples=${cd.getSamples()}, subchannels=${cd.getSubChannels()}`)
        })
    })
}

function printChannelSpecificinfo(dev: Device) {
    let cnt = dev.getChannelCount()
    console.log(`Channel specifics for ${cnt} channels`)

    for (let i = 0; i < cnt; i++) {
        let channel = dev.getChannel(i)
        let channel_info = channel.getInfo();
        if (
            channel_info.channel_type == CHANNEL_TYPE_SLOW_STRAIN ||
            channel_info.channel_type == CHANNEL_TYPE_FAST_STRAIN ||
            channel_info.channel_type == CHANNEL_TYPE_COMPOSITE_STRAIN
        ) {
            let bridge_count = channel.getStrainBridgeCount();

            for (let bridge_index = 0; bridge_index < bridge_count; bridge_index++) {
                let subchannel_index = channel.getStrainBridgeSubchannel(bridge_index) as number
                let values = channel.getStrainBridgeValues(bridge_index)

                console.log(`Bridge: ${bridge_index} (subchannel_index=${subchannel_index})`)
                console.log(`positive sense = ${values[0]}`)
                console.log(`negative sense = ${values[1]}`)
                console.log(`bridge element nominal = ${values[2]}`)
                console.log(`bridge element minimum = ${values[3]}`)
                console.log(`bridge element maximum = ${values[4]}`)
            }
        } else if (
            channel_info.channel_type == CHANNEL_TYPE_SLOW_ACCEL ||
            channel_info.channel_type == CHANNEL_TYPE_PACKED_ACCEL ||
            channel_info.channel_type == CHANNEL_TYPE_LINEAR_ACCEL
        ) {
            let limits = channel.getAccelSelfTestLimits()
            console.log(`   X axis self test difference: min=${limits[0]}, max=${limits[1]}`)
            console.log(`   Y axis self test difference: min=${limits[2]}, max=${limits[3]}`)
            console.log(`   Z axis self test difference: min=${limits[4]}, max=${limits[5]}`)

        } else {
            console.log(`Channel ${i} No specifics`)
        }
    }
}

function printChannelInfo(dev: Device) {

    let count = dev.getChannelCount()
    console.log("Channel count: ", count);

    for (let i = 0; i < count; i++) {
        let name = dev.getChannelName(i)
        console.log("Name: ", name)

        let channel = dev.getChannelInfo(i)
        let info = channel.getInfo()

        console.log(`channel type: ${info.channel_type})} (${getChannelTypeName(info.channel_type)})`)
        console.log(`unit_type: ${info.unit_type} (${getUnitTypeName(info.unit_type)})`)
        console.log(`filler_bits: ${info.filler_bits}, data_bits: ${info.data_bits}`)
        console.log(`samples: ${info.samples}, bits_per_sample: ${info.bits_per_sample}`)
        console.log(`minimum: ${info.minimum}, maximum: ${info.maximum}, resolution: ${info.resolution}`)
        console.log(`chunk_count: ${info.chunks.length}`)
        for (let j = 0; j < info.chunk_count; j++) {
            let chunk = dev.getChannelChunk(i, j, 255)
            console.log(chunk.result.slice(0, chunk.length))
        }

        let coefs = dev.getChannelCoefficients(i, 255)
        console.log("Coefficients: ", coefs.result.slice(0, coefs.length))

        let cb = dev.getChannelCalibration(i)
        if (cb.available == 0) {
            console.log("No calibration info")
        } else {
            console.log(`calibration base setting=${cb.calibration.base_setting_index}, calibration resolution setting=${cb.calibration.resolution_setting_index}`)
            console.log(`calibration scale=${cb.calibration.scale}, calibration offset=${cb.calibration.offset}`)
            console.log(`calibration minimum=${cb.calibration.minimum}, calibration max=${cb.calibration.maximum}`)
        }
    }
}

function printStreamInfo(dev: Device) {
    let count = dev.getStreamCount()
    console.log("Stream count: ", count);

    for (let i = 0; i < count.count; i++) {
        console.log("Stream: ", i);
        let stream_channels = dev.getStreamChannels(i, 255)
        console.log("channels: ", stream_channels.result.slice(0, stream_channels.length))

        const format = dev.getStreamFormat(i)
        let fmt = format.getInfo();
        console.log(`filler_bits=${fmt.filler_bits}, counter_bits=${fmt.counter_bits}`)
        console.log(`rate=${fmt.rate}, rate_error=${fmt.rate_error}`)
        console.log(`warm_up_delay=${fmt.warm_up_delay}`)

        const info = dev.getStreamRateInfo(i)
        console.log("Stream rate info: ", info)

        const status = dev.getStreamStatus(i)
        console.log("Stream status: ", status)
    }

}

async function printLedInfo(dev: Device) {
    let count = dev.getRGBCount()
    console.log("RGB Count: ", count)
    for (let i = 0; i < count; i++) {
        console.log("   RGB: ",
            dev.getRGBValues(i))
    }

    let led_count = dev.getLEDCount()
    console.log("LED count: ", led_count)
    for (let i = 0; i < led_count; i++) {
        console.log(`       LED ${i} value: ${dev.getLEDValue(i)}`)
    }
}

async function printNvmInfo(dev: Device) {
    const nvm_size = dev.getNVMSize()
    console.log("       NVM size: ", nvm_size);
    const locations = dev.getUserTagLocations();
    console.log("       User Tag Locations: ", locations);
    const user_tag_1 = dev.readUserTagString(locations[0], locations[1])
    console.log("       User Tag 1: ", user_tag_1)
    const user_tag_2 = dev.readUserTagString(locations[2], locations[3])
    console.log("           User Tag 1: ", user_tag_2)
    const nvm = dev.readNVMSection(locations[4], locations[5])
    console.log("           General Bytes: ", nvm)
    const nvm2 = dev.readNVMSection(0, nvm_size as number)
    console.log("           NVM Data: ", nvm2)

}

function printDeviceInfo(device: Device) {
    console.log("   Location String: ", device.getLocationString());
    console.log("   Serial Number: ", device.getSerialNumber());
    console.log("   Max Incoming Param Len: ", device.getMaxIncomingParamLength())
    console.log("   Max Outgoing Param Len: ", device.getMaxOutgoingParamLength())
    console.log("   Stream Packet Len: ", device.getStreamPacketLength())
    console.log("   Protocal Version: ", device.getProtocalVersionString())
    console.log("   Board info: ", device.getBoardInfo())
    console.log("   Build info: ", device.getBuildInfo())
    console.log("   Build date: ", device.getBuildDate())
    console.log("   Chip Family: ", device.getChipFamily())
    console.log("   Chip Model: ", device.getChipModel())
    console.log("   Chip ID: ", device.getChipID())
    console.log("   Bootloader Info: ", device.getBootloaderInfo())
    printNvmInfo(device);
    printLedInfo(device)
    printStreamInfo(device)
    printChannelInfo(device)
    printChannelSpecificinfo(device)
    printDecoderInfo(device)
    printSupplyInfo(device)
    printCtrlVarInf0(device)
    printSettingInfo(device)
    printCustomEnumInfo(device)
    printSettingCategoryInfo(device)
    printSettingInfo(device)
    printLowLevelInfo(device)
    printRfPowerInfo(device)
    printRadioInfo(device)
    printRemoteInfo(device)
    printBootloaderInfo(device)
}


function printRemoteDeviceInfo(dev: Device, serial: number) {
    let remote = dev.getRemoteDevice()
    remote.open()
    dev.connectRadio(serial)
    remote.waitForConnect(1000);
    printDeviceInfo(remote)
    dev.stopRadio()
    remote.close()
}


async function main() {
    console.log("Library protocol Version: ", getLibraryProtocalVersionString())
    console.log("Library Build Info: ", getLibraryBuildInfo());
    console.log("Library Build Date: ", getLibraryBuildDate());
    console.log("Library USB Backend Version: ", USBGetBackendVersion())

    const usb_devices = USBFindDevices(10)
    const tcp_devices = TCPFindDevices(10)

    const devices = usb_devices.concat(tcp_devices);

    console.log(`Found ${devices.length} devices!`)

    devices.forEach((device) => {
        device.open()
        printDeviceInfo(device)

        if(device.supportsRadioCommands()) {
            console.log("scanning fo remotes devices")
            device.startRadioScan()
            setTimeout(()=>{
                device.stopRadio()
                let serials = device.getRadioScanResults(255)

                    let sorted = serials.sort();
                    console.log("Radio Scan results: ", sorted)
                    sorted.forEach((serial)=>{
                        printRemoteDeviceInfo(device, serial)
                    })
            }, 200)
        }
    })

}

TCPInit();
USBInit();
main()
TCPDeinit();
USBDeInit()