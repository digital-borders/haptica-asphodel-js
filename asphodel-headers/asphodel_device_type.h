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

#ifndef ASPHODEL_DEVICE_TYPE_H_
#define ASPHODEL_DEVICE_TYPE_H_

#include <stdint.h>
#include "asphodel_device.h"


#ifdef __cplusplus
extern "C" {
#endif

// returns 1 if the device supports RF power commands (in asphodel_rf_power.h), otherwise returns 0
ASPHODEL_API int asphodel_supports_rf_power_commands(AsphodelDevice_t *device);

// returns 1 if the device supports radio commands (in asphodel_radio.h), otherwise returns 0
ASPHODEL_API int asphodel_supports_radio_commands(AsphodelDevice_t *device);

// returns 1 if the device supports remote commands (in asphodel_radio.h), otherwise returns 0
ASPHODEL_API int asphodel_supports_remote_commands(AsphodelDevice_t *device);

// returns 1 if the device supports bootloader commands (in asphodel_bootloader.h), otherwise returns 0
ASPHODEL_API int asphodel_supports_bootloader_commands(AsphodelDevice_t *device);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_DEVICE_TYPE_H_ */
