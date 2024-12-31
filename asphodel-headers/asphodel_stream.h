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

#ifndef ASPHODEL_STREAM_H_
#define ASPHODEL_STREAM_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	const uint8_t * channel_index_list;
	uint8_t channel_count;
	uint8_t filler_bits;
	uint8_t counter_bits;
	float rate;
	float rate_error;
	float warm_up_delay;
} AsphodelStreamInfo_t;

typedef struct {
	const uint8_t * name;
	uint8_t name_length;
	uint8_t channel_type;
	uint8_t unit_type;
	uint16_t filler_bits;
	uint16_t data_bits;
	uint8_t samples;
	int16_t bits_per_sample;
	float minimum;
	float maximum;
	float resolution;
	const float * coefficients;
	uint8_t coefficients_length;
	const uint8_t ** chunks;
	uint8_t *chunk_lengths;
	uint8_t chunk_count;
} AsphodelChannelInfo_t;

typedef struct {
	// NOTE: the five calibration settings are always in order as: unit, scale, offset, minimum and maximum.
	int base_setting_index; // index of the first setting (i.e. unit)
	int resolution_setting_index; // may optionally be an invalid setting number
	float scale;
	float offset;
	float minimum; // if not finite (e.g. NaN), then no min setting available
	float maximum; // if not finite (e.g. NaN), then no max setting available
} AsphodelChannelCalibration_t;


//void foo() {
//	sizeof(AsphodelChannelCalibration_t);
//	sizeof(AsphodelChannelInfo_t);
//	sizeof(AsphodelStreamInfo_t)
//}

// Return the number of streams present and ID size information
ASPHODEL_API int asphodel_get_stream_count(AsphodelDevice_t *device, int *count, uint8_t *filler_bits,
		uint8_t *id_bits, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_stream_count_blocking(AsphodelDevice_t *device, int *count, uint8_t *filler_bits,
		uint8_t *id_bits);

// Allocate and fill a AsphodelStreamInfo_t structure. Must be freed with asphodel_free_stream() when finished.
ASPHODEL_API int asphodel_get_stream(AsphodelDevice_t *device, int index, AsphodelStreamInfo_t **stream_info,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_stream_blocking(AsphodelDevice_t *device, int index, AsphodelStreamInfo_t **stream_info);

// Free a stream created by asphodel_get_stream() or asphodel_get_stream_blocking(). Streams created any other way
// must NOT be used with this function.
ASPHODEL_API void asphodel_free_stream(AsphodelStreamInfo_t *stream_info);

// Return the channel indexes for a specific stream. The length parameter should hold the maximum number of indexes to
// write into the array. When the command is finished it will hold the number of channels present on the stream (as
// opposed to the number of indexes actually written to the array).
ASPHODEL_API int asphodel_get_stream_channels(AsphodelDevice_t *device, int index, uint8_t *channels, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_stream_channels_blocking(AsphodelDevice_t *device, int index, uint8_t *channels,
		uint8_t *length);

// Write the stream information for a specific stream into stream_info.
ASPHODEL_API int asphodel_get_stream_format(AsphodelDevice_t *device, int index, AsphodelStreamInfo_t *stream_info,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_stream_format_blocking(AsphodelDevice_t *device, int index,
		AsphodelStreamInfo_t *stream_info);

// Enable or disable a specific stream. The enable parameter is a boolean (0/1).
ASPHODEL_API int asphodel_enable_stream(AsphodelDevice_t *device, int index, int enable,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_enable_stream_blocking(AsphodelDevice_t *device, int index, int enable);

// Enable or disable a specific stream's warm up function. The enable parameter is a boolean (0/1).
ASPHODEL_API int asphodel_warm_up_stream(AsphodelDevice_t *device, int index, int enable,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_warm_up_stream_blocking(AsphodelDevice_t *device, int index, int enable);

// Return the enable and warm up status of a specific stream.
ASPHODEL_API int asphodel_get_stream_status(AsphodelDevice_t *device, int index, int *enable, int *warm_up,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_stream_status_blocking(AsphodelDevice_t *device, int index, int *enable, int *warm_up);

// Return stream rate channel information. The available parameter is a boolean (0/1). NOTE: if available is zero then
// the other function outputs have not been written.
ASPHODEL_API int asphodel_get_stream_rate_info(AsphodelDevice_t *device, int index, int *available, int *channel_index,
		int *invert, float *scale, float *offset, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_stream_rate_info_blocking(AsphodelDevice_t *device, int index, int *available,
		int *channel_index, int *invert, float *scale, float *offset);

// Return the number of channels present.
ASPHODEL_API int asphodel_get_channel_count(AsphodelDevice_t *device, int *count, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_get_channel_count_blocking(AsphodelDevice_t *device, int *count);

// Allocate and fill a AsphodelChannelInfo_t structure. Must be freed with asphodel_free_channel() when finished.
ASPHODEL_API int asphodel_get_channel(AsphodelDevice_t *device, int index, AsphodelChannelInfo_t **channel_info,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_channel_blocking(AsphodelDevice_t *device, int index,
		AsphodelChannelInfo_t **channel_info);

// Free a channel created by asphodel_get_channel() or asphodel_get_channel_blocking(). Channels created any other way
// must NOT be used with this function.
ASPHODEL_API void asphodel_free_channel(AsphodelChannelInfo_t *channel_info);

// Return the name of a specific channel in string form (UTF-8). The length parameter should hold the maximum number
// of bytes to write into buffer. Upon completion, the length parameter will hold the length of the channel name
// not including the null terminator. The length parameter may be set larger than its initial value if the buffer
// was not big enough to hold the entire channel name.
ASPHODEL_API int asphodel_get_channel_name(AsphodelDevice_t *device, int index, char *buffer, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_channel_name_blocking(AsphodelDevice_t *device, int index, char *buffer,
		uint8_t *length);

// Write the channel information for a specific channel into channel_info.
ASPHODEL_API int asphodel_get_channel_info(AsphodelDevice_t *device, int index, AsphodelChannelInfo_t *channel_info,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_channel_info_blocking(AsphodelDevice_t *device, int index,
		AsphodelChannelInfo_t *channel_info);

// Fill an array with the coefficients from the specified channel. The length parameter should hold the maximum number
// of coefficients to write into the array. When the command is finished it will hold the number of coefficients
// present on the channel (as opposed to the number of coefficients actually written to the array).
ASPHODEL_API int asphodel_get_channel_coefficients(AsphodelDevice_t *device, int index, float *coefficients,
		uint8_t *length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_channel_coefficients_blocking(AsphodelDevice_t *device, int index, float *coefficients,
		uint8_t *length);

// Fill an array with a chunk for the specified channel. The length parameter should hold the maximum number
// of bytes to write into the array. When the command is finished, the length parameter will hold the size of the
// chunk (as opposed to the number of bytes actually written to the array).
ASPHODEL_API int asphodel_get_channel_chunk(AsphodelDevice_t *device, int index, uint8_t chunk_number,
		uint8_t *chunk, uint8_t *length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_channel_chunk_blocking(AsphodelDevice_t *device, int index, uint8_t chunk_number,
		uint8_t *chunk, uint8_t *length);

// Performa a channel specific transfer. The format of the data depends on the channel type. The reply_length parameter
// should hold the maximum number of bytes to write into the reply array. When the command is finished, the
// reply_length parameter will hold the size of the recieved reply (as opposed to the number of bytes actually written
// to the reply array).
ASPHODEL_API int asphodel_channel_specific(AsphodelDevice_t *device, int index, uint8_t *data, uint8_t data_length,
		uint8_t *reply, uint8_t *reply_length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_channel_specific_blocking(AsphodelDevice_t *device, int index, uint8_t *data,
		uint8_t data_length, uint8_t *reply, uint8_t *reply_length);

// Return channel calibration information. The available parameter is a boolean (0/1). NOTE: if available is zero then
// the calibration structure values have not been written.
ASPHODEL_API int asphodel_get_channel_calibration(AsphodelDevice_t *device, int index, int *available,
		AsphodelChannelCalibration_t *calibration, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_channel_calibration_blocking(AsphodelDevice_t *device, int index, int *available,
		AsphodelChannelCalibration_t *calibration);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_STREAM_H_ */
