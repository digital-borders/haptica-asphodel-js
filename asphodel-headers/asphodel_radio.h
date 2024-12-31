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

#ifndef ASPHODEL_RADIO_H_
#define ASPHODEL_RADIO_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t serial_number;
	uint8_t asphodel_type;
	uint8_t device_mode;
	uint16_t _reserved;
} AsphodelExtraScanResult_t;

// Stop the radio. Works on scanning, connected, and connecting radios. Has no effect on already stopped radios.
ASPHODEL_API int asphodel_stop_radio(AsphodelDevice_t *device, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_stop_radio_blocking(AsphodelDevice_t *device);

// Start a scan with the radio. The radio will remain scanning until stopped. Scan results can be retreived using
// asphodel_get_start_radio_scan_results(). Starting a scan will remove any old scan results still in the device.
ASPHODEL_API int asphodel_start_radio_scan(AsphodelDevice_t *device, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_start_radio_scan_blocking(AsphodelDevice_t *device);

// Query the device for scanned serial numbers. The device will return at most
// floor(device->get_max_incoming_param_length()/4) serial numbers at a time. Another query should be performed to make
// sure there are no more. See asphodel_get_radio_scan_results() for a more user-friendly version.
ASPHODEL_API int asphodel_get_raw_radio_scan_results(AsphodelDevice_t *device, uint32_t *serials, size_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_raw_radio_scan_results_blocking(AsphodelDevice_t *device, uint32_t *serials,
		size_t *length);

// Will return query the device for scanned serial numbers until no more are returned. Each array entry will be unique.
// Entries are unsorted. Must call asphodel_free_radio_scan_results() on the result array when finished.
ASPHODEL_API int asphodel_get_radio_scan_results(AsphodelDevice_t *device, uint32_t **serials, size_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_radio_scan_results_blocking(AsphodelDevice_t *device, uint32_t **serials,
		size_t *length);

// Must be called on the arary returned by asphodel_get_radio_scan_results() or by
// asphodel_get_radio_scan_results_blocking() when finished with the results.
ASPHODEL_API void asphodel_free_radio_scan_results(uint32_t *serials);

// Query the device for scan results. The device will return at most floor(device->get_max_incoming_param_length()/6)
// results at a time. Another query should be performed to make sure there are no more. See
// asphodel_get_radio_extra_scan_results() for a more user-friendly version.
ASPHODEL_API int asphodel_get_raw_radio_extra_scan_results(AsphodelDevice_t *device,
		AsphodelExtraScanResult_t *results, size_t *length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_raw_radio_extra_scan_results_blocking(AsphodelDevice_t *device,
		AsphodelExtraScanResult_t *results, size_t *length);

// Will query the device for scan results until no more are returned. Each array entry will have a unique serial
// number. Entries are unsorted. Must call asphodel_free_radio_extra_scan_results() on the result array when finished.
ASPHODEL_API int asphodel_get_radio_extra_scan_results(AsphodelDevice_t *device, AsphodelExtraScanResult_t **results,
		size_t *length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_radio_extra_scan_results_blocking(AsphodelDevice_t *device,
		AsphodelExtraScanResult_t **results, size_t *length);

// Must be called on the arary returned by asphodel_get_radio_extra_scan_results() or by
// asphodel_get_radio_extra_scan_results_blocking() when finished with the results.
ASPHODEL_API void asphodel_free_radio_extra_scan_results(AsphodelExtraScanResult_t *results);

// Will return the received radio power during the scan for the specified serial numbers. A power reading of 0x7F means
// there was no information for that serial number. The array length must be less than or equal to the smaller of
// (max_outgoing_param_length / 4) and (max_incoming_param_len / 1).
ASPHODEL_API int asphodel_get_radio_scan_power(AsphodelDevice_t *device, const uint32_t *serials, int8_t *powers,
		size_t length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_radio_scan_power_blocking(AsphodelDevice_t *device, const uint32_t *serials,
		int8_t *powers, size_t length);

// Connect the radio's remote to a specific serial number.
ASPHODEL_API int asphodel_connect_radio(AsphodelDevice_t *device, uint32_t serial_number,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_connect_radio_blocking(AsphodelDevice_t *device, uint32_t serial_number);

// Retrieve the current state of the radio. The state of the radio can be determined from the return values as follows:
// Radio Stopped: connected=0, serial_number=0, protocol_type=0, scanning=0
// Radio Scanning: connected=0, serial_number=0, protocol_type=0, scanning=1
// Radio Connecting: connected=0, serial_number=<sn>, protocol_type=0, scanning=0
// Radio Connected: connected=1, serial_number=<sn>, protocol_type=<type>, scanning=0
ASPHODEL_API int asphodel_get_radio_status(AsphodelDevice_t *device, int *connected, uint32_t *serial_number,
		uint8_t *protocol_type, int *scanning, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_radio_status_blocking(AsphodelDevice_t *device, int *connected, uint32_t *serial_number,
		uint8_t *protocol_type, int *scanning);

// Return the control variable indexes that are related to radio operation. The length parameter should hold the
// maximum number of indexes to write into the array. When the command is finished it will hold the number of indexes
// reported by the device (as opposed to the number of indexes actually written to the array).
ASPHODEL_API int asphodel_get_radio_ctrl_vars(AsphodelDevice_t *device, uint8_t *ctrl_var_indexes, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_radio_ctrl_vars_blocking(AsphodelDevice_t *device, uint8_t *ctrl_var_indexes,
		uint8_t *length);

// Return the default serial number configured for use with the radio. A default serial number of 0 means no serial
// number has been set as the default, or the functionality has been disabled.
ASPHODEL_API int asphodel_get_radio_default_serial(AsphodelDevice_t *device, uint32_t *serial_number,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_radio_default_serial_blocking(AsphodelDevice_t *device, uint32_t *serial_number);

// Start a bootloader scan with the radio. The radio will remain scanning until stopped. Scan results can be retreived
// using asphodel_get_start_radio_scan_results(). Starting a scan will remove any old scan results still in the device.
ASPHODEL_API int asphodel_start_radio_scan_boot(AsphodelDevice_t *device, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_start_radio_scan_boot_blocking(AsphodelDevice_t *device);

// Connect the radio's remote to a specific serial number, in bootloader mode.
ASPHODEL_API int asphodel_connect_radio_boot(AsphodelDevice_t *device, uint32_t serial_number,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_connect_radio_boot_blocking(AsphodelDevice_t *device, uint32_t serial_number);

// Stop the remote's radio. Has no effect on already stopped radios.
ASPHODEL_API int asphodel_stop_remote(AsphodelDevice_t *device, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_stop_remote_blocking(AsphodelDevice_t *device);

// Restarts the remote's radio, with the previously connected serial number.
ASPHODEL_API int asphodel_restart_remote(AsphodelDevice_t *device, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_restart_remote_blocking(AsphodelDevice_t *device);

// Return the remote's status. Will provide the serial number of the currently connected (or last connected) device.
// If no serial number has ever been used with the device, the serial number will be 0.
ASPHODEL_API int asphodel_get_remote_status(AsphodelDevice_t *device, int *connected, uint32_t *serial_number,
		uint8_t *protocol_type, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_remote_status_blocking(AsphodelDevice_t *device, int *connected, uint32_t *serial_number,
		uint8_t *protocol_type);

// Restarts the remote's radio, with the previously connected serial number. Forces the use of application mode.
ASPHODEL_API int asphodel_restart_remote_app(AsphodelDevice_t *device, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_restart_remote_app_blocking(AsphodelDevice_t *device);

// Restarts the remote's radio, with the previously connected serial number. Forces the use of bootloader mode.
ASPHODEL_API int asphodel_restart_remote_boot(AsphodelDevice_t *device, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_restart_remote_boot_blocking(AsphodelDevice_t *device);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_RADIO_H_ */
