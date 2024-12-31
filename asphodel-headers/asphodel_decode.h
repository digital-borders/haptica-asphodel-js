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

#ifndef ASPHODEL_DECODE_H_
#define ASPHODEL_DECODE_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"
#include "asphodel_stream.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*AsphodelDecodeCallback_t)(uint64_t counter, double *data, size_t samples, size_t subchannels,
		void * closure);

typedef struct AsphodelChannelDecoder_t {
	void (*decode)(struct AsphodelChannelDecoder_t *decoder, uint64_t counter, const uint8_t *buffer);
	void (*free_decoder)(struct AsphodelChannelDecoder_t *decoder);
	void (*reset)(struct AsphodelChannelDecoder_t *decoder);
	void (*set_conversion_factor)(struct AsphodelChannelDecoder_t *decoder, double scale, double offset);
	uint16_t channel_bit_offset; // cumulative offset to the first bit "owned" by the channel
	size_t samples; // 0 for unknown/variable
	char * channel_name; // may be "", but will not be NULL
	size_t subchannels; // >= 1
	char ** subchannel_names; // array length is subchannels. entries may be "", but will not be NULL

	AsphodelDecodeCallback_t callback;
	void * closure;

	// struct is expected to be longer to contain implementation-specific information
} AsphodelChannelDecoder_t;

// Create a channel decoder for the supplied channel. The bit offset is the number of bits from the beginning of the
// stream data to the first bit of the channel. The channel decoder is returned via the decoder parameter. The decoder
// can be freed via its free_decoder function.
ASPHODEL_API int asphodel_create_channel_decoder(AsphodelChannelInfo_t *channel_info, uint16_t channel_bit_offset,
		AsphodelChannelDecoder_t **decoder);

// NOTE: shares the same signature as unwrap_func_t
typedef uint64_t (*AsphodelCounterDecoderFunc_t)(const uint8_t *buffer, uint64_t last);

typedef void (*AsphodelLostPacketCallback_t)(uint64_t current, uint64_t last, void * closure);

typedef struct AsphodelStreamDecoder_t {
	void (*decode)(struct AsphodelStreamDecoder_t *decoder, const uint8_t *buffer);
	void (*free_decoder)(struct AsphodelStreamDecoder_t *decoder); // will free channel decoders
	void (*reset)(struct AsphodelStreamDecoder_t *decoder); // will reset channel decoders

	uint64_t last_count;
	size_t counter_byte_offset;
	AsphodelCounterDecoderFunc_t counter_decoder;

	size_t channels; // >= 1
	AsphodelChannelDecoder_t **decoders; // array of decoders, length channels

	AsphodelLostPacketCallback_t lost_packet_callback;
	void * lost_packet_closure;

	uint16_t used_bits;
} AsphodelStreamDecoder_t;

typedef struct {
	uint8_t stream_id;
	AsphodelStreamInfo_t *stream_info;
	AsphodelChannelInfo_t **channel_info; // array of channel info pointers, length stream_info->channel_count
} AsphodelStreamAndChannels_t;


//foo(){
//	sizeof(AsphodelStreamAndChannels_t)
//}

// Create a stream decoder for the supplied stream and channels. The bit offset is the number of bits from the
// beginning of the stream packet to the first bit of the stream. The stream decoder is returned via the decoder
// parameter. The decoder can be freed via its free_decoder function.
ASPHODEL_API int asphodel_create_stream_decoder(AsphodelStreamAndChannels_t *info,
		uint16_t stream_bit_offset, AsphodelStreamDecoder_t **decoder);

// NOTE: shares the same signature as unpack_id_func_t
typedef uint8_t (*AsphodelIDDecoderFunc_t)(const uint8_t *buffer);

typedef void (*AsphodelUnknownIDCallback_t)(uint8_t id, void * closure);

typedef struct AsphodelDeviceDecoder_t {
	void (*decode)(struct AsphodelDeviceDecoder_t *decoder, const uint8_t *buffer);
	void (*free_decoder)(struct AsphodelDeviceDecoder_t *decoder); // will free channel decoders
	void (*reset)(struct AsphodelDeviceDecoder_t *decoder); // will reset stream and channel decoders

	size_t id_byte_offset;
	AsphodelIDDecoderFunc_t id_decoder;

	size_t streams; // >= 1
	uint8_t *stream_ids; // array of stream ids, length streams
	AsphodelStreamDecoder_t **decoders; // array of decoders, length streams

	AsphodelUnknownIDCallback_t unknown_id_callback;
	void * unknown_id_closure;

	uint16_t used_bits; // maximum of used_bits in the stream decoders
} AsphodelDeviceDecoder_t;

// Create a device decoder for the supplied streams and channels. The filler bits are the number of bits from the
// beginning of the stream packet to the first bit of the id. The device decoder is returned via the decoder parameter.
// The decoder can be freed via its free_decoder function.
ASPHODEL_API int asphodel_create_device_decoder(AsphodelStreamAndChannels_t *info_array, uint8_t info_array_size,
		uint8_t filler_bits, uint8_t id_bits, AsphodelDeviceDecoder_t **decoder);

// Calculate reasonable packet count, transfer count and timeout. The info_array should contain the list of streams
// that will be streamed from all at once (channel information will be ignored). Response time (in seconds) determines
// how many packets should be bundled together for processing (i.e. the streaming callback will trigger with an average
// period equal to the response time). The buffer time (in seconds) determines how many transfers should be active to
// be able to hold enough data (i.e. if processing stalls there will be enough transfers running to hold buffer_time
// seconds worth of data). The desired timeout should be passed to the function. The timeout will be increased, if
// necessary, to twice the fastest packet interval (to prevent timeouts under normal circumstances).
// NOTE: this function is too simplistic if not all streams in info_array will be used at the same time. In such a case
// the streaming counts should be calculated some other way.
ASPHODEL_API int asphodel_get_streaming_counts(AsphodelStreamAndChannels_t *info_array, uint8_t info_array_size,
		double response_time, double buffer_time, int *packet_count, int *transfer_count, unsigned int *timeout);


#ifdef __cplusplus
}
#endif

//void foo(){
//	sizeof(AsphodelChannelDecoder_t);
//	sizeof(AsphodelStreamDecoder_t);
//	sizeof(AsphodelDeviceDecoder_t)
//}


#endif /* ASPHODEL_DECODE_H_ */
