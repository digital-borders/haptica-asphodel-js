/*
 * Copyright (c) 2015, Suprock Technologies
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

#ifndef ASPHODEL_SUPPLY_H_
#define ASPHODEL_SUPPLY_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	const uint8_t * name;
	uint8_t name_length;
	uint8_t unit_type;
	uint8_t is_battery;
	int32_t nominal;
	float scale;
	float offset;
} AsphodelSupplyInfo_t;

// Return the number of supplies present.
ASPHODEL_API int asphodel_get_supply_count(AsphodelDevice_t *device, int *count, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_supply_count_blocking(AsphodelDevice_t *device, int *count);

// Return the name of a specific supply in string form (UTF-8). The length parameter should hold the maximum number
// of bytes to write into buffer. Upon completion, the length parameter will hold the length of the supply name not
// including the null terminator. The length parameter may be set larger than its initial value if the buffer was not
// big enough to hold the entire supply name.
ASPHODEL_API int asphodel_get_supply_name(AsphodelDevice_t *device, int index, char *buffer, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_supply_name_blocking(AsphodelDevice_t *device, int index, char *buffer, uint8_t *length);

// Write the supply information for a specific supply into supply_info.
ASPHODEL_API int asphodel_get_supply_info(AsphodelDevice_t *device, int index, AsphodelSupplyInfo_t *supply_info,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_supply_info_blocking(AsphodelDevice_t *device, int index,
		AsphodelSupplyInfo_t *supply_info);

// Perform a measurement on the specified supply. If tries is greater than zero, this will no more than this many
// transfers before returning a status of ASPHODEL_TOO_MANY_TRIES. Otherwise, will try indefinitely.
ASPHODEL_API int asphodel_check_supply(AsphodelDevice_t *device, int index, int32_t *measurement, uint8_t *result,
		int tries, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_check_supply_blocking(AsphodelDevice_t *device, int index, int32_t *measurement,
		uint8_t *result, int tries);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_SUPPLY_H_ */
