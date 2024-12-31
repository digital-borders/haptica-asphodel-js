/*
 * Copyright (c) 2019, Suprock Technologies
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef ASPHODEL_PROTOCOL_H_
#define ASPHODEL_PROTOCOL_H_

// Asphodel protocol version 2.3.3
// NOTE: use the functions in asphodel_version.h to access the protocol version
#define ASPHODEL_PROTOCOL_VERSION_MAJOR      0x02
#define ASPHODEL_PROTOCOL_VERSION_MINOR      0x03
#define ASPHODEL_PROTOCOL_VERSION_SUBMINOR   0x03

// USB class/subclass defines
// use one ASPHODEL_PROTOCOL_TYPE_* as the USB protocol definition
#define ASPHODEL_USB_CLASS                   0xFF // 0xFF: vendor specific USB class
#define ASPHODEL_USB_SUBCLASS                0x01 // 0x01: Generic Sensor

// protocol types to define various implementations
#define ASPHODEL_PROTOCOL_TYPE_BASIC         0x00 // 0x00: basic (minimum) implementation
#define ASPHODEL_PROTOCOL_TYPE_RF_POWER      0x01 // 0x01: RF Power protocol extension
#define ASPHODEL_PROTOCOL_TYPE_RADIO         0x02 // 0x02: radio interface controlling a remote interface
#define ASPHODEL_PROTOCOL_TYPE_REMOTE        0x04 // 0x04: remote interface (controlled by a radio interface)
#define ASPHODEL_PROTOCOL_TYPE_BOOTLOADER    0x08 // 0x08: firmware bootloader

// general information commands
#define CMD_GET_PROTOCOL_VERSION             0x00
#define CMD_GET_BOARD_INFO                   0x01
#define CMD_GET_USER_TAG_LOCATIONS           0x02
#define CMD_GET_BUILD_INFO                   0x03
#define CMD_GET_BUILD_DATE                   0x04
#define CMD_GET_CHIP_FAMILY                  0x05
#define CMD_GET_CHIP_MODEL                   0x06
#define CMD_GET_CHIP_ID                      0x07

// NVM commands
#define CMD_GET_NVM_SIZE                     0x08
#define CMD_ERASE_NVM                        0x09
#define CMD_WRITE_NVM                        0x0A
#define CMD_READ_NVM                         0x0B

// Flush/Reinit communication pipes
#define CMD_FLUSH                            0x0C

// reset commands
#define CMD_RESET                            0x0D
#define CMD_GET_BOOTLOADER_INFO              0x0E
#define CMD_BOOTLOADER_JUMP                  0x0F

// LED commands
#define CMD_GET_RGB_COUNT                    0x10
#define CMD_GET_RGB_VALUES                   0x11
#define CMD_SET_RGB                          0x12
#define CMD_SET_RGB_INSTANT                  0x13
#define CMD_GET_LED_COUNT                    0x14
#define CMD_GET_LED_VALUE                    0x15
#define CMD_SET_LED                          0x16
#define CMD_SET_LED_INSTANT                  0x17

// state commands
#define CMD_GET_RESET_FLAG                   0x18
#define CMD_CLEAR_RESET_FLAG                 0x19
#define CMD_GET_NVM_MODIFIED                 0x1a
#define CMD_GET_NVM_HASH                     0x1b
#define CMD_GET_SETTING_HASH                 0x1c

// extra build info
#define CMD_GET_COMMIT_ID                    0x1d
#define CMD_GET_REPO_BRANCH                  0x1e
#define CMD_GET_REPO_NAME                    0x1f

// stream commands
#define CMD_GET_STREAM_COUNT_AND_ID          0x20
#define CMD_GET_STREAM_CHANNELS              0x21
#define CMD_GET_STREAM_FORMAT                0x22
#define CMD_ENABLE_STREAM                    0x23
#define CMD_WARM_UP_STREAM                   0x24
#define CMD_GET_STREAM_STATUS                0x25
#define CMD_GET_STREAM_RATE_INFO             0x26

// channel commands
#define CMD_GET_CHANNEL_COUNT                0x30
#define CMD_GET_CHANNEL_NAME                 0x31
#define CMD_GET_CHANNEL_INFO                 0x32
#define CMD_GET_CHANNEL_COEFFICIENTS         0x33
#define CMD_GET_CHANNEL_CHUNK                0x34
#define CMD_CHANNEL_SPECIFIC                 0x35
#define CMD_GET_CHANNEL_CALIBRATION          0x36

// power supply check commands
#define CMD_GET_SUPPLY_COUNT                 0x40
#define CMD_GET_SUPPLY_NAME                  0x41
#define CMD_GET_SUPPLY_INFO                  0x42
#define CMD_CHECK_SUPPLY                     0x43

// control variable commands
#define CMD_GET_CTRL_VAR_COUNT               0x50
#define CMD_GET_CTRL_VAR_NAME                0x51
#define CMD_GET_CTRL_VAR_INFO                0x52
#define CMD_GET_CTRL_VAR                     0x53
#define CMD_SET_CTRL_VAR                     0x54

// settings commands
#define CMD_GET_SETTING_COUNT                0x60
#define CMD_GET_SETTING_NAME                 0x61
#define CMD_GET_SETTING_INFO                 0x62
#define CMD_GET_SETTING_DEFAULT              0x63
#define CMD_GET_CUSTOM_ENUM_COUNTS           0x64
#define CMD_GET_CUSTOM_ENUM_VALUE_NAME       0x65
#define CMD_GET_SETTING_CATEGORY_COUNT       0x66
#define CMD_GET_SETTING_CATEGORY_NAME        0x67
#define CMD_GET_SETTING_CATERORY_SETTINGS    0x68

// device mode commands
#define CMD_SET_DEVICE_MODE                  0x70
#define CMD_GET_DEVICE_MODE                  0x71

// RF Power commands (only supported by ASPHODEL_PROTOCOL_TYPE_RF_POWER)
#define CMD_ENABLE_RF_POWER                  0x80
#define CMD_GET_RF_POWER_STATUS              0x81
#define CMD_GET_RF_POWER_CTRL_VARS           0x82
#define CMD_RESET_RF_POWER_TIMEOUT           0x83

// Radio commands (only supported by ASPHODEL_PROTOCOL_TYPE_RADIO)
#define CMD_STOP_RADIO                       0x90
#define CMD_START_RADIO_SCAN                 0x91
#define CMD_GET_RADIO_SCAN_RESULTS           0x92
#define CMD_CONNECT_RADIO                    0x93
#define CMD_GET_RADIO_STATUS                 0x94
#define CMD_GET_RADIO_CTRL_VARS              0x95
#define CMD_GET_RADIO_DEFAULT_SERIAL         0x96
#define CMD_START_RADIO_SCAN_BOOT            0x97
#define CMD_CONNECT_RADIO_BOOT               0x98
#define CMD_GET_RADIO_EXTRA_SCAN_RESULTS     0x99
#define CMD_GET_RADIO_SCAN_POWER             0x9F

// Remote commands (only supported by ASPHODEL_PROTOCOL_TYPE_REMOTE)
#define CMD_STOP_REMOTE                      0x9A
#define CMD_RESTART_REMOTE                   0x9B
#define CMD_GET_REMOTE_STATUS                0x9C
#define CMD_RESTART_REMOTE_APP               0x9D
#define CMD_RESTART_REMOTE_BOOT              0x9E
// NOTE: 0x9F is grouped above with the radio commands

// Bootloader commands (only supported by ASPHODEL_PROTOCOL_TYPE_BOOTLOADER)
#define CMD_BOOTLOADER_START_PROGRAM         0xA0
#define CMD_GET_BOOTLOADER_PAGE_INFO         0xA1
#define CMD_GET_BOOTLOADER_BLOCK_SIZES       0xA2
#define CMD_START_BOOTLOADER_PAGE            0xA3
#define CMD_WRITE_BOOTLOADER_CODE_BLOCK      0xA4
#define CMD_FINISH_BOOTLOADER_PAGE           0xA5
#define CMD_VERIFY_BOOTLOADER_PAGE           0xA6

// Commands for low-level hardware interaction. Used for testing.
#define CMD_GET_GPIO_PORT_COUNT              0xE0
#define CMD_GET_GPIO_PORT_NAME               0xE1
#define CMD_GET_GPIO_PORT_INFO               0xE2
#define CMD_GET_GPIO_PORT_VALUES             0xE3
#define CMD_SET_GPIO_PORT_MODES              0xE4
#define CMD_DISABLE_GPIO_PORT_OVERRIDES      0xE5
#define CMD_GET_BUS_COUNTS                   0xE6
#define CMD_SET_SPI_CS_MODE                  0xE7
#define CMD_DO_SPI_TRANSFER                  0xE8
#define CMD_DO_I2C_WRITE                     0xE9
#define CMD_DO_I2C_READ                      0xEA
#define CMD_DO_I2C_WRITE_READ                0xEB
#define CMD_DO_RADIO_FIXED_TEST              0xEC
#define CMD_DO_RADIO_SWEEP_TEST              0xED

// Commands for querying device info regions. Used for testing.
#define CMD_GET_INFO_REGION_COUNT            0xF0
#define CMD_GET_INFO_REGION_NAME             0xF1
#define CMD_GET_INFO_REGION                  0xF2

// Misc internal testing commands. Seriously, don't use these.
#define CMD_GET_STACK_INFO                   0xF3

// Commands for echoing various bytes back to the host. Used for testing.
#define CMD_ECHO_RAW                         0xFC
#define CMD_ECHO_TRANSACTION                 0xFD
#define CMD_ECHO_PARAMS                      0xFE

// Error reply
#define CMD_REPLY_ERROR                      0xFF

// Error codes
#define ERROR_CODE_UNSPECIFIED               0x01
#define ERROR_CODE_MALFORMED_COMMAND         0x02
#define ERROR_CODE_UNIMPLEMENTED_COMMAND     0x03
#define ERROR_CODE_BAD_CMD_LENGTH            0x04
#define ERROR_CODE_BAD_ADDRESS               0x05
#define ERROR_CODE_BAD_INDEX                 0x06
#define ERROR_CODE_INVALID_DATA              0x07
#define ERROR_CODE_UNSUPPORTED               0x08
#define ERROR_CODE_BAD_STATE                 0x09
#define ERROR_CODE_I2C_ERROR                 0x0A
#define ERROR_CODE_INCOMPLETE                0x0B
// NOTE: remember to update asphodel_error_name() implementation when adding more error codes

// Unit types
#define UNIT_TYPE_NONE                       0 // should not be converted to any other unit
#define UNIT_TYPE_LSB                        1 // LSB (directly from an ADC or similar)
#define UNIT_TYPE_PERCENT                    2 // percent (unitless * 100)
#define UNIT_TYPE_VOLT                       3 // voltage
#define UNIT_TYPE_AMPERE                     4 // current
#define UNIT_TYPE_WATT                       5 // power
#define UNIT_TYPE_OHM                        6 // electrical resistance
#define UNIT_TYPE_CELSIUS                    7 // temperature
#define UNIT_TYPE_PASCAL                     8 // pressure
#define UNIT_TYPE_NEWTON                     9 // force
#define UNIT_TYPE_M_PER_S                    10 // velocity
#define UNIT_TYPE_M_PER_S2                   11 // acceleration / gravity
#define UNIT_TYPE_DB                         12 // logarithmic (unitless)
#define UNIT_TYPE_DBM                        13 // logarithmic (power)
#define UNIT_TYPE_STRAIN                     14 // strain (unitless)
#define UNIT_TYPE_HZ                         15 // frequency
#define UNIT_TYPE_SECOND                     16 // time
#define UNIT_TYPE_LSB_PER_CELSIUS            17 // LSB per unit temperature
#define UNIT_TYPE_GRAM_PER_S                 18 // mass flow
#define UNIT_TYPE_L_PER_S                    19 // liquid volumetric flow (see also UNIT_TYPE_M3_PER_S)
#define UNIT_TYPE_NEWTON_METER               20 // torque
#define UNIT_TYPE_METER                      21 // length
#define UNIT_TYPE_GRAM                       22 // mass
#define UNIT_TYPE_M3_PER_S                   23 // volumetric flow (see also UNIT_TYPE_L_PER_S)
// NOTE: remember to update asphodel_unit_type_name() implementation when adding more unit types
#define UNIT_TYPE_COUNT                      24 // note: use asphodel_get_unit_type_count() to get this number

// Channel Types
#define CHANNEL_TYPE_LINEAR                  0
#define CHANNEL_TYPE_NTC                     1
#define CHANNEL_TYPE_ARRAY                   2
#define CHANNEL_TYPE_SLOW_STRAIN             3
#define CHANNEL_TYPE_FAST_STRAIN             4
#define CHANNEL_TYPE_SLOW_ACCEL              5
#define CHANNEL_TYPE_PACKED_ACCEL            6
#define CHANNEL_TYPE_COMPOSITE_STRAIN        7
#define CHANNEL_TYPE_LINEAR_ACCEL            8
#define CHANNEL_TYPE_BIG_ENDIAN_FLOAT32      9
#define CHANNEL_TYPE_BIG_ENDIAN_FLOAT64      10
#define CHANNEL_TYPE_LITTLE_ENDIAN_FLOAT32   11
#define CHANNEL_TYPE_LITTLE_ENDIAN_FLOAT64   12
// NOTE: remember to update asphodel_channel_type_name() implementation when adding more channel types
#define CHANNEL_TYPE_COUNT                   13 // note use asphodel_get_channel_type_count() to get this number

// Supply check result bit masks
#define ASPHODEL_SUPPLY_LOW_BATTERY          0x01
#define ASPHODEL_SUPPLY_TOO_LOW              0x02
#define ASPHODEL_SUPPLY_TOO_HIGH             0x04

// Setting Types
#define SETTING_TYPE_BYTE                    0
#define SETTING_TYPE_BOOLEAN                 1
#define SETTING_TYPE_UNIT_TYPE               2
#define SETTING_TYPE_CHANNEL_TYPE            3
#define SETTING_TYPE_BYTE_ARRAY              4
#define SETTING_TYPE_STRING                  5
#define SETTING_TYPE_INT32                   6
#define SETTING_TYPE_INT32_SCALED            7
#define SETTING_TYPE_FLOAT                   8
#define SETTING_TYPE_FLOAT_ARRAY             9
#define SETTING_TYPE_CUSTOM_ENUM             10
// NOTE: remember to update asphodel_setting_type_name() implementation when adding more setting types
#define SETTING_TYPE_COUNT                   11 // note use asphodel_get_setting_type_count() to get this number

// GPIO pin modes
#define GPIO_PIN_MODE_HI_Z                   0
#define GPIO_PIN_MODE_PULL_DOWN              1
#define GPIO_PIN_MODE_PULL_UP                2
#define GPIO_PIN_MODE_LOW                    3
#define GPIO_PIN_MODE_HIGH                   4

// SPI CS modes
#define SPI_CS_MODE_LOW                      0
#define SPI_CS_MODE_HIGH                     1
#define SPI_CS_MODE_AUTO_TRANSFER            2
#define SPI_CS_MODE_AUTO_BYTE                3

// Strain channel specific commands
#define STRAIN_SET_OUTPUTS                   0x01

// Accel channel specific commands
#define ACCEL_ENABLE_SELF_TEST               0x01


#endif /* ASPHODEL_PROTOCOL_H_ */
