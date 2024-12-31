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

#ifndef ASPHODEL_USB_H_
#define ASPHODEL_USB_H_

#include <stdint.h>
#include <stddef.h>
#include "asphodel_api.h"
#include "asphodel_device.h"

#ifdef __cplusplus
extern "C" {
#endif

// returns 1 if the library has been build with USB device support (-DASPHODEL_USB_DEVICE), and 0 otherwise.
ASPHODEL_API int asphodel_usb_devices_supported(void);

// initialize the USB portion of the library. MUST be called before asphodel_usb_find_devices()
ASPHODEL_API int asphodel_usb_init(void);

// close down the USB portion of the library. asphodel_usb_init() can be called afterwards, if needed later.
ASPHODEL_API void asphodel_usb_deinit(void);

// Find any Asphodel devices attached to the system via USB. The device_list parameter should be an array of
// AsphodelDevice_t pointers, with the array size pointed to by list_size. The array will be filled with pointers
// to AsphodelDevice_t structs, up to the array length. The total number of found USB devices will be written into
// the address pointed to by list_size. ALL returned devices must be freed (either immediately or at a later point)
// by the calling code by calling the AsphodelDevice_t free_device function pointer.
ASPHODEL_API int asphodel_usb_find_devices(AsphodelDevice_t **device_list, size_t *list_size);

// Poll all USB devices at once. Implementation note: each USB device's poll_device() will poll all devices. This
// function is provided for code readability when polling multiple devices.
ASPHODEL_API int asphodel_usb_poll_devices(int milliseconds);

// Return the version string for the running version of the usb backend (libusb-1.0 in current implementations)
ASPHODEL_API const char * asphodel_usb_get_backend_version(void);

#ifdef __cplusplus
}
#endif


#endif /* ASPHODEL_USB_H_ */
