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

#ifndef ASPHODEL_BOOTLOADER_H_
#define ASPHODEL_BOOTLOADER_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_device.h"

#ifdef __cplusplus
extern "C" {
#endif


// Start the main program from the bootloader. Implicitly resets the device, like asphodel_reset().
ASPHODEL_API int asphodel_bootloader_start_program(AsphodelDevice_t *device, AsphodelCommandCallback_t callback,
		void * closure);
ASPHODEL_API int asphodel_bootloader_start_program_blocking(AsphodelDevice_t *device);

// Returns page information from the bootloader. The length parameter should hold the maximum number of entries to
// write into the array. When the command is finished it will hold the number of entries available on the device (as
// opposed to the number of entries actually written to the array). Entries are paris of page count and page size, in
// that order. The total number of pages is the sum of all page counts. The total number of bytes is the sum of the
// products of each entry pair (i.e. page count * page size).
ASPHODEL_API int asphodel_get_bootloader_page_info(AsphodelDevice_t *device, uint32_t *page_info, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_bootloader_page_info_blocking(AsphodelDevice_t *device, uint32_t *page_info,
		uint8_t *length);

// Fill an array with the allowed code block sizes for the device. The length parameter should hold the maximum number
// of block sizes to write into the array. When the command is finished it will hold the number of block sizes
// available on the device (as opposed to the number of block sizes actually written to the array).
// The array of code block sizes will be sorted, ascending. There will be no duplicates, and all values will be greater
// than zero.
ASPHODEL_API int asphodel_get_bootloader_block_sizes(AsphodelDevice_t *device, uint16_t *block_sizes, uint8_t *length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_get_bootloader_block_sizes_blocking(AsphodelDevice_t *device, uint16_t *block_sizes,
		uint8_t *length);

// Prepares a page for writing. Must be called before writing code blocks or verifying pages. The nonce, if any, is
// used by the device to decode the written code blocks.
ASPHODEL_API int asphodel_start_bootloader_page(AsphodelDevice_t *device, uint32_t page_number, uint8_t *nonce,
		size_t length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_start_bootloader_page_blocking(AsphodelDevice_t *device, uint32_t page_number,
		uint8_t *nonce, size_t length);

// Write a code block to the current page. Code blocks must be sent strictly sequentally. Blocks are only accepted by
// the device in certain sizes. See asphodel_get_bootloader_block_sizes() for allowable code block sizes.
// NOTE: after the final code block is written to a page, the page must be finished with a call to
// asphodel_finish_bootloader_page().
ASPHODEL_API int asphodel_write_bootloader_code_block(AsphodelDevice_t *device, uint8_t *data, size_t length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_write_bootloader_code_block_blocking(AsphodelDevice_t *device, uint8_t *data, size_t length);

// Wrapper which calls asphodel_write_bootloader_code_block() repeatedly with valid block sizes.
ASPHODEL_API int asphodel_write_bootloader_page(AsphodelDevice_t *device, uint8_t *data, size_t data_length,
		uint16_t *block_sizes, uint8_t block_sizes_length, AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_write_bootloader_page_blocking(AsphodelDevice_t *device, uint8_t *data, size_t data_length,
		uint16_t *block_sizes, uint8_t block_sizes_length);

// Must be called after all code blocks for a specific page have been written. The MAC tag is used to verify the
// contents of the page.
ASPHODEL_API int asphodel_finish_bootloader_page(AsphodelDevice_t *device, uint8_t *mac_tag, size_t length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_finish_bootloader_page_blocking(AsphodelDevice_t *device, uint8_t *mac_tag, size_t length);

// Used to verify the contents of a page. The page contents are checked against the MAC tag to verify integrity.
// NOTE: the asphodel_start_bootloader_page() must be called for the page prior to verification.
ASPHODEL_API int asphodel_verify_bootloader_page(AsphodelDevice_t *device, uint8_t *mac_tag, size_t length,
		AsphodelCommandCallback_t callback, void * closure);
ASPHODEL_API int asphodel_verify_bootloader_page_blocking(AsphodelDevice_t *device, uint8_t *mac_tag, size_t length);


#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_BOOTLOADER_H_ */
