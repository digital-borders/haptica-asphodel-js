const asp = require("./build/Release/haptica-asphodel-js.node")
const lzma = require("lzma-native")
import * as fs from "fs";
import { WriteStream } from "fs";


// Asphodel protocol version 2.3.3
// NOTE: use the functions in asphodel_version.h to access the protocol version
export const ASPHODEL_PROTOCOL_VERSION_MAJOR = 0x02
export const ASPHODEL_PROTOCOL_VERSION_MINOR = 0x03
export const ASPHODEL_PROTOCOL_VERSION_SUBMINOR = 0x03

// USB class/subclass defines
// use one ASPHODEL_PROTOCOL_TYPE_* as the USB protocol definition
export const ASPHODEL_USB_CLASS = 0xFF // 0xFF: vendor specific USB class
export const ASPHODEL_USB_SUBCLASS = 0x01 // 0x01: Generic Sensor

// protocol types to define various implementations
export const ASPHODEL_PROTOCOL_TYPE_BASIC = 0x00 // 0x00: basic (minimum) implementation
export const ASPHODEL_PROTOCOL_TYPE_RF_POWER = 0x01 // 0x01: RF Power protocol extension
export const ASPHODEL_PROTOCOL_TYPE_RADIO = 0x02 // 0x02: radio interface controlling a remote interface
export const ASPHODEL_PROTOCOL_TYPE_REMOTE = 0x04 // 0x04: remote interface (controlled by a radio interface)
export const ASPHODEL_PROTOCOL_TYPE_BOOTLOADER = 0x08 // 0x08: firmware bootloader

// general information commands
export const CMD_GET_PROTOCOL_VERSION = 0x00
export const CMD_GET_BOARD_INFO = 0x01
export const CMD_GET_USER_TAG_LOCATIONS = 0x02
export const CMD_GET_BUILD_INFO = 0x03
export const CMD_GET_BUILD_DATE = 0x04
export const CMD_GET_CHIP_FAMILY = 0x05
export const CMD_GET_CHIP_MODEL = 0x06
export const CMD_GET_CHIP_ID = 0x07

// NVM commands
export const CMD_GET_NVM_SIZE = 0x08
export const CMD_ERASE_NVM = 0x09
export const CMD_WRITE_NVM = 0x0A
export const CMD_READ_NVM = 0x0B

// Flush/Reinit communication pipes
export const CMD_FLUSH = 0x0C

// reset commands
export const CMD_RESET = 0x0D
export const CMD_GET_BOOTLOADER_INFO = 0x0E
export const CMD_BOOTLOADER_JUMP = 0x0F

// LED commands
export const CMD_GET_RGB_COUNT = 0x10
export const CMD_GET_RGB_VALUES = 0x11
export const CMD_SET_RGB = 0x12
export const CMD_SET_RGB_INSTANT = 0x13
export const CMD_GET_LED_COUNT = 0x14
export const CMD_GET_LED_VALUE = 0x15
export const CMD_SET_LED = 0x16
export const CMD_SET_LED_INSTANT = 0x17

// state commands
export const CMD_GET_RESET_FLAG = 0x18
export const CMD_CLEAR_RESET_FLAG = 0x19
export const CMD_GET_NVM_MODIFIED = 0x1a
export const CMD_GET_NVM_HASH = 0x1b
export const CMD_GET_SETTING_HASH = 0x1c

// extra build info
export const CMD_GET_COMMIT_ID = 0x1d
export const CMD_GET_REPO_BRANCH = 0x1e
export const CMD_GET_REPO_NAME = 0x1f

// stream commands
export const CMD_GET_STREAM_COUNT_AND_ID = 0x20
export const CMD_GET_STREAM_CHANNELS = 0x21
export const CMD_GET_STREAM_FORMAT = 0x22
export const CMD_ENABLE_STREAM = 0x23
export const CMD_WARM_UP_STREAM = 0x24
export const CMD_GET_STREAM_STATUS = 0x25
export const CMD_GET_STREAM_RATE_INFO = 0x26

// channel commands
export const CMD_GET_CHANNEL_COUNT = 0x30
export const CMD_GET_CHANNEL_NAME = 0x31
export const CMD_GET_CHANNEL_INFO = 0x32
export const CMD_GET_CHANNEL_COEFFICIENTS = 0x33
export const CMD_GET_CHANNEL_CHUNK = 0x34
export const CMD_CHANNEL_SPECIFIC = 0x35
export const CMD_GET_CHANNEL_CALIBRATION = 0x36

// power supply check commands
export const CMD_GET_SUPPLY_COUNT = 0x40
export const CMD_GET_SUPPLY_NAME = 0x41
export const CMD_GET_SUPPLY_INFO = 0x42
export const CMD_CHECK_SUPPLY = 0x43

// control variable commands
export const CMD_GET_CTRL_VAR_COUNT = 0x50
export const CMD_GET_CTRL_VAR_NAME = 0x51
export const CMD_GET_CTRL_VAR_INFO = 0x52
export const CMD_GET_CTRL_VAR = 0x53
export const CMD_SET_CTRL_VAR = 0x54

// settings commands
export const CMD_GET_SETTING_COUNT = 0x60
export const CMD_GET_SETTING_NAME = 0x61
export const CMD_GET_SETTING_INFO = 0x62
export const CMD_GET_SETTING_DEFAULT = 0x63
export const CMD_GET_CUSTOM_ENUM_COUNTS = 0x64
export const CMD_GET_CUSTOM_ENUM_VALUE_NAME = 0x65
export const CMD_GET_SETTING_CATEGORY_COUNT = 0x66
export const CMD_GET_SETTING_CATEGORY_NAME = 0x67
export const CMD_GET_SETTING_CATERORY_SETTINGS = 0x68

// device mode commands
export const CMD_SET_DEVICE_MODE = 0x70
export const CMD_GET_DEVICE_MODE = 0x71

// RF Power commands (only supported by ASPHODEL_PROTOCOL_TYPE_RF_POWER)
export const CMD_ENABLE_RF_POWER = 0x80
export const CMD_GET_RF_POWER_STATUS = 0x81
export const CMD_GET_RF_POWER_CTRL_VARS = 0x82
export const CMD_RESET_RF_POWER_TIMEOUT = 0x83

// Radio commands (only supported by ASPHODEL_PROTOCOL_TYPE_RADIO)
export const CMD_STOP_RADIO = 0x90
export const CMD_START_RADIO_SCAN = 0x91
export const CMD_GET_RADIO_SCAN_RESULTS = 0x92
export const CMD_CONNECT_RADIO = 0x93
export const CMD_GET_RADIO_STATUS = 0x94
export const CMD_GET_RADIO_CTRL_VARS = 0x95
export const CMD_GET_RADIO_DEFAULT_SERIAL = 0x96
export const CMD_START_RADIO_SCAN_BOOT = 0x97
export const CMD_CONNECT_RADIO_BOOT = 0x98
export const CMD_GET_RADIO_EXTRA_SCAN_RESULTS = 0x99
export const CMD_GET_RADIO_SCAN_POWER = 0x9F

// Remote commands (only supported by ASPHODEL_PROTOCOL_TYPE_REMOTE)
export const CMD_STOP_REMOTE = 0x9A
export const CMD_RESTART_REMOTE = 0x9B
export const CMD_GET_REMOTE_STATUS = 0x9C
export const CMD_RESTART_REMOTE_APP = 0x9D
export const CMD_RESTART_REMOTE_BOOT = 0x9E
// NOTE: 0x9F is grouped above with the radio commands

// Bootloader commands (only supported by ASPHODEL_PROTOCOL_TYPE_BOOTLOADER)
export const CMD_BOOTLOADER_START_PROGRAM = 0xA0
export const CMD_GET_BOOTLOADER_PAGE_INFO = 0xA1
export const CMD_GET_BOOTLOADER_BLOCK_SIZES = 0xA2
export const CMD_START_BOOTLOADER_PAGE = 0xA3
export const CMD_WRITE_BOOTLOADER_CODE_BLOCK = 0xA4
export const CMD_FINISH_BOOTLOADER_PAGE = 0xA5
export const CMD_VERIFY_BOOTLOADER_PAGE = 0xA6

// Commands for low-level hardware interaction. Used for testing.
export const CMD_GET_GPIO_PORT_COUNT = 0xE0
export const CMD_GET_GPIO_PORT_NAME = 0xE1
export const CMD_GET_GPIO_PORT_INFO = 0xE2
export const CMD_GET_GPIO_PORT_VALUES = 0xE3
export const CMD_SET_GPIO_PORT_MODES = 0xE4
export const CMD_DISABLE_GPIO_PORT_OVERRIDES = 0xE5
export const CMD_GET_BUS_COUNTS = 0xE6
export const CMD_SET_SPI_CS_MODE = 0xE7
export const CMD_DO_SPI_TRANSFER = 0xE8
export const CMD_DO_I2C_WRITE = 0xE9
export const CMD_DO_I2C_READ = 0xEA
export const CMD_DO_I2C_WRITE_READ = 0xEB
export const CMD_DO_RADIO_FIXED_TEST = 0xEC
export const CMD_DO_RADIO_SWEEP_TEST = 0xED

// Commands for querying device info regions. Used for testing.
export const CMD_GET_INFO_REGION_COUNT = 0xF0
export const CMD_GET_INFO_REGION_NAME = 0xF1
export const CMD_GET_INFO_REGION = 0xF2

// Misc internal testing commands. Seriously, don't use these.
export const CMD_GET_STACK_INFO = 0xF3

// Commands for echoing various bytes back to the host. Used for testing.
export const CMD_ECHO_RAW = 0xFC
export const CMD_ECHO_TRANSACTION = 0xFD
export const CMD_ECHO_PARAMS = 0xFE

// Error reply
export const CMD_REPLY_ERROR = 0xFF

// Error codes
export const ERROR_CODE_UNSPECIFIED = 0x01
export const ERROR_CODE_MALFORMED_COMMAND = 0x02
export const ERROR_CODE_UNIMPLEMENTED_COMMAND = 0x03
export const ERROR_CODE_BAD_CMD_LENGTH = 0x04
export const ERROR_CODE_BAD_ADDRESS = 0x05
export const ERROR_CODE_BAD_INDEX = 0x06
export const ERROR_CODE_INVALID_DATA = 0x07
export const ERROR_CODE_UNSUPPORTED = 0x08
export const ERROR_CODE_BAD_STATE = 0x09
export const ERROR_CODE_I2C_ERROR = 0x0A
export const ERROR_CODE_INCOMPLETE = 0x0B
// NOTE: remember to update asphodel_error_name() implementation when adding more error codes

// Unit types
export const UNIT_TYPE_NONE = 0 // should not be converted to any other unit
export const UNIT_TYPE_LSB = 1 // LSB (directly from an ADC or similar)
export const UNIT_TYPE_PERCENT = 2 // percent (unitless * 100)
export const UNIT_TYPE_VOLT = 3 // voltage
export const UNIT_TYPE_AMPERE = 4 // current
export const UNIT_TYPE_WATT = 5 // power
export const UNIT_TYPE_OHM = 6 // electrical resistance
export const UNIT_TYPE_CELSIUS = 7 // temperature
export const UNIT_TYPE_PASCAL = 8 // pressure
export const UNIT_TYPE_NEWTON = 9 // force
export const UNIT_TYPE_M_PER_S = 10 // velocity
export const UNIT_TYPE_M_PER_S2 = 11 // acceleration / gravity
export const UNIT_TYPE_DB = 12 // logarithmic (unitless)
export const UNIT_TYPE_DBM = 13 // logarithmic (power)
export const UNIT_TYPE_STRAIN = 14 // strain (unitless)
export const UNIT_TYPE_HZ = 15 // frequency
export const UNIT_TYPE_SECOND = 16 // time
export const UNIT_TYPE_LSB_PER_CELSIUS = 17 // LSB per unit temperature
export const UNIT_TYPE_GRAM_PER_S = 18 // mass flow
export const UNIT_TYPE_L_PER_S = 19 // liquid volumetric flow (see also UNIT_TYPE_M3_PER_S)
export const UNIT_TYPE_NEWTON_METER = 20 // torque
export const UNIT_TYPE_METER = 21 // length
export const UNIT_TYPE_GRAM = 22 // mass
export const UNIT_TYPE_M3_PER_S = 23 // volumetric flow (see also UNIT_TYPE_L_PER_S)
// NOTE: remember to update asphodel_unit_type_name() implementation when adding more unit types
export const UNIT_TYPE_COUNT = 24 // note: use asphodel_get_unit_type_count() to get this number

// Channel Types
export const CHANNEL_TYPE_LINEAR = 0
export const CHANNEL_TYPE_NTC = 1
export const CHANNEL_TYPE_ARRAY = 2
export const CHANNEL_TYPE_SLOW_STRAIN = 3
export const CHANNEL_TYPE_FAST_STRAIN = 4
export const CHANNEL_TYPE_SLOW_ACCEL = 5
export const CHANNEL_TYPE_PACKED_ACCEL = 6
export const CHANNEL_TYPE_COMPOSITE_STRAIN = 7
export const CHANNEL_TYPE_LINEAR_ACCEL = 8
export const CHANNEL_TYPE_BIG_ENDIAN_FLOAT32 = 9
export const CHANNEL_TYPE_BIG_ENDIAN_FLOAT64 = 10
export const CHANNEL_TYPE_LITTLE_ENDIAN_FLOAT32 = 11
export const CHANNEL_TYPE_LITTLE_ENDIAN_FLOAT64 = 12
// NOTE: remember to update asphodel_channel_type_name() implementation when adding more channel types
export const CHANNEL_TYPE_COUNT = 13 // note use asphodel_get_channel_type_count() to get this number

// Supply check result bit masks
export const ASPHODEL_SUPPLY_LOW_BATTERY = 0x01
export const ASPHODEL_SUPPLY_TOO_LOW = 0x02
export const ASPHODEL_SUPPLY_TOO_HIGH = 0x04

// Setting Types
export const SETTING_TYPE_BYTE = 0
export const SETTING_TYPE_BOOLEAN = 1
export const SETTING_TYPE_UNIT_TYPE = 2
export const SETTING_TYPE_CHANNEL_TYPE = 3
export const SETTING_TYPE_BYTE_ARRAY = 4
export const SETTING_TYPE_STRING = 5
export const SETTING_TYPE_INT32 = 6
export const SETTING_TYPE_INT32_SCALED = 7
export const SETTING_TYPE_FLOAT = 8
export const SETTING_TYPE_FLOAT_ARRAY = 9
export const SETTING_TYPE_CUSTOM_ENUM = 10
// NOTE: remember to update asphodel_setting_type_name() implementation when adding more setting types
export const SETTING_TYPE_COUNT = 11 // note use asphodel_get_setting_type_count() to get this number

// GPIO pin modes
export const GPIO_PIN_MODE_HI_Z = 0
export const GPIO_PIN_MODE_PULL_DOWN = 1
export const GPIO_PIN_MODE_PULL_UP = 2
export const GPIO_PIN_MODE_LOW = 3
export const GPIO_PIN_MODE_HIGH = 4

// SPI CS modes
export const SPI_CS_MODE_LOW = 0
export const SPI_CS_MODE_HIGH = 1
export const SPI_CS_MODE_AUTO_TRANSFER = 2
export const SPI_CS_MODE_AUTO_BYTE = 3

// Strain channel specific commands
export const STRAIN_SET_OUTPUTS = 0x01

// Accel channel specific commands
export const ACCEL_ENABLE_SELF_TEST = 0x01



export type ChannelDecoder = {
    getChannelName: () => string,
    decode: (counter: number, buffer: Uint8Array) => void,
    setConversionFactor: (scale: number, offset: number) => void,
    reset: () => void,
    stop: () => void
    getChannelBitOffset: () => number,
    getSamples: () => number,
    getSubChannels: () => number,
    getSubChannelNames: () => string[],
    setDecodeCallback: (callback: (counter: number, data: Float64Array, samples: number, subchannels: number) => void) => void,
}


export type StreamDecoder = {
    decode: (buffer: Uint8Array) => void,
    reset: () => void,
    getLastCount: () => number,
    getCounterByteOffset: () => number,
    getChannels: () => number,
    setLostPacketCallback: (callback: (current: number, last: number) => void) => void,
    getDecoders: () => ChannelDecoder[],
    getUsedBits: () => number
}

export type DeviceDecoder = {
    decode: (buffer: Uint8Array) => void,
    reset: () => void,
    getIDByteOffset: () => number,
    getStreams: () => number,
    setUnknownIDCallback: (callback: (id: number) => void) => void,
    getDecoders: () => StreamDecoder[],
    getUsedBits: () => number,
    getStreamIDs: () => Uint8Array,
}

export type StreamInfo = {
    getInfo: () => {
        channel_count: number,
        channel_index_list: Uint8Array,
        counter_bits: number,
        filler_bits: number,
        rate: number,
        rate_error: number,
        warm_up_delay: number
    }
}

export type ChannelInfo = {
    getInfo: () => {
        bits_per_sample: number,
        channel_type: number,
        //chunks: Uint8Array[],
        //coefficients: Float32Array
        data_bits: number,
        filler_bits: number,
        maximum: number,
        minimum: number,
        resolution: number,
        samples: number,
        unit_type: number,
        //name: string,
        chunk_count: number
    },

    checkAccelSelfTest: () => {
        disabled: number,
        enabled: number,
        passed: number
    },
    getAccelSelfTestLimits: () => Float32Array,
    checkStrainResistances: (bridge_index: number, baseline: number, positive_high: number, egative_high: number) => {
        positive_resistance: number,
        negative_resistance: number,
        passed: number
    },
    getStrainBridgeValues: (bridge_index: number) => Float32Array,
    getStrainBridgeSubchannel: (bridge_index: number) => number,
    getStrainBridgeCount: () => number,
}

export type StreamAndChannels = {
    getStreamInfo: () => StreamInfo,
    getChannelInfos: () => ChannelInfo[],
    new(stream_id: number, stream_info: StreamInfo, channel_infos: ChannelInfo[]): StreamAndChannels
}

export const StreamAndChannels: StreamAndChannels = asp.StreamAndChannels;

export type UnitFormatter = {
    FormatBare: (value: number) => string,
    FormatAscii: (value: number) => string,
    FormatHtml: (value: number) => string,

    getUnitAscii: () => string,
    getUnitHtml: () => string,
    getUnitUtf8: () => string

    getConversionScale: () => number,
    getConversionOffset: () => number,

    new(unit_type: number, minimum: number, maximum: number, resolution: number, use_metric: boolean): UnitFormatter,
}


export const UnitFormatter: UnitFormatter = asp.UnitFormatter;

export type Device = {
    getUserTagLocations: () => Uint32Array,
    close: () => void,
    supportsRadioCommands: () => boolean,
    supportsRemoteCommands: () => boolean,
    supportsBootloaderCommands: () => boolean,
    supportsRFPowerCommands: () => boolean,
    getProtocalVersion: () => number,
    getProtocalVersionString: () => string,
    getBoardInfo: () => { info: string, rev: number },
    getBuildInfo: () => string,
    getBuildDate: () => string,

    getCommitID: () => string,
    getRepoBranch: () => string,
    getRepoName: () => string,
    getChipFamily: () => string,
    getChipModel: () => string,
    eraseNVM: () => void,
    getNVMSize: () => number,
    getChipID: () => string,


    writeNVMRaw: (start_address: number, data: Uint8Array) => void,
    writeNVMSection: (start_address: number, data: Uint8Array) => void,
    readNVMRaw: (start_address: number, length: number) => Uint8Array,
    readNVMSection: (start_address: number, length: number) => Uint8Array,
    readUserTagString: (offset: number, length: number) => string,
    getNVMModified: () => number,
    getNVMHash: () => string,
    getSettingHash: () => string,
    reset: () => void

    getBootloaderInfo: () => string,
    bootloaderJump: () => void,
    getResetFlag: () => number,
    clearResetFlag: () => void,
    getRGBCount: () => number,
    getRGBValues: (index: number) => Uint8Array
    setRGBValues: (index: number, values: Uint8Array) => void,
    setRGBValuesHex: (index: number, values: number) => void,
    getLEDCount: () => number,
    getLEDValue: (index: number) => number,
    setLEDValue: (index: number, value: number) => void,
    getDeviceMode: () => number,
    setDeviceMode: (mode: number) => void,
    open: () => void,
    getProtocalType: () => number,
    getLocationString: () => string,
    getSerialNumber: () => string,

    doTranfer: (command: number, params: Uint8Array, call_back: (
        status: number, params: Uint8Array
    ) => void) => void,
    doTranferReset: (command: number, params: Uint8Array, call_back: (
        status: number, params: Uint8Array
    ) => void) => void,
    startStreamingPackets: (packet_count: number, transfer_count: number, timeout: number, callback: (
        status: number, stream_data: Uint8Array, packet_size: number, packet_count: number
    ) => void) => void,
    stopStreamingPackets: () => void,
    getStreamPackets: (count: number, timeout: number) => Uint8Array,
    getStreamPacketLength: () => number,
    getMaxIncomingParamLength: () => number,
    getMaxOutgoingParamLength: () => number,
    poll: (millis: number) => number,
    setConnectCallback: (callback: (status: number, connected: number) => void) => void,
    waitForConnect: (millis: number) => void,
    getRemoteDevice: () => Device,
    reconnectDevice: () => Device,
    reconnectDeviceBootloader: () => Device,
    reconnectDeviceApplication: () => Device,
    setErrorCallback: (callback: (status: number) => void) => void,
    getTransportType: () => string,
    tcpGetAdvertisement: () => {
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
    },
    stopRadio: () => void,
    startRadioScan: () => void,
    getRawRadioScanResults: (length: number) => Uint8Array,
    getRadioScanResults: (length: number) => Uint8Array,
    getRawRadioExtraScanResults: (length: number) => {
        asphodel_type: number, device_mode: number, serial_number: number
    }[],
    getRadioExtraScanResults: (length: number) => {
        asphodel_type: number, device_mode: number, serial_number: number
    }[],
    getRadioScanPower: (serials: Uint32Array) => Uint8Array,
    connectRadio: (serial: number) => void,
    getRadioStatus: () => {
        connected: number,
        serial: number,
        protocal_type: number,
        scanning: number
    },
    getRadioCtrlVars: (length: number) => {
        result: Uint8Array,
        length: number
    },
    getRadioDefaultSerial: () => number,
    startRadioScanBoot: () => void,
    connectRadioBoot: (serial: number) => void,
    stopRemote: () => void,
    restartRemote: () => void,
    restartRemoteApp: () => void,
    restartRemoteBoot: () => void,
    getRemoteStatus: () => {
        connected: number,
        serial: number,
        protocal_type: number,
        scanning: number
    },
    getStreamCount: () => {
        count: number,
        filler_bits: number,
        id_bits: number
    },
    getStream: (index: number) => StreamInfo,
    getStreamChannels: (index: number, length: number) => {
        result: Uint8Array,
        length: number
    }
    getStreamFormat: (index: number) => StreamInfo,
    enableStream: (index: number, enable: boolean) => void,
    warmUpStream: (index: number, enable: boolean) => void,
    getStreamStatus: (index: number) => {
        enable: number,
        warmup: number
    }
    getStreamRateInfo: (index: number) => {
        available: boolean,
        channel_index: number,
        invert: number,
        scale: number,
        offset: number
    },
    getChannelCount: () => number,
    getChannel: (index: number) => ChannelInfo,
    getChannelName: (index: number) => string,
    getChannelInfo: (index: number) => ChannelInfo,
    getChannelCoefficients: (index: number, length: number) => {
        result: Float32Array,
        length: number,
    },
    getChannelChunk: (index: number, chunk_number: number, chunk_length: number) => {
        result: Uint8Array,
        length: number,
    },
    getChannelSpecific: (index: number, data: Uint8Array, reply_length: number) => {
        result: Uint8Array,
        length: number,
    },
    getChannelCalibration: (index: number) => {
        available: number,
        calibration: {
            base_setting_index: number,
            maximum: number,
            minimum: number,
            offset: number
            resolution_setting_index: number,
            scale: number
        }
    },
    bootloaderStartProgram: () => void,
    getBootloaderPageInfo: () => {
        page_info: Uint32Array,
        length: number
    },
    getBootloaderBlockSizes: (length: number) => {
        result: Uint16Array,
        length: number
    },
    startBootloaderPage: (page_number: number, nonce: Uint8Array) => void,
    writeBootloaderCodeBlock: (data: Uint8Array) => number,
    writeBootloaderPage: (data: Uint8Array, block_sizes: Uint16Array) => void,
    finishBootloaderPage: (mac_tag: Uint8Array) => void,
    verifyBootloaderPage: (mac_tag: Uint8Array) => void,

    setStrainOutputs: (channel_index: number, bridge_index: number, positive_side: number, negative_side: number) => void,
    enableAccelSelfTest: (channel_index: number, enable: boolean) => void,

    getCtrlVarCount: () => number,
    getCtrlVarName: (index: number) => string,
    getCtrlVarInfo: (index: number) => {
        maximum: number,
        minimum: number,
        offset: number,
        scale: number,
        unit_type: number,
        name: string
    },
    getCtrlVar: (index: number) => number,
    setCtrlVar: (index: number, value: number) => void,

    enableRfPower: (enable: boolean) => void,
    getRfPowerStatus: () => number,
    getRfPowerCtlVars: (length: number) => {
        result: Uint8Array,
        length: number
    },
    resetRfPowerTimeout: (timeout: number) => void,

    getSupplyCount: () => number,
    getSupplyName: (index: number) => string,
    getSupplyInfo: (index: number) => {
        is_battery: number,
        nominal: number,
        offset: number,
        scale: number,
        unit_type: number,
        name: string,
    },
    checkSupply: (index: number, tries: number) => {
        measurement: number,
        result: number
    },


    getSettingCount: () => number,
    getSettingName: (index: number) => string,
    getSettingInfo: (index: number) => {
        u: any,
        name: string,
        default_bytes: Uint8Array,
        setting_type: number,
        repr_name: string
    }
    getSettingDefault: (index: number, length: number) => {
        result: Uint8Array,
        length: number
    },
    getCustomEnumCounts: (length: number) => {
        result: Uint8Array,
        length: number
    },
    getCustomEnumValueName: (index: number, value: number) => string,
    getSettingCategoryCount: () => number,
    getSettingCategoryName: (index: number) => string,
    getSettingCategorySettings: (index: number, length: number) => {
        result: Uint8Array,
        length: number
    },


    getGPIOPortCount: () => number,
    getGPIOPortName: (index: number) => string,
    getGPIOPortInfo: (index: number) => {
        floating_pins: number,
        input_pins: number,
        loaded_pins: number,
        output_pins: number,
        overridden_pins: number,
        name: string
    },
    getGPIOPortValues: (index: number) => number,
    setGPIOPortModes: (index: number, mode: number, pins: number) => number,
    disableGPIOOverrides: () => void,
    getBusCounts: () => {
        spi_count: number,
        i2c_count: number
    },
    setSpiCsMode: (index: number, cs_mode: number) => void,
    doSPITransfer: (index: number, tx_data: Uint8Array) => Uint8Array,
    doI2CWrite: (index: number, address: number, tx_data: Uint8Array) => void,
    doI2CRead: (index: number, address: number, read_length: number) => Uint8Array,
    doI2CWriteRead: (index: number, address: number, tx_data: Uint8Array, read_length: number) => Uint8Array,
    doRadioFixedTest: (channel: number, duration: number, mode: number) => void,
    doRadioSweepTest: (start_channel: number, stop_channel: number, hop_interval: number, hop_count: number, mode: number) => void,
    getInfoRegionCount: () => number,
    getInfoRegionName: (index: number) => string,
    getInfoRegion: (index: number, length: number) => {
        result: Uint8Array,
        length: number
    },
    getStackInfo: () => {
        free: number,
        used: number
    },
    echoRaw: (data: Uint8Array, reply_length: number) => Uint8Array,
    echoTransaction: (data: Uint8Array, reply_length: number) => Uint8Array,
    echoParams: (data: Uint8Array, reply_length: number) => Uint8Array,
    free: () => void
}

export const getErrorName: (err: number) => string = asp.getErrorName
export const getUnitTypeName: (t: number) => string = asp.getUnitTypeName
export const getUnitTypeCount: () => number = asp.getUnitTypeCount
export const getSettingTypeName: (s: number) => string = asp.getSettingTypeName
export const getSettingTypeCount: () => number = asp.getSettingTypeCount
export const getChannelTypeName: (c: number) => string = asp.getChannelTypeName
export const getChannelTypeCount: () => number = asp.getChannelTypeCount

export const USBInit: () => void = asp.USBInit
export const USBDeInit: () => void = asp.USBDeInit
export const USBPollDevices: (millis: number) => void = asp.USBPollDevices
export const USBFindDevices: () => Device[] = asp.USBFindDevices
export const USBGetBackendVersion: () => string = asp.USBGetBackendVersion

export enum TCPFilter {
    ASPHODEL_TCP_FILTER_DEFAULT = 0x0, // default parameters used by asphodel_tcp_find_devices()
    ASPHODEL_TCP_FILTER_PREFER_IPV6 = 0x0, // when a device SN is discovered on multiple protocols return only IPv6
    ASPHODEL_TCP_FILTER_PREFER_IPV4 = 0x1, // when a device SN is discovered on multiple protocols return only IPv4
    ASPHODEL_TCP_FILTER_ONLY_IPV6 = 0x2, // only search for devices using IPv6
    ASPHODEL_TCP_FILTER_ONLY_IPV4 = 0x3, // only search for devices using IPv4
    ASPHODEL_TCP_FILTER_RETURN_ALL = 0x4 // return each protocol instance of all devices found
}


export const TCPInit: () => void = asp.TCPInit
export const TCPDevicesSupported: () => boolean = asp.TCPDevicesSupported
export const TCPDeinit: () => void = asp.TCPDeInit
export const TCPFindDevices: () => Device[] = asp.TCPFindDevices
export const TCPFindDevicesFilter: (filter: TCPFilter, length: number) => Device[] = asp.TCPFindDevicesFilter
export const TCPPollDevices: (millis: number) => void = asp.TCPPollDevices
export const TCPCreateDevice: (host: string, port: number, timeout: number, serial: string) => Device = asp.TCPCreateDevice


export const createChannelDecoder: (channel_info: ChannelInfo, channel_bit_offset: number) => ChannelDecoder = asp.createChannelDecoder
export const createDeviceDecoder: (stream_and_channels: StreamAndChannels[], filler_bits: number, id_bits: number) => DeviceDecoder = asp.createDeviceDecoder
export const createStreamDecoder: (stream_and_channels: StreamAndChannels, stream_bit_offset: number) => StreamDecoder = asp.createStreamDecoder
export const getStreamingCounts: (stream_and_channels: StreamAndChannels[], response_time: number, buffer_time: number, timeout: number) => {
    packet_count: number,
    transfer_count: number,
    timeout: number
} = asp.getStreamingCounts

export const getLibraryProtocalVersion: () => number = asp.getLibraryProtocalVersion
export const getLibraryProtocalVersionString: () => string = asp.getLibraryProtocalVersionString
export const getLibraryBuildInfo: () => string = asp.getLibraryBuildInfo
export const getLibraryBuildDate: () => string = asp.getLibraryBuildDate

function stringToHex(str: string) {
    let hex = "";
    for (let i = 0; i < str.length; i++) {
        let chars = str.charCodeAt(i).toString(16);
        if (chars.length == 1) {
            chars = "0" + chars
        }
        hex += chars
    }
    return hex;
}

export function deviceToString(
    device: Device,
    streams_to_activate: number[],
    stream_counts: number[],
    schedule_id: string
): string {
    const board_info = device.getBoardInfo();
    const tag_locations = device.getUserTagLocations();
    const nvm_size = device.getNVMSize();
    const channel_count = device.getChannelCount();
    var calibrations: any[] = []
    var channels: any[] = []
    for (let i = 0; i < channel_count; i++) {
        try {
            calibrations.push(device.getChannelCalibration(i).calibration)
        } catch (e) {
            calibrations.push(null)
        }
        let info = device.getChannelInfo(i);

        const self = info.getInfo()
        const channel_name = device.getChannelName(i)
        const coefficients = device.getChannelCoefficients(i, 255)
        var chunks: any[] = [];
        var chunk_lengths: any[] = []
        for (let j = 0; j < self.chunk_count; j++) {
            const chunk = device.getChannelChunk(i, j, 255);
            const slice = chunk.result.slice(0, chunk.length);
            //chunks.push([...slice])
            slice.forEach((s)=>chunks.push(s))
            chunk_lengths.push(slice.length)
        }
        var coefz: any[] = []
        coefficients.result.slice(0, coefficients.length).forEach((c)=>coefz.push(c))
        channels.push({
            "_name_array": stringToHex(channel_name),
            "name_length": channel_name.length,
            "channel_type": self.channel_type,
            "unit_type": self.unit_type,
            "filler_bits": self.filler_bits,
            "data_bits": self.data_bits,
            "samples": self.samples,
            "bits_per_sample": self.bits_per_sample,
            "minimum": self.minimum,
            "maximum": self.maximum,
            "resolution": self.resolution,
            "_coefficients_array": coefz,
            "coefficients_length": coefficients.length,
            "_chunk_list": [...chunks],
            "_chunk_length_array": chunk_lengths,
            "chunk_count": self.chunk_count
        }
        )
    }

    var ctrl_vars: any[] = [];
    var ctrl_var_count = device.getCtrlVarCount();


    for (let i = 0; i < ctrl_var_count; i++) {
        const ctrl_var = device.getCtrlVar(i);
        const ctrl_var_name = device.getCtrlVarName(i);
        const ctrl_var_info = device.getCtrlVarInfo(i);

        ctrl_vars.push([
            ctrl_var_name,
            [
                ctrl_var_info.unit_type,
                ctrl_var_info.minimum,
                ctrl_var_info.maximum,
                ctrl_var_info.scale,
                ctrl_var_info.offset
            ],
            ctrl_var
        ])
    }

    const custom_enum_counts_h = device.getCustomEnumCounts(256);
    const custom_enum_counts = custom_enum_counts_h.result.slice(0, custom_enum_counts_h.length);
    const custom_enums = {};
    custom_enum_counts.forEach((count, j) => {
        var names: string[] = []
        for (let i = 0; i < count; i++) {
            names.push(
                device.getCustomEnumValueName(j, i)
            )
        }
        custom_enums[j] = names
    })

    const led_settings: number[] = [];
    const led_count = device.getLEDCount();
    for (let i = 0; i < led_count; i++) {
        led_settings.push(device.getLEDValue(i))
    }

    const rgb_settings: number[][] = [];
    const rgb_count = device.getRGBCount();

    for (let i = 0; i < rgb_count; i++) {
        //rgb_settings.push([...device.getRGBValues(i)])
        var rgb_setting: any[] = [];
        device.getRGBValues(i).forEach((r)=>rgb_setting.push(r))
        rgb_setting.push(rgb_setting)
    }

    const setting_category_count = device.getSettingCategoryCount();
    var setting_categories: any[] = [];
    for (let i = 0; i < setting_category_count; i++) {
        let category: any[] = [];
        category.push(device.getSettingCategoryName(i));
        const catset = device.getSettingCategorySettings(i, 256);
        category.push(catset.result.slice(0, catset.length))
        setting_categories.push(category)
    }


    const setting_count = device.getSettingCount();
    var settings: string[] = []
    for (let i = 0; i < setting_count; i++) {
        const setting_info = device.getSettingInfo(i);
        const setting_name = getSettingTypeName(setting_info.setting_type);
        const thi_name = device.getSettingName(i);
        var set = `<AsphodelSettingInfo {name=b'${thi_name}', name_length=${thi_name.length}, `
        set += "default_bytes="
        const sd = device.getSettingDefault(i, 255)
        const sd_slice = sd.result.slice(0, sd.length);
        for (let idx = 0; idx < sd_slice.length; idx++) {
            var chars = sd_slice[idx].toString(16);
            set += "0x"
            set += (chars.length == 1) ? "0" + chars : chars
            if (idx != sd_slice.length - 1) {
                set += ","
            }
        }

        set += `, default_bytes_length=${sd_slice.length}, setting_type=${setting_info.setting_type} (${setting_name}), `

        var u = "u=<" + setting_info.repr_name + " {";
        const keys = Object.keys(setting_info.u)

        for (let idx = 0; idx < keys.length; idx++) {
            const key = keys[idx]
            u += key
            u += "="
            u += setting_info.u[keys[idx]].toString()
            if (idx != keys.length - 1) {
                u += ", "
            }
        }

        u += "}>"
        set += u;
        set += "}>"

        settings.push(set)
    }

    const stream_count = device.getStreamCount()
    var stream_rate_infos: any[] = []
    for (let i = 0; i < stream_count.count; i++) {
        const stream_rate_info = device.getStreamRateInfo(i)
        stream_rate_infos.push([
            stream_rate_info.available,
            stream_rate_info.channel_index,
            stream_rate_info.invert,
            stream_rate_info.scale,
            stream_rate_info.offset
        ])
    }

    var streams: any[] = [];
    for (let i = 0; i < stream_count.count; i++) {
        const stream = device.getStream(i)
        const self = stream.getInfo()
        var _ch: any[] = []
        self.channel_index_list.slice(0, self.channel_count).forEach((v)=>_ch.push(v))
        streams.push({
            _channel_array: _ch,
            channel_count: self.channel_count,
            filler_bits: self.filler_bits,
            counter_bits: self.counter_bits,
            rate: self.rate,
            rate_error: self.rate_error,
            warm_up_delay: self.warm_up_delay
        })
    }

    const supply_count = device.getSupplyCount();
    var supplies: any[] = []
    var supply_results: any[] = []
    for (let i = 0; i < supply_count; i++) {
        const supply_name = device.getSupplyName(i);
        const supply_info = device.getSupplyInfo(i);
        supplies.push([
            supply_name,
            [
                supply_info.unit_type,
                supply_info.is_battery,
                supply_info.nominal,
                supply_info.scale,
                supply_info.offset
            ]
        ])
        var supply_result: any = null;
        try {
            supply_result = device.checkSupply(i, 20);
        } catch (e) { }
        supply_results.push(supply_result == null ? null : [supply_result.measurement, supply_result.result])
    }

    var radio_ctrl_vars: any = null
    try {
        radio_ctrl_vars = device.getRadioCtrlVars(255);
    } catch (e) { }

    var rf_ctrl_vars: any = null;
    try {
        var rf_ctrl = device.getRfPowerCtlVars(255)
        rf_ctrl_vars = [];
        rf_ctrl.result.slice(0, rf_ctrl.length).forEach((v)=>rf_ctrl_vars.push(v))
    } catch (e) { }

    var supports_device_mode = true;
    var device_mode: any = null;
    try {
        device_mode = device.getDeviceMode()
    } catch (e) {
        supports_device_mode = false;
    }

    var rf_power_status: any = null;
    try {
        rf_power_status = device.getRfPowerStatus()
    } catch (e) { }

    var nvm_hash: any = null;
    try {
        nvm_hash = device.getNVMHash()
    } catch (e) { }

    var nvm_modified: any = null;
    try {
        nvm_modified = device.getNVMModified()
    } catch (e) { }

    var setting_hash: any = null;
    try {
        setting_hash = device.getSettingHash()
    } catch (e) { }

    var commit_id: any = null;
    try {
        commit_id = device.getCommitID();
    } catch (e) { }

    var repo_branch: any = null
    try {
        repo_branch = device.getRepoBranch()
    } catch (e) { }

    var repo_name: any = null;
    try {
        repo_name = device.getRepoName()
    } catch (e) { }

    var nvm = device.readNVMSection(0, nvm_size);
    var nvm_str = "";
    nvm.forEach((byte) => {
        let chars = byte.toString(16)
        if (chars.length == 1) {
            chars = "0" + chars;
        }
        nvm_str += chars
    })

    return JSON.stringify({
        board_info: [
            board_info.info, board_info.rev
        ],
        bootloader_info: device.getBootloaderInfo(),
        build_date: device.getBuildDate(),
        build_info: device.getBuildInfo(),
        library_build_date: getLibraryBuildDate(),
        library_build_info: getLibraryBuildInfo(),
        library_protocol_version: getLibraryProtocalVersionString(),
        location_string: device.getLocationString(),
        max_incoming_param_length: device.getMaxIncomingParamLength(),
        max_outgoing_param_length: device.getMaxOutgoingParamLength(),
        nvm_hash: nvm_hash,
        nvm_modified: nvm_modified,
        serial_number: device.getSerialNumber(),
        setting_hash: setting_hash,
        stream_packet_length: device.getStreamPacketLength(),
        supports_bootloader: device.supportsBootloaderCommands(),
        supports_radio: device.supportsRadioCommands(),
        supports_remote: device.supportsRemoteCommands(),
        supports_rf_power: device.supportsRFPowerCommands(),
        tag_locations: [
            [tag_locations[0], tag_locations[1]],
            [tag_locations[2], tag_locations[3]],
            [tag_locations[4], tag_locations[5]],
        ],
        user_tag_1: device.readUserTagString(tag_locations[0], tag_locations[1]),
        user_tag_2: device.readUserTagString(tag_locations[2], tag_locations[3]),
        nvm: nvm_str,
        channel_calibration: calibrations,
        channels: channels,
        chip_family: device.getChipFamily(),
        chip_id: device.getChipID(),
        chip_model: device.getChipModel(),
        commit_id: commit_id,
        ctrl_vars: ctrl_vars,
        custom_enums: custom_enums,
        led_settings: led_settings,
        protocol_version: device.getProtocalVersionString(),
        repo_branch: repo_branch,
        repo_name: repo_name,
        rgb_settings: rgb_settings,
        setting_categories: setting_categories,
        settings: settings,
        stream_filler_bits: stream_count.filler_bits,
        stream_id_bits: stream_count.id_bits,
        stream_rate_info: stream_rate_infos,
        streams: streams,
        supplies: supplies,
        supply_results: supply_results,
        supports_device_mode: supports_device_mode,
        device_mode: device_mode,
        radio_ctrl_vars: radio_ctrl_vars == null ? [] : [...radio_ctrl_vars.result.slice(0, radio_ctrl_vars.length)],
        radio_default_serial: device.getRadioDefaultSerial(),
        radio_scan_power: true,
        rf_power_status: rf_power_status,
        rf_power_ctrl_vars: rf_ctrl_vars,
        streams_to_activate: streams_to_activate,
        stream_counts: stream_counts,
        schedule_id: schedule_id
    })
}



export class ApdBuilder {
    stream: WriteStream;
    path: string;

    constructor(
        device: Device,
        streams_to_activate: number[],
        stream_counts: {
            packet_count: number;
            transfer_count: number;
            timeout: number;
        },
        schedule_id: string,
        path: string
    ) {
        const dev_str = deviceToString(device, streams_to_activate, [
            stream_counts.packet_count,
            stream_counts.transfer_count,
            stream_counts.timeout
        ], schedule_id)

        const now = Date.now() / 1000
        this.path = path;
        this.stream = fs.createWriteStream(path)
        this.writeBuffer({
            timestamp: now,
            buffer: dev_str
        }, true).catch((e)=>{
            throw e
        })
    }

    public update(data: Uint8Array) {
        var now = Date.now() / 1000;
        this.writeBuffer({
            timestamp: now,
            buffer: data
        })
    }

    async writeBuffer(dat : {
        timestamp: number,
        buffer: any
    }, head = false) {
        let buffer = Buffer.alloc(12 + dat.buffer.length);
        buffer.writeDoubleBE(dat.timestamp, 0);
        buffer.writeUint32BE(dat.buffer.length, 8);
        if (head) {
            buffer.write(dat.buffer as string, 12, "utf-8");
        } else {
            (dat.buffer as Uint8Array).forEach((byte, i) => {
                buffer.writeUint8(byte, 12 + i)
            })
        }
        this.stream.write(buffer, (err)=>{
            if(err) throw err
        })
    }

    public final(filename:string) {
        this.stream.end()
        this.stream.on("finish", () => {
            console.log("all chunks have been written...")
            var compressor = lzma.createCompressor()
            var input = fs.createReadStream(this.path);
            var file = fs.createWriteStream(filename + ".apd");
            input.pipe(compressor).pipe(file);
            
            file.on("finish", () => {
                console.log("apd file written successfully: ", this.path + ".apd")
                fs.unlink(this.path, () => { });
            })
        })


    }
}