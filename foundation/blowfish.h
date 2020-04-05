/* blowfish.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file blowfish.h
\brief Blowfish encryption and decryption

Blowfish encryption and decryption. Blowfish is a symmetric block cipher with variable length key.
For more information, see https://www.schneier.com/blowfish.html

The blowfish state is not inherently thread safe, synchronization in a multithread use case must
be done by caller. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate a blowfish state object. Does NOT initialize the object, must
be done with a call to #blowfish_initialize before using the object for
encryption/descryption.
\return  New blowfish state object */
FOUNDATION_API blowfish_t*
blowfish_allocate(void);

/*! Deallocate a blowfish state object and free resources
\param blowfish Blowfish state object to deallocate */
FOUNDATION_API void
blowfish_deallocate(blowfish_t* blowfish);

/*! Initialize the blowfish state object with the given key data. After
a call to this function the state object can be used for encryption/decryption.
\param blowfish Blowfish state object
\param key      Key data
\param length   Length of key data in bytes */
FOUNDATION_API void
blowfish_initialize(blowfish_t* blowfish, const void* key, size_t length);

/*! Finalize a blowfish state object and free resources
\param blowfish Blowfish state object to finalize */
FOUNDATION_API void
blowfish_finalize(blowfish_t* blowfish);

/*! Encrypt data using the given blowfish state object. Encryption is done in-place,
no memory allocation is done internally. Length is expected to be a multiple of 8
bytes (any extra unaligned data will be ignored).
\param blowfish Blowfish state object
\param data     Data buffer
\param length   Length of data buffer in bytes
\param mode     Mode of operation (see #blockcipher_mode_t)
\param vec      Initialization vector */
FOUNDATION_API void
blowfish_encrypt(const blowfish_t* blowfish, void* data, size_t length, blockcipher_mode_t mode, uint64_t vec);

/*! Decrypt data using the given blowfish state object. Decryption is done in-place,
no memory allocation is done internally. Length is expected to be a multiple of 8
bytes (any extra unaligned data will be ignored).
\param blowfish Blowfish state object
\param data     Data buffer
\param length   Length of data buffer in bytes
\param mode     Mode of operation (see #blockcipher_mode_t)
\param vec      Initialization vector */
FOUNDATION_API void
blowfish_decrypt(const blowfish_t* blowfish, void* data, size_t length, blockcipher_mode_t mode, uint64_t vec);
