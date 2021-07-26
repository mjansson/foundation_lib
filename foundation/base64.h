/* base64.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#pragma once

/*! \file base64.h
\brief Base64 encoding and decoding

Base64 encoding and decoding, using [A-Z][a-z][0-9][+/] as encoding characters. For more
information, see https://en.wikipedia.org/wiki/Base64 */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Encode data in base64. Source and destination buffers must NOT be equal, encoding cannot
be in-place as encoded base64 data occupies more memory space than input data (3 input bytes
generate 4 output bytes).

To encode entire source buffer, the destination buffer must be at least
1+(ceil(size/3)*4) bytes, including space for the terminating zero character. If the
destination buffer is not large enough, the function will encode as much of source data that
fits into the destination buffer, including a terminating zero (i.e ((capacity-1)/4)*3 bytes
of source data).

The destination buffer string will always be zero terminated.

\param source      Source data buffer
\param size        Size of source data buffer in bytes
\param destination Destination string buffer
\param capacity    Capacity of destination string buffer in bytes
\return            Number of bytes written to destination string including terminating zero */
FOUNDATION_API size_t
base64_encode(const void* source, size_t size, char* destination, size_t capacity);

/*! Decode base64-encoded data. Any invalid characters, linebreaks and noise will be
silently discarded. Source and destination buffers can be equal, since decoded data takes
less space than source encoded data (4 input bytes generate 3 output bytes).

To decode the entire source buffer, the destination buffer must be at least
((size-1)/4)*3 bytes. If the destination buffer is not large enough, the function will
decode as much of the source data that fits into the destination buffer (i.e
1+(ceil(capacity/3)*4) bytes of source data).

\param source      Source string buffer
\param size        Size of source string buffer in bytes (NOT including zero terminator)
\param destination Destination buffer
\param capacity    Capacity of destination buffer in bytes
\return            Number of bytes written to destination buffer */
FOUNDATION_API size_t
base64_decode(const char* source, size_t size, void* destination, size_t capacity);
