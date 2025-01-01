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
}


export const USBInit: ()=>void = asp.USBInit
export const USBDeInit: ()=>void = asp.USBDeInit
export const USBPollDevices: (millis: number)=>void = asp.USBPollDevices
export const USBFindDevices: ()=>Device[] = asp.USBFindDevices
export const USBGetBackenVersion: ()=>void = asp.USBGetBackenVersion
