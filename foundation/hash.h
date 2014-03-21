/* hash.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 * 
 * https://github.com/rampantpixels/foundation_lib
 * 
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#pragma once

/*! \file hash.h
    Murmur3 hash from http://code.google.com/p/smhasher/
	Also wrapper macros around predefined static hashed strings
	See hashify utility for creating static hashes */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/assert.h>


/*! Hash memory blob. Pointer must be aligned to 8 bytes
    \param key                               Key to hash
    \param len                               Length of key in bytes
    \return                                  Hash of key */
FOUNDATION_API PURECALL hash_t               hash( const void* key, const unsigned int len );

/*! Reverse hash lookup. Only available if BUILD_ENABLE_STATIC_HASH_DEBUG is
    enabled, otherwise if will always return an empty string
    \param value                             Hash value
    \return                                  String matching hash value, or empty string if not found */
FOUNDATION_API const char*                   hash_to_string( const hash_t value );

/*! \def static_hash_string
    Declare a statically hashed string. If BUILD_ENABLE_STATIC_HASH_DEBUG is enabled
    in the build config this will allow the string to be reverse looked up with hash_to_string.
    Static hash strings are usually defined by using the hashify tool on a declaration file,
    see the hashstrings.txt and corresponding hashstrings.h header */

#if BUILD_ENABLE_STATIC_HASH_DEBUG
static FORCEINLINE hash_t                    static_hash( const void* key, const unsigned int len, const hash_t value );
#  define static_hash_string( key, value )   static_hash( key, string_length( key ), value )
#else
#  define static_hash( key, len, value )     ( (void)sizeof( key ), (void)sizeof( len ), (value) )
#  define static_hash_string( key, value )   ( (void)sizeof( key ), (value) )
#  define hash_to_string( str )              ( (void)sizeof( str ), (const char*)0 )
#endif


#if BUILD_ENABLE_STATIC_HASH_DEBUG

FOUNDATION_API void                          _static_hash_store( const void* key, const unsigned int len, const hash_t value );

static FORCEINLINE hash_t                    static_hash( const void* key, const unsigned int len, const hash_t value )
{
	hash_t ref = hash( key, len );
	FOUNDATION_ASSERT_MSGFORMAT( !value || ( ref == value ), "Static hash fail: %s -> 0x%llx, expected 0x%llx", key, ref, value );
	_static_hash_store( key, len, ref );
	return ref;
}

#endif

/*! \def HASH_EMPTY_STRING
    Hash of an empty string (length 0) */
#define HASH_EMPTY_STRING                    0xC2D00F032E25E509ULL
