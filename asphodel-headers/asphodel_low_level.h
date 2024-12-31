/*
 * Copyright (c) 2017, Suprock Technologies
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

#ifndef ASPHODEL_LOW_LEVEL_H_
#define ASPHODEL_LOW_LEVEL_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	const uint8_t * name;
	uint8_t name_length;
	uint32_t input_pins; // pins that are inputs (i.e. their read value cannot be controlled)
	uint32_t output_pins; // pins that are outputs
	uint32_t floating_pins; // pins that are not connected to anything
	uint32_t loaded_pins; // output pins that are loaded (i.e. they may not respond to pull-up or pull-down)
	uint32_t overridden_pins; // pins that are connected to dedicated hardware functionality, and cannot be controlled
			// without calling asphodel_disable_gpio_overrides() first.
} AsphodelGPIOPortInfo_t;

//void foo() {
//	sizeof(AsphodelGPIOPortInfo_t)
//}

// Return the number of GPIO ports present.
ASPHODEL_API int asphodel_get_gpio_port_count(AsphodelDevice_t *device, int *count, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_gpio_port_count_blocking(AsphodelDevice_t *device, int *count);

// Return the name of a specific GPIO port in string form (UTF-8). The length parameter should hold the maximum number
// of bytes to write into buffer. Upon completion, the length parameter will hold the length of the GPIO port name not
// including the null terminator. The length parameter may be set larger than its initial value if the buffer was not
// big enough to hold the entire GPIO port name.
ASPHODEL_API int asphodel_get_gpio_port_name(AsphodelDevice_t *device, int index, char *buffer, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_gpio_port_name_blocking(AsphodelDevice_t *device, int index, char *buffer,
		uint8_t *length);

// Write the information for a specific GPIO port into gpio_port_info.
ASPHODEL_API int asphodel_get_gpio_port_info(AsphodelDevice_t *device, int index,
		AsphodelGPIOPortInfo_t *gpio_port_info, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_gpio_port_info_blocking(AsphodelDevice_t *device, int index,
		AsphodelGPIOPortInfo_t *gpio_port_info);

// Get the pin values of a specific GPIO port.
ASPHODEL_API int asphodel_get_gpio_port_values(AsphodelDevice_t *device, int index, uint32_t *pin_values,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_gpio_port_values_blocking(AsphodelDevice_t *device, int index, uint32_t *pin_values);

// Set the pin mode for a set of pins on a specific GPIO port.
ASPHODEL_API int asphodel_set_gpio_port_modes(AsphodelDevice_t *device, int index, uint8_t mode, uint32_t pins,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_set_gpio_port_modes_blocking(AsphodelDevice_t *device, int index, uint8_t mode,
		uint32_t pins);

// Disable hardware overrides on all GPIO pins. Only a device reset can restore the device to normal operations.
ASPHODEL_API int asphodel_disable_gpio_overrides(AsphodelDevice_t *device, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_disable_gpio_overrides_blocking(AsphodelDevice_t *device);

// Return the number of SPI and I2C busses present.
ASPHODEL_API int asphodel_get_bus_counts(AsphodelDevice_t *device, int *spi_count, int *i2c_count,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_bus_counts_blocking(AsphodelDevice_t *device, int *spi_count, int *i2c_count);

// Set the CS mode for a specific SPI bus.
ASPHODEL_API int asphodel_set_spi_cs_mode(AsphodelDevice_t *device, int index, uint8_t cs_mode,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_set_spi_cs_mode_blocking(AsphodelDevice_t *device, int index, uint8_t cs_mode);

// Does a transfer on the specified SPI bus. The TX data is transmitted. The RX data buffer must be at least as long
// as the transmission length.
ASPHODEL_API int asphodel_do_spi_transfer(AsphodelDevice_t *device, int index, const uint8_t *tx_data,
		uint8_t *rx_data, uint8_t data_length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_do_spi_transfer_blocking(AsphodelDevice_t *device, int index, const uint8_t *tx_data,
		uint8_t *rx_data, uint8_t data_length);

// Does a write to the given 7-bit address on the specified I2C bus.
ASPHODEL_API int asphodel_do_i2c_write(AsphodelDevice_t *device, int index, uint8_t addr, const uint8_t *tx_data,
		uint8_t write_length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_do_i2c_write_blocking(AsphodelDevice_t *device, int index, uint8_t addr,
		const uint8_t *tx_data, uint8_t write_length);

// Does a read from the given 7-bit address on the specified I2C bus.
ASPHODEL_API int asphodel_do_i2c_read(AsphodelDevice_t *device, int index, uint8_t addr, uint8_t *rx_data,
		uint8_t read_length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_do_i2c_read_blocking(AsphodelDevice_t *device, int index, uint8_t addr,
		uint8_t *rx_data, uint8_t read_length);

// Does a write, then a read from the given 7-bit address on the specified I2C bus.
ASPHODEL_API int asphodel_do_i2c_write_read(AsphodelDevice_t *device, int index, uint8_t addr, const uint8_t *tx_data,
		uint8_t write_length, uint8_t *rx_data, uint8_t read_length, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_do_i2c_write_read_blocking(AsphodelDevice_t *device, int index, uint8_t addr,
		const uint8_t *tx_data, uint8_t write_length, uint8_t *rx_data, uint8_t read_length);

// Do a fixed channel test with the radio hardware. For testing purposes only.
ASPHODEL_API int asphodel_do_radio_fixed_test(AsphodelDevice_t *device, uint16_t channel, uint16_t duration,
		uint8_t mode, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_do_radio_fixed_test_blocking(AsphodelDevice_t *device, uint16_t channel, uint16_t duration,
		uint8_t mode);

// Do a sweep test with the radio hardware. For testing purposes only.
ASPHODEL_API int asphodel_do_radio_sweep_test(AsphodelDevice_t *device, uint16_t start_channel, uint16_t stop_channel,
		uint16_t hop_interval, uint16_t hop_count, uint8_t mode, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_do_radio_sweep_test_blocking(AsphodelDevice_t *device, uint16_t start_channel,
		uint16_t stop_channel, uint16_t hop_interval, uint16_t hop_count, uint8_t mode);

// Return the number of info regions present. For testing purposes only.
ASPHODEL_API int asphodel_get_info_region_count(AsphodelDevice_t *device, int *count, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_info_region_count_blocking(AsphodelDevice_t *device, int *count);

// Return the name of a specific info region in string form (UTF-8). The length parameter should hold the maximum number
// of bytes to write into buffer. Upon completion, the length parameter will hold the length of the info region name not
// including the null terminator. The length parameter may be set larger than its initial value if the buffer was not
// big enough to hold the entire info region name. For testing purposes only.
ASPHODEL_API int asphodel_get_info_region_name(AsphodelDevice_t *device, int index, char *buffer, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_info_region_name_blocking(AsphodelDevice_t *device, int index, char *buffer,
		uint8_t *length);

// Reads data from a specific info region. The length parameter should hold the maximum number of bytes to write into
// the array. When the command is finished it will hold the number of bytes present in the info region (as opposed to
// the number of bytes actually written to the array). For testing purposes only.
ASPHODEL_API int asphodel_get_info_region(AsphodelDevice_t *device, int index, uint8_t *data, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_info_region_blocking(AsphodelDevice_t *device, int index, uint8_t *data,
		uint8_t *length);

// Get stack info. stack_info should point to an array of size 2. stack_info[0] is free bytes. stack_info[1] is used
// bytes. For testing purposes only.
ASPHODEL_API int asphodel_get_stack_info(AsphodelDevice_t *device, uint32_t *stack_info,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_stack_info_blocking(AsphodelDevice_t *device, uint32_t *stack_info);

// Echo raw bytes. For testing purposes only.
ASPHODEL_API int asphodel_echo_raw(AsphodelDevice_t *device, const uint8_t *data, size_t data_length, uint8_t *reply,
		size_t *reply_length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_echo_raw_blocking(AsphodelDevice_t *device, const uint8_t *data, size_t data_length,
		uint8_t *reply, size_t *reply_length);

// Echo bytes as transaction. For testing purposes only.
ASPHODEL_API int asphodel_echo_transaction(AsphodelDevice_t *device, const uint8_t *data, size_t data_length,
		uint8_t *reply, size_t *reply_length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_echo_transaction_blocking(AsphodelDevice_t *device, const uint8_t *data, size_t data_length,
		uint8_t *reply, size_t *reply_length);

// Echo parameters. For testing purposes only.
ASPHODEL_API int asphodel_echo_params(AsphodelDevice_t *device, const uint8_t *data, size_t data_length,
		uint8_t *reply, size_t *reply_length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_echo_params_blocking(AsphodelDevice_t *device, const uint8_t *data, size_t data_length,
		uint8_t *reply, size_t *reply_length);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_LOW_LEVEL_H_ */
