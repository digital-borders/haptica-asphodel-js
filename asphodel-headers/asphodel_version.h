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

#ifndef ASPHODEL_VERSION_H_
#define ASPHODEL_VERSION_H_

#include <stdint.h>
#include "asphodel_api.h"


#ifdef __cplusplus
extern "C" {
#endif

// Return the protocol version supported by this device in BCD form (e.g. 0x0200).
ASPHODEL_API uint16_t asphodel_get_library_protocol_version(void);

// Return the protocol version supported by this device in string form (e.g. "2.0.0").
ASPHODEL_API const char * asphodel_get_library_protocol_version_string(void);

// Return the library's build info string.
ASPHODEL_API const char * asphodel_get_library_build_info(void);

// Return the library's build date as a string.
ASPHODEL_API const char * asphodel_get_library_build_date(void);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_VERSION_H_ */
