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

#ifndef ASPHODEL_SETTING_H_
#define ASPHODEL_SETTING_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct { // used for byte, boolean, unit type and channel type settings
	uint16_t nvm_word;
	uint8_t nvm_word_byte; // 0-3
} AsphodelByteSetting_t;

typedef struct { // used for byte array, string
	uint16_t nvm_word; // where the first array element is stored (at byte 0)
	uint8_t maximum_length; // maximum array length
	uint16_t length_nvm_word; // where the actual length is stored
	uint8_t length_nvm_word_byte; // byte offset in the word (0-3)
} AsphodelByteArraySetting_t;

typedef struct {
	uint16_t nvm_word; // where the first string character is stored (at byte 0)
	uint8_t maximum_length; // maximum string length
} AsphodelStringSetting_t;

typedef struct {
	uint16_t nvm_word; // where the int32_t is stored
	int32_t minimum;
	int32_t maximum;
} AsphodelInt32Setting_t;

typedef struct {
	uint16_t nvm_word; // where the int32_t is stored
	int32_t minimum;
	int32_t maximum;
	uint8_t unit_type;
	float scale;
	float offset;
} AsphodelInt32ScaledSetting_t;

typedef struct {
	uint16_t nvm_word; // where the float is stored
	float minimum;
	float maximum;
	uint8_t unit_type;
	float scale;
	float offset;
} AsphodelFloatSetting_t;

typedef struct {
	uint16_t nvm_word; // where the first float is stored
	float minimum;
	float maximum;
	uint8_t unit_type;
	float scale;
	float offset;
	uint8_t maximum_length; // maximum array length
	uint16_t length_nvm_word; // where the actual length is stored
	uint8_t length_nvm_word_byte; // 0-3
} AsphodelFloatArraySetting_t;

typedef struct {
	uint16_t nvm_word;
	uint8_t nvm_word_byte; // 0-3
	uint8_t custom_enum_index;
} AsphodelCustomEnumSetting_t;

typedef struct {
	const uint8_t * name;
	uint8_t name_length;
	const uint8_t *default_bytes;
	uint8_t default_bytes_length;
	uint8_t setting_type;
	union {
		AsphodelByteSetting_t byte_setting;
		AsphodelByteArraySetting_t byte_array_setting;
		AsphodelStringSetting_t string_setting;
		AsphodelInt32Setting_t int32_setting;
		AsphodelInt32ScaledSetting_t int32_scaled_setting;
		AsphodelFloatSetting_t float_setting;
		AsphodelFloatArraySetting_t float_array_setting;
		AsphodelCustomEnumSetting_t custom_enum_setting;
	} u;
} AsphodelSettingInfo_t;

//foo() {
//	sizeof(AsphodelSettingInfo_t)
//}

// Return the number of settings present.
ASPHODEL_API int asphodel_get_setting_count(AsphodelDevice_t *device, int *count, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_setting_count_blocking(AsphodelDevice_t *device, int *count);

// Return the name of a specific setting in string form (UTF-8). The length parameter should hold the maximum number of
// bytes to write into buffer. Upon completion, the length parameter will hold the length of the setting name not
// including the null terminator. The length parameter may be set larger than its initial value if the buffer was not
// big enough to hold the entire setting name.
ASPHODEL_API int asphodel_get_setting_name(AsphodelDevice_t *device, int index, char *buffer, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_setting_name_blocking(AsphodelDevice_t *device, int index, char *buffer,
		uint8_t *length);

// Write the information for a specific setting into setting_info.
ASPHODEL_API int asphodel_get_setting_info(AsphodelDevice_t *device, int index, AsphodelSettingInfo_t *setting_info,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_setting_info_blocking(AsphodelDevice_t *device, int index,
		AsphodelSettingInfo_t *setting_info);

// Fill an array with the default bytes for the specified setting. The length parameter should hold the maximum number
// of bytes to write into the array. When the command is finished, the length parameter will hold the size of the
// default bytes (as opposed to the number of bytes actually written to the array).
ASPHODEL_API int asphodel_get_setting_default(AsphodelDevice_t *device, int index, uint8_t *default_bytes,
		uint8_t *length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_setting_default_blocking(AsphodelDevice_t *device, int index, uint8_t *default_bytes,
		uint8_t *length);

// Return the number of elements for each custom enumeration on the device. The length parameter should hold the
// maximum number of counts to write into the array. When the command is finished it will hold the number of custom
// enumerations present on the device (as opposed to the number of counts actually written to the array).
ASPHODEL_API int asphodel_get_custom_enum_counts(AsphodelDevice_t *device, uint8_t *counts, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_custom_enum_counts_blocking(AsphodelDevice_t *device, uint8_t *counts, uint8_t *length);

// Return the name of a specific custom enumeration value in string form (UTF-8). The length parameter should hold the
// maximum number of bytes to write into buffer. Upon completion, the length parameter will hold the length of the
// custom enumeration value name not including the null terminator. The length parameter may be set larger than its
// initial value if the buffer was not big enough to hold the entire channel name.
ASPHODEL_API int asphodel_get_custom_enum_value_name(AsphodelDevice_t *device, int index, int value, char *buffer,
		uint8_t *length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_custom_enum_value_name_blocking(AsphodelDevice_t *device, int index, int value,
		char *buffer, uint8_t *length);

// Return the number of setting categories present.
ASPHODEL_API int asphodel_get_setting_category_count(AsphodelDevice_t *device, int *count,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_setting_category_count_blocking(AsphodelDevice_t *device, int *count);

// Return the name of a specific setting category in string form (UTF-8). The length parameter should hold the maximum
// number of bytes to write into buffer. Upon completion, the length parameter will hold the length of the setting
// category name not including the null terminator. The length parameter may be set larger than its initial value if
// the buffer was not big enough to hold the entire setting category name.
ASPHODEL_API int asphodel_get_setting_category_name(AsphodelDevice_t *device, int index, char *buffer, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_setting_category_name_blocking(AsphodelDevice_t *device, int index, char *buffer,
		uint8_t *length);

// Return the setting indexes for a specific setting category. The length parameter should hold the maximum number of
// indexes to write into the array. When the command is finished it will hold the number of settings present on the
// setting category (as opposed to the number of indexes actually written to the array).
ASPHODEL_API int asphodel_get_setting_category_settings(AsphodelDevice_t *device, int index, uint8_t *settings,
		uint8_t *length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_setting_category_settings_blocking(AsphodelDevice_t *device, int index,
		uint8_t *settings, uint8_t *length);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_SETTING_H_ */
