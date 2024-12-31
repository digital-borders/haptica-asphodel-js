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

#ifndef ASPHODEL_RF_POWER_H_
#define ASPHODEL_RF_POWER_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"

#ifdef __cplusplus
extern "C" {
#endif


// Enable or disable the RF power output.
ASPHODEL_API int asphodel_enable_rf_power(AsphodelDevice_t *device, int enable, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_enable_rf_power_blocking(AsphodelDevice_t *device, int enable);

// Retrieve the enabled state of the RF power output.
ASPHODEL_API int asphodel_get_rf_power_status(AsphodelDevice_t *device, int *enabled,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_rf_power_status_blocking(AsphodelDevice_t *device, int *enabled);

// Return the control variable indexes that are related to RF power transmission. The length parameter should hold the
// maximum number of indexes to write into the array. When the command is finished it will hold the number of indexes
// reported by the device (as opposed to the number of indexes actually written to the array).
ASPHODEL_API int asphodel_get_rf_power_ctrl_vars(AsphodelDevice_t *device, uint8_t *ctrl_var_indexes, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_rf_power_ctrl_vars_blocking(AsphodelDevice_t *device, uint8_t *ctrl_var_indexes,
		uint8_t *length);

// Sets or resets the RF Power timeout. The timeout parameter is specified in milliseconds. If the timeout duration
// passes without the device receiving another timeout reset command, then the device will disable the RF power output
// (if applicable). Sending a timeout value of 0 will disable the timeout functionality.
ASPHODEL_API int asphodel_reset_rf_power_timeout(AsphodelDevice_t *device, uint32_t timeout,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_reset_rf_power_timeout_blocking(AsphodelDevice_t *device, uint32_t timeout);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_RF_POWER_H_ */
