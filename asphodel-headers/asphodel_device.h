/*
 * Copyright (c) 2016, Suprock Technologies
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

#ifndef ASPHODEL_DEVICE_H_
#define ASPHODEL_DEVICE_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_api.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*AsphodelTransferCallback_t)(int status, const uint8_t *params, size_t param_length, void * closure);
typedef void (*AsphodelStreamingCallback_t)(int status, const uint8_t *stream_data, size_t packet_size, size_t packet_count, void * closure);
typedef void (*AsphodelConnectCallback_t)(int status, int connected, void * closure);

typedef struct AsphodelDevice_t {
	// Holds one of ASPHODEL_PROTOCOL_TYPE_*
	int protocol_type;

	// A string that uniquely specifies the device location. Will be the same as long as the device is connected, even
	// across different processes. May change after the device is restarted or reconnected. The string is null
	// terminated UTF-8.
	const char * location_string;

	// Open the device for usage. Must be called before any others.
	int (*open_device)(struct AsphodelDevice_t * device);

	// Close the device and release any shared resources (e.g. usb handles, tcp sockets).
	void (*close_device)(struct AsphodelDevice_t *device);

	// Free memory held by the device.
	void (*free_device)(struct AsphodelDevice_t *device);

	// Copy the device's serial number (UTF-8 encoded) into the specified buffer.
	// The copy will be null terminated, and use at most buffer_size bytes (including the null).
	int (*get_serial_number)(struct AsphodelDevice_t *device, char *buffer, size_t buffer_size);

	// Start an Asphodel command transfer. The specified callback will be called when finished.
	int (*do_transfer)(struct AsphodelDevice_t *device, uint8_t command, const uint8_t *params,
			size_t param_length, AsphodelTransferCallback_t callback, void * closure);

	// Start an Asphodel command transfer that does not return (e.g. reset, bootloader jump).
	// The specified callback will be called when the transfer is finished.
	int (*do_transfer_reset)(struct AsphodelDevice_t *device, uint8_t command, const uint8_t *params,
			size_t param_length, AsphodelTransferCallback_t callback, void * closure);

	// Start a continuous set of stream transfers. The specified callback will be called after each transfer is
	// finished. The timeout is specified in milliseconds. The packet_count specifies how many packets should be lumped
	// together, if possible. The transfer_count specifies how many transfers should be run in parallel, to avoid
	// losing data while handling received data. The poll_device function must be called continually to receive data.
	int (*start_streaming_packets)(struct AsphodelDevice_t *device, int packet_count, int transfer_count,
			unsigned int timeout, AsphodelStreamingCallback_t callback, void * closure);

	// Stop the transfers started with start_streaming_packets.
	void (*stop_streaming_packets)(struct AsphodelDevice_t *device);

	// Get streaming packets in a blocking fashion. Do not mix with start_streaming_packets(). The buffer must be able
	// to hold at least count bytes. NOTE: count should be a multiple of get_stream_packet_length().
	int (*get_stream_packets_blocking)(struct AsphodelDevice_t *device, uint8_t *buffer, int *count,
			unsigned int timeout);

	// Return the maximum length of the incoming parameters on this device.
	size_t (*get_max_incoming_param_length)(struct AsphodelDevice_t * device);

	// Return the maximum length of the outgoing parameters on this device.
	size_t (*get_max_outgoing_param_length)(struct AsphodelDevice_t * device);

	// Return the size of individual stream packets. Data collected with read_stream_packets will be a multiple of
	// this size.
	size_t (*get_stream_packet_length)(struct AsphodelDevice_t * device);

	// Used to convert non-blocking functions to blocking ones by calling this in a loop.
	int (*poll_device)(struct AsphodelDevice_t * device, int milliseconds, int *completed);

	// Set the connect callback. If the device is already connected, this will immediately call the callback. The
	// callback will be called whenever the device experiences a connect or disconnect. Call this function with a NULL
	// callback to remove any previously registered callback. This function is implemented for all device types, but
	// really only makes sense in the context of remote devices. Non-remote devices will immediately call the callback
	// with the connect parameter set. Non-remote devices will never have a disconnect event.
	int (*set_connect_callback)(struct AsphodelDevice_t * device, AsphodelConnectCallback_t callback, void * closure);

	// This will wait for the device to be connected. NOTE: this will override any existing callback set with
	// set_connect_callback(). This function is implemented for all device types, but really only makes sense in the
	// context of remote devices. Non-remote devices will return immediately.
	int (*wait_for_connect)(struct AsphodelDevice_t * device, unsigned int timeout);

	// Return the radio's remote device. This function will return an error for non-radio devices. The device should be
	// freed with free_device() as usual.
	int (*get_remote_device)(struct AsphodelDevice_t * device, struct AsphodelDevice_t **remote_device);

	// This function will try to find another AsphodelDevice_t that has the same location string as the current device.
	// This can be used after a device is reset or disconnected. NOTE: this may return the same device! In that case
	// the old device should *not* be freed!
	int (*reconnect_device)(struct AsphodelDevice_t * device, struct AsphodelDevice_t **reconnected_device);

	// This callback will be called whenever there is a device error that cannot be associated with a function call.
	// NOTE: because of race conditions, the callback and closure should not be altered while the device is in use.
	int (*error_callback)(struct AsphodelDevice_t * device, int status, void * closure);
	void * error_closure;

	// Like reconnect_device, but this will try to connect to an asphodel compatible bootloader for the device.
	// The only time this makes sense is after a call to asphodel_bootloader_jump() on a device with an Asphodel
	// bootloader. All other situations should use reconnect_device() directly.
	int (*reconnect_device_bootloader)(struct AsphodelDevice_t * device, struct AsphodelDevice_t **reconnected_device);

	// Like reconnect_device(), but this will try to connect to a non-bootloader device. The only time this makes sense
	// is after a call to asphodel_bootloader_start_program() on an Asphodel bootloader. All other situations should
	// use reconnect_device() directly.
	int (*reconnect_device_application)(struct AsphodelDevice_t * device,
			struct AsphodelDevice_t **reconnected_device);

	// Internal data used by the device implementation code.
	void * implementation_info;

	// A string that specifies the asphodel transport type used by the device (e.g. "usb", "tcp").
	// The string is null terminated UTF-8.
	const char * transport_type;

	// Extra space allocated to allow backwards compatability in future library versions
	void * reserved[9];
} AsphodelDevice_t;

//void foo() {
//	sizeof(AsphodelDevice_t)
//}


typedef void (*AsphodelCommandCallback_t)(int status, void * closure);

// Return the protocol version supported by this device.
ASPHODEL_API int asphodel_get_protocol_version(AsphodelDevice_t *device, uint16_t *version,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_protocol_version_blocking(AsphodelDevice_t *device, uint16_t *version);

ASPHODEL_API int asphodel_get_protocol_version_string(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_protocol_version_string_blocking(AsphodelDevice_t *device, char *buffer,
		size_t buffer_size);

// Return the board revision number along with board name in string form (UTF-8).
ASPHODEL_API int asphodel_get_board_info(AsphodelDevice_t *device, uint8_t *rev, char *buffer, size_t buffer_size,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_board_info_blocking(AsphodelDevice_t *device, uint8_t *rev, char *buffer,
		size_t buffer_size);

// Fill an array with the user tag offsets and lengths. Locations must be an arary of length 6.
ASPHODEL_API int asphodel_get_user_tag_locations(AsphodelDevice_t *device, size_t *locations,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_user_tag_locations_blocking(AsphodelDevice_t *device, size_t *locations);

// Return the build info of the device's firmware in string form (UTF-8).
ASPHODEL_API int asphodel_get_build_info(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_build_info_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the build date of the device's firmware in string form (UTF-8).
ASPHODEL_API int asphodel_get_build_date(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_build_date_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the commit id of the device's firmware in string form (UTF-8).
ASPHODEL_API int asphodel_get_commit_id(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
	AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_commit_id_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the repository branch name of the device's firmware (UTF-8).
ASPHODEL_API int asphodel_get_repo_branch(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
	AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_repo_branch_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the repository name of the device's firmware (UTF-8).
ASPHODEL_API int asphodel_get_repo_name(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
	AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_repo_name_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the chip family of the device's processor (e.g. "XMega") in string form (UTF-8).
ASPHODEL_API int asphodel_get_chip_family(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_chip_family_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the chip model of the device's processor (e.g. "ATxmega256A3U") in string form (UTF-8).
ASPHODEL_API int asphodel_get_chip_model(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_chip_model_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the chip ID of the processor in string form (UTF-8).
ASPHODEL_API int asphodel_get_chip_id(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_chip_id_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the size of the NVM region in bytes.
ASPHODEL_API int asphodel_get_nvm_size(AsphodelDevice_t *device, size_t *size, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_nvm_size_blocking(AsphodelDevice_t *device, size_t *size);

// Erase the NVM region.
ASPHODEL_API int asphodel_erase_nvm(AsphodelDevice_t *device, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_erase_nvm_blocking(AsphodelDevice_t *device);

// Write bytes to the NVM region. The start_address is given in bytes, and must be a multiple of 4.
// The length of the data must be a multiple of 4 and must be at most 2 less than the device's maximum outgoing
// parameter length. See write_nvm_section for a more user friendly function.
ASPHODEL_API int asphodel_write_nvm_raw(AsphodelDevice_t *device, size_t start_address, const uint8_t *data,
		size_t length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_write_nvm_raw_blocking(AsphodelDevice_t *device, size_t start_address, const uint8_t *data,
		size_t length);

// Write bytes to the NVM region. The start_address is given in bytes, and must be a multiple of 4.
// The length of the data must be a multiple of 4.
ASPHODEL_API int asphodel_write_nvm_section(AsphodelDevice_t *device, size_t start_address, const uint8_t *data,
		size_t length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_write_nvm_section_blocking(AsphodelDevice_t *device, size_t start_address,
		const uint8_t *data, size_t length);

// Read bytes from the NVM region. The start_address is given in bytes, and must be a multiple of 4.
// The number of bytes read is controlled by the device. The length parameter specifies the maximum number of bytes to
// write into data. See asphodel_read_nvm_section for a more user friendly function.
ASPHODEL_API int asphodel_read_nvm_raw(AsphodelDevice_t *device, size_t start_address, uint8_t *data, size_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_read_nvm_raw_blocking(AsphodelDevice_t *device, size_t start_address, uint8_t *data,
		size_t *length);

// Read a number of bytes from a specific section of the NVM region. The start_address is given in bytes, and must be
// a multiple of 4. Will read exactly 'length' number of bytes into data.
ASPHODEL_API int asphodel_read_nvm_section(AsphodelDevice_t *device, size_t start_address, uint8_t *data,
		size_t length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_read_nvm_section_blocking(AsphodelDevice_t *device, size_t start_address, uint8_t *data,
		size_t length);

// Read a string from a user tag location. Offset and length are in bytes, and must be a multiple of 4 (guaranteed if
// they came from the get_user_tag_locations). Buffer will be written with a null-terminated UTF-8 string. Up to
// length+1 bytes will be written to the buffer.
ASPHODEL_API int asphodel_read_user_tag_string(AsphodelDevice_t *device, size_t offset, size_t length, char *buffer,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_read_user_tag_string_blocking(AsphodelDevice_t *device, size_t offset, size_t length,
		char *buffer);

// Write a string to a user tag location. Erases and rewrites the NVM. Offset and length are in bytes, and must be a
// multiple of 4 (guaranteed if they came from the get_user_tag_locations). Buffer should be a null-terminated UTF-8
// string. Additional bytes in the location will be filled with zeros.
ASPHODEL_API int asphodel_write_user_tag_string(AsphodelDevice_t *device, size_t offset, size_t length,
		const char *buffer, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_write_user_tag_string_blocking(AsphodelDevice_t *device, size_t offset, size_t length,
		const char *buffer);

// The returned modified value is true (1) when the device NVM has been modified since its last reset. This can
// indicate that the device is using a stale configuration, different from what the device settings might indicate.
ASPHODEL_API int asphodel_get_nvm_modified(AsphodelDevice_t *device, uint8_t *modified,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_nvm_modified_blocking(AsphodelDevice_t *device, uint8_t *modified);

// Return the hash of the NVM region data in string form (UTF-8). Intended for use in determining when cached NVM data
// is valid when reconnecting to a device.
ASPHODEL_API int asphodel_get_nvm_hash(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
	AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_nvm_hash_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Return the hash of the current device settings in string form (UTF-8). Intended for use in determining when cached
// device information (streams, etc) is valid when reconnecting to a device.
ASPHODEL_API int asphodel_get_setting_hash(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
	AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_setting_hash_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Performs a "soft" reset. Flushes any device side communication and disables any enabled streams.
ASPHODEL_API int asphodel_flush(AsphodelDevice_t *device, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_flush_blocking(AsphodelDevice_t *device);

// Reset the device.
ASPHODEL_API int asphodel_reset(AsphodelDevice_t *device, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_reset_blocking(AsphodelDevice_t *device);

// Return the bootloader info string for the device (e.g. "XMega AES") in string form (UTF-8).
ASPHODEL_API int asphodel_get_bootloader_info(AsphodelDevice_t *device, char *buffer, size_t buffer_size,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_bootloader_info_blocking(AsphodelDevice_t *device, char *buffer, size_t buffer_size);

// Reset the device and start the device's bootloader.
ASPHODEL_API int asphodel_bootloader_jump(AsphodelDevice_t *device, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_bootloader_jump_blocking(AsphodelDevice_t *device);

// Flag returned is 1 if the device has been reset since the last time the reset flag has been cleared. Otherwise the
// flag is 0. See also asphodel_clear_reset_flag. The combination of these two functions can be used to verify that a
// device has actually reset, since the reset command itself does not give feedback due to the device disconnecting
// during the command.
ASPHODEL_API int asphodel_get_reset_flag(AsphodelDevice_t *device, uint8_t *flag, AsphodelCommandCallback_t callback,
	void * closure);
ASPHODEL_API int asphodel_get_reset_flag_blocking(AsphodelDevice_t *device, uint8_t *flag);

// Will clear the reset flag on the device. See asphodel_get_reset_flag for usage details.
ASPHODEL_API int asphodel_clear_reset_flag(AsphodelDevice_t *device, AsphodelCommandCallback_t callback, 
	void * closure);
ASPHODEL_API int asphodel_clear_reset_flag_blocking(AsphodelDevice_t *device);

// Return the number of RGB LEDs present.
ASPHODEL_API int asphodel_get_rgb_count(AsphodelDevice_t *device, int *count, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_rgb_count_blocking(AsphodelDevice_t *device, int *count);

// Return the present setting of a specific RGB LED.
// values must be a length 3 array.
ASPHODEL_API int asphodel_get_rgb_values(AsphodelDevice_t *device, int index, uint8_t *values,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_rgb_values_blocking(AsphodelDevice_t *device, int index, uint8_t *values);

// Set the value of a specific RGB LED.
// values must be a length 3 array. The instant parameter is a boolean (0/1).
ASPHODEL_API int asphodel_set_rgb_values(AsphodelDevice_t *device, int index, const uint8_t *values, int instant,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_set_rgb_values_blocking(AsphodelDevice_t *device, int index, const uint8_t *values,
		int instant);

// Set the value of a specific RGB LED. Convenience function for specifying colors in hex.
// The instant parameter is a boolean (0/1).
ASPHODEL_API int asphodel_set_rgb_values_hex(AsphodelDevice_t *device, int index, uint32_t color, int instant,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_set_rgb_values_hex_blocking(AsphodelDevice_t *device, int index, uint32_t color,
		int instant);

// Return the number of stand-alone (not RGB) LEDs present.
ASPHODEL_API int asphodel_get_led_count(AsphodelDevice_t *device, int *count, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_led_count_blocking(AsphodelDevice_t *device, int *count);

// Return the present setting of the specific LED.
ASPHODEL_API int asphodel_get_led_value(AsphodelDevice_t *device, int index, uint8_t *value,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_led_value_blocking(AsphodelDevice_t *device, int index, uint8_t *value);

// Set the value of a specific LED. The instant parameter is a boolean (0/1).
ASPHODEL_API int asphodel_set_led_value(AsphodelDevice_t *device, int index, uint8_t value, int instant,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_set_led_value_blocking(AsphodelDevice_t *device, int index, uint8_t value, int instant);

// Set the mode of the device to a specific value.
ASPHODEL_API int asphodel_set_device_mode(AsphodelDevice_t *device, uint8_t mode, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_set_device_mode_blocking(AsphodelDevice_t *device, uint8_t mode);

// Return the present setting of the device mode.
ASPHODEL_API int asphodel_get_device_mode(AsphodelDevice_t *device, uint8_t *mode, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_device_mode_blocking(AsphodelDevice_t *device, uint8_t *mode);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_DEVICE_H_ */
