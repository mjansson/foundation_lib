/* base64.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <foundation/foundation.h>

/*lint -e{840}  We use null character in string literal deliberately here*/
static const char base64_decode_table[] =
    "|\0\0\0}rstuvwxyz{\0\0\0\0\0\0\0>?@ABCDEFGHIJKLMNOPQRSTUVW\0\0\0\0\0\0XYZ[\\]^_`"
    "abcdefghijklmnopq";
static const char base64_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t
base64_encode(const void* source, size_t size, char* destination, size_t capacity) {
	char* ptr;
	const unsigned char* carr;
	unsigned char bits;

	if (capacity > 0) {
		size_t maxsize = ((capacity - 1) / 4) * 3;
		if (maxsize < size)
			size = maxsize;
	} else {
		return 0;
	}

	carr = (const unsigned char*)source;
	ptr = destination;
	while (size > 2) {
		bits = (*carr >> 2) & 0x3F;
		*ptr++ = base64_encode_table[bits];
		bits = (unsigned char)((*carr & 0x3) << 4) | ((*(carr + 1) >> 4) & 0xF);
		*ptr++ = base64_encode_table[bits];
		bits = (unsigned char)((*(carr + 1) & 0xF) << 2) | ((*(carr + 2) >> 6) & 0x3);
		*ptr++ = base64_encode_table[bits];
		bits = *(carr + 2) & 0x3F;
		*ptr++ = base64_encode_table[bits];
		size -= 3;
		carr += 3;
	}
	if (size == 2) {
		bits = (*carr >> 2) & 0x3F;
		*ptr++ = base64_encode_table[bits];
		bits = (unsigned char)((*carr & 0x3) << 4) | ((*(carr + 1) >> 4) & 0xF);
		*ptr++ = base64_encode_table[bits];
		bits = (unsigned char)((*(carr + 1) & 0xF) << 2);
		*ptr++ = base64_encode_table[bits];
		*ptr++ = '=';
	} else if (size == 1) {
		bits = (*carr >> 2) & 0x3F;
		*ptr++ = base64_encode_table[bits];
		bits = (unsigned char)((*carr & 0x3) << 4);
		*ptr++ = base64_encode_table[bits];
		*ptr++ = '=';
		*ptr++ = '=';
	}

	*ptr++ = 0;

	return (size_t)pointer_diff(ptr, destination);
}

size_t
base64_decode(const char* source, size_t size, void* destination, size_t capacity) {
	size_t i, blocksize;
	char* cdst = (char*)destination;
	char* cdstend = cdst + capacity;
	while (size && (cdst < cdstend)) {
		unsigned char in[4] = {0, 0, 0, 0};  // Always build blocks of 4 bytes to decode, pad with 0
		blocksize = 0;
		for (i = 0; size && (i < 4); i++) {
			char v = 0;
			while (size && !v) {  // Consume one valid byte from input, discarding invalid data
				v = *source++;
				v = ((v < 43 || v > 122) ? 0 : base64_decode_table[v - 43]);
				if (v) {
					in[i] = (unsigned char)(v - 62);
					blocksize++;
				}
				--size;
			}
		}
		if (blocksize > 1) {
			char out[3];
			out[0] = (char)((in[0] << 2) | (in[1] >> 4));
			out[1] = (char)((in[1] << 4) | (in[2] >> 2));
			out[2] = (char)(((in[2] << 6) & 0xc0) | in[3]);
			for (i = 0; (i < blocksize - 1) && (cdst < cdstend); ++i)
				*cdst++ = out[i];
		}
	}

	return (size_t)pointer_diff(cdst, destination);
}
