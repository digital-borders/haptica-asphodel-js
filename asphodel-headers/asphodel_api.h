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

#ifndef ASPHODEL_API_H_
#define ASPHODEL_API_H_

#include <stdint.h>

// cross platform definitions for shared/static library support
// see https://gcc.gnu.org/wiki/Visibility
#if defined _WIN32 || defined __CYGWIN__
#define ASPHODEL_HELPER_DLL_IMPORT __declspec(dllimport)
#define ASPHODEL_HELPER_DLL_EXPORT __declspec(dllexport)
#define ASPHODEL_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define ASPHODEL_HELPER_DLL_IMPORT __attribute__((visibility("default")))
#define ASPHODEL_HELPER_DLL_EXPORT __attribute__((visibility("default")))
#define ASPHODEL_HELPER_DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define ASPHODEL_HELPER_DLL_IMPORT
#define ASPHODEL_HELPER_DLL_EXPORT
#define ASPHODEL_HELPER_DLL_LOCAL
#endif
#endif

#ifdef ASPHODEL_STATIC_LIB // defined if asphodel library is compiled as static lib
#define ASPHODEL_API
#define ASPHODEL_LOCAL
#else						// ASPHODEL_STATIC_LIB is not defined: this means asphodel is a DLL
#ifdef ASPHODEL_API_EXPORTS // defined if we are building the library (instead of using it)
#define ASPHODEL_API ASPHODEL_HELPER_DLL_EXPORT
#ifdef ASPHODEL_MEM_TEST
#define malloc asphodel_mem_test_malloc
#define free asphodel_mem_test_free
#endif
#else
#define ASPHODEL_API ASPHODEL_HELPER_DLL_IMPORT
#endif // ASPHODEL_API_EXPORTS
#define ASPHODEL_LOCAL ASPHODEL_HELPER_DLL_LOCAL
#endif // ASPHODEL_STATIC_LIB

#ifdef __cplusplus
extern "C"
{
#endif

	enum asphodel_error
	{
		ASPHODEL_SUCCESS = 0,

		// negative numbers are internally generated. positive numbers come from the device.

		// The following errors were originally taken directly from LibUSB. Now they're converted internally
		ASPHODEL_ERROR_IO = -1, // LIBUSB_ERROR_IO
		// -2 used to be LIBUSB_ERROR_INVALID_PARAM, now translated to ASPHODEL_BAD_PARAMETER
		ASPHODEL_ACCESS_ERROR = -3, // LIBUSB_ERROR_ACCESS
		ASPHODEL_NO_DEVICE = -4,	// LIBUSB_ERROR_NO_DEVICE
		// -5 used to be LIBUSB_ERROR_NOT_FOUND, now translated to ASPHODEL_NOT_FOUND
		ASPHODEL_BUSY = -6,			// LIBUSB_ERROR_BUSY
		ASPHODEL_TIMEOUT = -7,		// LIBUSB_ERROR_TIMEOUT
		ASPHODEL_OVERFLOW = -8,		// LIBUSB_ERROR_OVERFLOW
		ASPHODEL_PIPE_ERROR = -9,	// LIBUSB_ERROR_PIPE
		ASPHODEL_INTERRUPTED = -10, // LIBUSB_ERROR_INTERRUPTED
		// -11 used to be LIBUSB_ERROR_NO_MEM, now tranlated to ASPHODEL_NO_MEM
		ASPHODEL_NOT_SUPPORTED = -12, // LIBUSB_ERROR_NOT_SUPPORTED

		ASPHODEL_TRANSPORT_ERROR = -50, // unknown/unspecified LibUSB error gets converted here
		ASPHODEL_STALL = -51,			// translated from LIBUSB_TRANSFER_STALL
		ASPHODEL_CANCELLED = -52,		// translated from LIBUSB_TRANSFER_CANCELLED

		ASPHODEL_NO_MEM = -101,						  // a malloc call returned NULL
		ASPHODEL_BAD_REPLY_LENGTH = -102,			  // got a reply from the device of unexpected length
		ASPHODEL_MALFORMED_REPLY = -103,			  // reply packet was too short to be a reply
		ASPHODEL_MALFORMED_ERROR = -104,			  // error packet was too short to contain the error code
		ASPHODEL_MISMATCHED_TRANSACTION = -105,		  // got an unexpected transaction id
		ASPHODEL_MISMATCHED_COMMAND = -106,			  // got an unexpected command response
		ASPHODEL_TRANSFER_ERROR = -107,				  // got an unknown error during a transfer
		ASPHODEL_INVALID_DESCRIPTOR = -108,			  // a usb descriptor is badly formed
		ASPHODEL_FULL_TRANSACTION_TABLE = -109,		  // the transaction table has no open spots
		ASPHODEL_DEVICE_CLOSED = -110,				  // the device is closed
		ASPHODEL_BAD_PARAMETER = -111,				  // passed an invalid parameter to a function
		ASPHODEL_COUNTER_FORMAT_UNSUPPORTED = -112,	  // could not create a counter decoder
		ASPHODEL_CHANNEL_FORMAT_UNSUPPORTED = -113,	  // could not create a channel decoder
		ASPHODEL_STREAM_ID_FORMAT_UNSUPPORTED = -114, // could not create a stream id decoder
		ASPHODEL_TOO_MANY_TRIES = -115,				  // Tried too many times without success.
		ASPHODEL_BAD_STREAM_PACKET_SIZE = -116,		  // stream packet not a multiple of the endpoint size
		ASPHODEL_BAD_CHANNEL_TYPE = -117,			  // tried to use an unsupported channel type in a channel specific call
		ASPHODEL_OUTGOING_PACKET_TOO_LONG = -118,	  // tried to send an outgoing packet too long for the device
		ASPHODEL_BAD_STREAM_RATE = -119,			  // stream rate is unintelligible
		ASPHODEL_NOT_FOUND = -120,					  // couldn't find a device
		ASPHODEL_NO_RESOURCES = -121,				  // not enough computer resources (more general than no memory)
		ASPHODEL_UNREACHABLE = -122,				  // host or network unreachable
		ASPHODEL_UNINITIALIZED = -123,				  // host or network unreachable
													  // NOTE: remember to update asphodel_error_name() implementation when adding more error codes
	};

	// Convert an error code into its string representation. For example -101 to "ASPHODEL_NO_MEM".
	// Note this will also convert libusb error codes into their string form.
	ASPHODEL_API const char *asphodel_error_name(int error_code);

	// Convert a unit type into its string representation. For example 3 to "UNIT_TYPE_VOLT".
	ASPHODEL_API const char *asphodel_unit_type_name(uint8_t unit_type);

	// Return the number of unit types supported by this library. Can be used to build a list of unit type names by calling
	// asphodel_unit_type_name() in a loop.
	ASPHODEL_API uint8_t asphodel_get_unit_type_count(void);

	// Convert a channel type into its string representation. For example 2 to "CHANNEL_TYPE_ARRAY".
	ASPHODEL_API const char *asphodel_channel_type_name(uint8_t channel_type);

	// Return the number of channel types supported by this library. Can be used to build a list of channel type names by
	// calling asphodel_channel_type_name() in a loop.
	ASPHODEL_API uint8_t asphodel_get_channel_type_count(void);

	// Convert a setting type into its string representation. For example 4 to "SETTING_TYPE_BYTE_ARRAY".
	ASPHODEL_API const char *asphodel_setting_type_name(uint8_t setting_type);

	// Return the number of setting types supported by this library. Can be used to build a list of setting type names by
	// calling asphodel_setting_type_name() in a loop.
	ASPHODEL_API uint8_t asphodel_get_setting_type_count(void);

#ifdef __cplusplus
}
#endif /* ASPHODEL_API_H_ */


#endif /* ASPHODEL_API_H_ */
