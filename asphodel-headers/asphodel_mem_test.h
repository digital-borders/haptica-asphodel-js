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

#ifndef ASPHODEL_MEM_TEST_H_
#define ASPHODEL_MEM_TEST_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// returns 1 if the library has been build with memory testing support (-DASPHODEL_MEM_TEST), and 0 otherwise.
ASPHODEL_API int asphodel_mem_test_supported(void);

// sets the malloc limit. -1 is no limit. 0 means next malloc call will return NULL.
ASPHODEL_API void asphodel_mem_test_set_limit(int limit);

// returns the current malloc limit
ASPHODEL_API int asphodel_mem_test_get_limit(void);

// INTERNAL USE ONLY!
// These are used for malloc and free, respectively, when the library is built with memory testing functionality.
ASPHODEL_LOCAL void *asphodel_mem_test_malloc(size_t size);
ASPHODEL_LOCAL void asphodel_mem_test_free(void *ptr);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_MEM_TEST_H_ */
