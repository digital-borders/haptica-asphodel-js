const asp = require("./build/Release/haptica-asphodel-js.node")


type Device = {
    close: ()=>void,
    supportsRadioCommands: ()=>boolean,
    supportsRemoteCommands: ()=>boolean,
    supportsBootloaderCommands: ()=>boolean,
    supportsRFPowerCommands: ()=>boolean,
    getProtocalVersion: ()=>number,
    getProtocalVersionString: ()=>string,
    getBoardInfo: ()=>{info:string, rev:number},
    getBuildInfo: ()=>string,
    getBuildDate: ()=>string,

    getCommitID: ()=>string,
    getRepoBranch: ()=>string,
    getChipFamily: ()=>string,
    getChipModel: ()=>string,
    eraseNVM: ()=>void,
    getNVMSize: ()=>number,
    getChipID: ()=>string,


    writeNVMRaw: (start_address:number, data: Uint8Array)=>void,
    writeNVMSection: (start_address:number, data: Uint8Array)=>void,
    readNVMRaw: (start_address:number, length: number)=>Uint8Array,
    readNVMSection:(start_address:number, length: number)=>Uint8Array,
    readUserTagString: (offset:number) =>string,
    getNVMModified: ()=>number,
    getNVMHash: ()=>string,
    getSettingHash: ()=>string,
    reset: ()=>void
   
    getBootloaderInfo: ()=>string,
    bootloaderJump:()=>void,
    getResetFlag: ()=>number,
    clearResetFlag: ()=>void,
    getRGBCount: ()=>number,
    getRGBValues: (index: number)=>Uint8Array
    setRGBValues: (index: number, values: Uint8Array)=>void,
    setRGBValuesHex: (index: number, values: number)=>void,
    getLEDCount: ()=>number,
    getLEDValue: (index: number)=>number,
    setLEDValue: (index: number, value: number) => void,
    getDeviceMode: ()=>number,
    setDeviceMode: (mode:number)=>void,
    open: ()=>void,
    getProtocalType: ()=>number,
    getLocationString: ()=>string,
    getSerialNumber: ()=>string,

    doTranfer: (command: number, params: Uint8Array, call_back: (
        status:number, params: Uint8Array
    )=>void)=>void,
    doTranferReset: (command: number, params: Uint8Array, call_back: (
        status:number, params: Uint8Array
    )=>void)=>void,
    startStreamingPackets:(packet_count:number, transfer_count:number, timeout:number, callback: (
        status:number, stream_data: Uint8Array, packet_size: number, packet_count: number
    )=>void)=>void,
    stopStreamingPackets: ()=>void,
    getStreamPackets: (count: number, timeout:number)=>Uint8Array,
    getStreamPacketLength: ()=>number,
    getMaxIncomingParamLength: ()=>number,
    getMaxOutgoingParamLength: ()=>number,
    poll: (millis: number)=>number,
    setConnectCallback: (callback: (status:number, connected:number)=>void)=>void,
    waitForConnect: (millis: number)=> void,
    getRemoteDevice: ()=>Device,
    reconnectDevice: ()=>Device,
    reconnectDeviceBootloader: ()=>Device,
    reconnectDeviceApplication: ()=>Device,
    setErrorCallback: (callback: (status:number)=>void)=>void,
    getTransportType: ()=>string,
    tcpGetAdvertisement: ()=> {
        tcp_version: number,
        connected: number,
        max_incoming_param_length: number,
        max_outgoing_param_length: number,
        stream_packet_length: number,
        protocol_type: number,
        serial_number: string,
        board_rev: number,
        board_type: string,
        build_info: string,
        build_date: string,
        user_tag1: string,
        user_tag2: string,
        remote_max_incoming_param_length: number,
        remote_max_outgoing_param_length: number,
        remote_stream_packet_length: number
    }
}

export const getErrorName: (err:number) =>string = asp.getErrorName
export const getUnitTypeName: (t: number)=>string = asp.getUnitTypeName
export const getUnitTypeCount: ()=>number= asp.getUnitTypeCount
export const getSettingTypeName: (s:number)=>string = asp.getSettingTypeName
export const getSettingTypeCount: ()=>number = asp.getSettingTypeCount
export const getChannelTypeName: (c:number)=>string = asp.getChannelTypeName
export const getChannelTypeCount: ()=>number = asp.getChannelTypeCount

export const USBInit: ()=>void = asp.USBInit
export const USBDeInit: ()=>void = asp.USBDeInit
export const USBPollDevices: (millis: number)=>void = asp.USBPollDevices
export const USBFindDevices: ()=>Device[] = asp.USBFindDevices
export const USBGetBackendVersion: ()=>string = asp.USBGetBackendVersion

enum TCPFilter{
    ASPHODEL_TCP_FILTER_DEFAULT = 0x0, // default parameters used by asphodel_tcp_find_devices()
    ASPHODEL_TCP_FILTER_PREFER_IPV6 = 0x0, // when a device SN is discovered on multiple protocols return only IPv6
    ASPHODEL_TCP_FILTER_PREFER_IPV4 = 0x1, // when a device SN is discovered on multiple protocols return only IPv4
    ASPHODEL_TCP_FILTER_ONLY_IPV6 = 0x2, // only search for devices using IPv6
    ASPHODEL_TCP_FILTER_ONLY_IPV4 = 0x3, // only search for devices using IPv4
    ASPHODEL_TCP_FILTER_RETURN_ALL = 0x4 // return each protocol instance of all devices found
}


export const TCPInit: ()=>void = asp.TCPInit
export const TCPDevicesSupported: ()=>boolean = asp.TCPDevicesSupported
export const TCPDeinit: ()=>void = asp.TCPDeinit
export const TCPFindDevices: ()=>Device[] = asp.TCPFindDevices
export const TCPFindDevicesFilter: (filter:TCPFilter)=>Device[] = asp.TCPFindDevicesFilter
export const TCPPollDevices: (millis:number)=>void = asp.TCPPollDevices
export const TCPCreateDevice: (host:string, port:number, timeout:number, serial: string)=>Device = asp.TCPCreateDevice


