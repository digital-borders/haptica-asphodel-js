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

#ifndef ASPHODEL_CTRL_VAR_H_
#define ASPHODEL_CTRL_VAR_H_

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
	int32_t minimum;
	int32_t maximum;
	float scale;
	float offset;
} AsphodelCtrlVarInfo_t;


//void foo() {
//	sizeof(AsphodelCtrlVarInfo_t)
//}

// Return the number of control variables present.
ASPHODEL_API int asphodel_get_ctrl_var_count(AsphodelDevice_t *device, int *count, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_ctrl_var_count_blocking(AsphodelDevice_t *device, int *count);

// Return the name of a specific control variable in string form (UTF-8). The length parameter should hold the maximum
// number of bytes to write into buffer. Upon completion, the length parameter will hold the length of the control
// variable name not including the null terminator. The length parameter may be set larger than its initial value if
// the buffer was not big enough to hold the entire control variable name.
ASPHODEL_API int asphodel_get_ctrl_var_name(AsphodelDevice_t *device, int index, char *buffer, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_ctrl_var_name_blocking(AsphodelDevice_t *device, int index, char *buffer,
		uint8_t *length);

// Write the information for a specific control variable into ctrl_var_info.
ASPHODEL_API int asphodel_get_ctrl_var_info(AsphodelDevice_t *device, int index,
		AsphodelCtrlVarInfo_t *ctrl_var_info, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_ctrl_var_info_blocking(AsphodelDevice_t *device, int index,
		AsphodelCtrlVarInfo_t *ctrl_var_info);

// Get the value of a specific control variable.
ASPHODEL_API int asphodel_get_ctrl_var(AsphodelDevice_t *device, int index, int32_t *value,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_ctrl_var_blocking(AsphodelDevice_t *device, int index, int32_t *value);

// Set the value of a specific control variable.
ASPHODEL_API int asphodel_set_ctrl_var(AsphodelDevice_t *device, int index, int32_t value,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_set_ctrl_var_blocking(AsphodelDevice_t *device, int index, int32_t value);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_CTRL_VAR_H_ */
