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

#ifndef ASPHODEL_UNIT_FORMAT_H_
#define ASPHODEL_UNIT_FORMAT_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_api.h"


#ifdef __cplusplus
extern "C" {
#endif

// NOTE: All format functions behave similarly to C99 snprintf. If the character buffer is too small, then any
// remaining characters are discarded. The return value is the number of bytes that would have been written if the
// buffer had been large enough, not including the null termination.

typedef struct AsphodelUnitFormatter_t {
	// format the value (including number of digits) without appended the unit string.
	int (*format_bare)(struct AsphodelUnitFormatter_t *formatter, char *buffer, size_t buffer_size, double value);

	// format the value and include the ascii-only unit string (output is only ascii)
	int (*format_ascii)(struct AsphodelUnitFormatter_t *formatter, char *buffer, size_t buffer_size, double value);

	// format the value and include the unicode unit string (output is UTF-8)
	int (*format_utf8)(struct AsphodelUnitFormatter_t *formatter, char *buffer, size_t buffer_size, double value);

	// format the value and include the HTML unit string (output is only ascii)
	int (*format_html)(struct AsphodelUnitFormatter_t *formatter, char *buffer, size_t buffer_size, double value);

	// free the memory used by this formatter
	void (*free)(struct AsphodelUnitFormatter_t *formatter);

	// ascii encoded string for the display unit
	const char *unit_ascii;

	// UTF-8 encoded string for the display unit, using unicode characters
	const char *unit_utf8;

	// ascii encoded HTML string for the display unit, using HTML markup
	const char *unit_html;

	// values used to convert from the basic unit type to the display unit (should be applied before calling format_*)
	double conversion_scale;
	double conversion_offset;
} AsphodelUnitFormatter_t;

//foo() {
//	sizeof(AsphodelUnitFormatter_t)
//}

// Create a unit formatter that can be used for multiple format calls.
ASPHODEL_API AsphodelUnitFormatter_t* asphodel_create_unit_formatter(uint8_t unit_type, double minimum, double maximum,
		double resolution, int use_metric);

// Create a unit formatter for a custom unit that can be used for multiple format calls. The scale and offset are put
// directly into the output structure. Resolution is the original resolution, measured in the base unit.
ASPHODEL_API AsphodelUnitFormatter_t* asphodel_create_custom_unit_formatter(double scale, double offset,
		double resolution, const char *unit_ascii, const char *unit_utf8, const char *unit_html);


// Format a single value of the specified unit_type. Any conversion scale and offset are applied internally.
// If resolution is 0 or too low, then the default number of display digits will be used.
ASPHODEL_API int asphodel_format_value_ascii(char *buffer, size_t buffer_size, uint8_t unit_type, double resolution,
		int use_metric, double value);
ASPHODEL_API int asphodel_format_value_utf8(char *buffer, size_t buffer_size, uint8_t unit_type, double resolution,
		int use_metric, double value);
ASPHODEL_API int asphodel_format_value_html(char *buffer, size_t buffer_size, uint8_t unit_type, double resolution,
		int use_metric, double value);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_UNIT_FORMAT_H_ */
