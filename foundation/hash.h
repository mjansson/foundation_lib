/* hash.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file hash.h
\brief Murmur3 hash

Murmur3 hash from http://code.google.com/p/smhasher/

Wrapper macros around predefined static hashed strings. See hashify utility for
creating static hashes */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/assert.h>

#if BUILD_ENABLE_STATIC_HASH_DEBUG
#include <foundation/string.h>
#endif

/*! Hash data memory blob. Pointer must be aligned to 8 bytes
\param key Key to hash
\param len Length of key in bytes
\return    Hash of key */
FOUNDATION_API FOUNDATION_PURECALL hash_t
hash(const void* key, size_t len);

/*! Reverse hash lookup. Only available if #BUILD_ENABLE_STATIC_HASH_DEBUG is
enabled, otherwise if will always return an empty string
\param value Hash value
\return      String matching hash value, or empty string if not found */
FOUNDATION_API string_const_t
hash_to_string(hash_t value);

#if BUILD_ENABLE_STATIC_HASH_DEBUG

static FOUNDATION_FORCEINLINE hash_t
static_hash(const void* key, size_t len, hash_t value);

#else

#define static_hash(key, len, value) ((void)sizeof(key), (void)sizeof(len), (hash_t)(value))

#define hash_to_string(value) ((void)sizeof(value), (const char*)0)

#endif

/*! Declare a statically hashed string. If #BUILD_ENABLE_STATIC_HASH_DEBUG is enabled
in the build config this will allow the string to be reverse looked up with hash_to_string.
Static hash strings are usually defined by using the hashify tool on a declaration file,
see the hashstrings.txt and corresponding hashstrings.h header
\param key    Key string
\param len    Length of key in bytes
\param value  Hash value */
#define static_hash_string(key, len, value) static_hash(key, len, (hash_t)value)

#if BUILD_ENABLE_STATIC_HASH_DEBUG

FOUNDATION_API void
static_hash_store(const void* key, size_t len, hash_t value);

static FOUNDATION_FORCEINLINE hash_t
static_hash(const void* key, size_t len, hash_t value) {
	hash_t ref = hash(key, len);
	FOUNDATION_ASSERT_MSGFORMAT(!value || (ref == value), "Static hash fail: %s -> 0x%" PRIx64 ", expected 0x%" PRIx64,
	                            (const char*)key, ref, value);
	static_hash_store(key, len, ref);
	return ref;
}

#endif

/*! Hash of an empty/null string (length 0) */
#define HASH_EMPTY_STRING 0xC2D00F032E25E509ULL

/*! Null hash value */
#define HASH_NULL 0ULL
