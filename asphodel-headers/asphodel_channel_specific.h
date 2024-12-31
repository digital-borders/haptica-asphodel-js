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

#ifndef ASPHODEL_CHANNEL_SPECIFIC_H_
#define ASPHODEL_CHANNEL_SPECIFIC_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"
#include "asphodel_stream.h"


#ifdef __cplusplus
extern "C" {
#endif

// Supported types: CHANNEL_TYPE_SLOW_STRAIN, CHANNEL_TYPE_FAST_STRAIN, CHANNEL_TYPE_COMPOSITE_STRAIN
// Returns the number of strain bridges present on the given channel. Other strain functions called on this channel
// will accept bridge indexes from 0 to bridge_count-1.
// NOTE: this will return 0 for any unsupported channel type.
ASPHODEL_API int asphodel_get_strain_bridge_count(AsphodelChannelInfo_t *channel_info, int *bridge_count);

// Supported types: CHANNEL_TYPE_SLOW_STRAIN, CHANNEL_TYPE_FAST_STRAIN, CHANNEL_TYPE_COMPOSITE_STRAIN
// Returns the subchannel index for a particular bridge index. This is necessary for collecting the streaming data for
// to be used with asphodel_check_strain_resistances().
ASPHODEL_API int asphodel_get_strain_bridge_subchannel(AsphodelChannelInfo_t *channel_info, int bridge_index,
		size_t *subchannel_index);

// Supported types: CHANNEL_TYPE_SLOW_STRAIN, CHANNEL_TYPE_FAST_STRAIN, CHANNEL_TYPE_COMPOSITE_STRAIN
// Fills the values array (length 5) with bridge values from the channel's chunk data.
// values[0] is the resistance of the positive sense resistor (in ohms). NOTE: 0.0 means not present.
// values[1] is the resistance of the negative sense resistor (in ohms). NOTE: 0.0 means not present.
// values[2] is the nominal resistance of the bridge elements (in ohms)
// values[3] is the minimum resistance of the bridge elements (in ohms)
// values[4] is the maximum resistance of the bridge elements (in ohms)
ASPHODEL_API int asphodel_get_strain_bridge_values(AsphodelChannelInfo_t *channel_info, int bridge_index,
		float *values);

// Supported types: CHANNEL_TYPE_SLOW_STRAIN, CHANNEL_TYPE_FAST_STRAIN, CHANNEL_TYPE_COMPOSITE_STRAIN
// Sets the output of the sense resistors on a specified strain bridge. The side inputs are booleans (0 or 1).
// Wraps a call to asphodel_channel_specific().
ASPHODEL_API int asphodel_set_strain_outputs(AsphodelDevice_t *device, int channel_index, int bridge_index,
		int positive_side, int negative_side, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_set_strain_outputs_blocking(AsphodelDevice_t *device, int channel_index, int bridge_index,
		int positive_side, int negative_side);

// Supported types: CHANNEL_TYPE_SLOW_STRAIN, CHANNEL_TYPE_FAST_STRAIN, CHANNEL_TYPE_COMPOSITE_STRAIN
// Checks the element resistances of the specified strain bridge on a channel.
// The three inputs should be measured with various parameters to asphodel_set_strain_outputs():
// baseline should be the average value with positive_size=1, negative_side=0.
// positive_high should be the average value with positive_size=1, negative_side=0.
// negative_high should be the average value with positive_size=0, negative_side=1.
// NOTE: all measurements should be made in the channel's unit type, not a derived unit.
// positive_resistance will be the calculated resistance (in ohms) for the elements on the positive side of the bridge.
// negative_resistance will be the calculated resistance (in ohms) for the elements on the negative side of the bridge.
// passed will be set to 1 if the resistances are within an acceptable range. Otherwise, it will be set to 0.
ASPHODEL_API int asphodel_check_strain_resistances(AsphodelChannelInfo_t *channel_info, int bridge_index,
		double baseline, double positive_high, double negative_high, double *positive_resistance,
		double *negative_resistance, int *passed);

// Supported types: CHANNEL_TYPE_SLOW_ACCEL, CHANNEL_TYPE_PACKED_ACCEL, CHANNEL_TYPE_LINEAR_ACCEL
// Fills the values array (length 6) with self test limits from the channel's chunk data.
// Differences are (enabled - disabled). If a given difference is between the minimum and maximum, then the channel's
// self test passes; otherwise it fails.
// limits[0] is the minimum X difference
// limits[1] is the maximum X difference
// limits[2] is the minimum Y difference
// limits[3] is the maximum Y difference
// limits[4] is the minimum Z difference
// limits[5] is the maximum Z difference
ASPHODEL_API int asphodel_get_accel_self_test_limits(AsphodelChannelInfo_t *channel_info, float *limits);

// Supported types: CHANNEL_TYPE_SLOW_ACCEL, CHANNEL_TYPE_PACKED_ACCEL, CHANNEL_TYPE_LINEAR_ACCEL
// Enables or disables the accel channel's self test functionality. Enable is a boolean (0/1).
// Wraps a call to asphodel_channel_specific().
ASPHODEL_API int asphodel_enable_accel_self_test(AsphodelDevice_t *device, int channel_index, int enable,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_enable_accel_self_test_blocking(AsphodelDevice_t *device, int channel_index, int enable);

// Supported types: CHANNEL_TYPE_SLOW_ACCEL, CHANNEL_TYPE_PACKED_ACCEL, CHANNEL_TYPE_LINEAR_ACCEL
// Checks the self test functionality of an accel channel.
// The two inputs (arrays of length 3) should be measured with asphodel_enable_accel_self_test() disabled and enabled.
// NOTE: all measurements should be made in the channel's unit type, not a derived unit.
// passed will be set to 1 if the self test values are acceptable. Otherwise, it will be set to 0.
ASPHODEL_API int asphodel_check_accel_self_test(AsphodelChannelInfo_t *channel_info, double *disabled, double *enabled, int *passed);


#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_CHANNEL_SPECIFIC_H_ */
