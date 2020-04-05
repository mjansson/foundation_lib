/* md5.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file md5.h
\brief MD5 algorithm

MD5 message-digest algorithm. Inspired by Alexander Peslyak's public domain implementation
available at http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5

Normal use case is to first allocate/initialize the md5 block, then do any number of
initialize-digest-finalize call sequences:

<pre>md5_initialize()
md5_digest()
md5_digest()
... //More digest operations
md5_digest_finalize()
md5_get_digest()
md5_get_digest_raw()
... //More initialize, digest sequences
md5_finalize()</pre> */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate a new MD5 block and initialize for digestion.
\return New MD5 block */
FOUNDATION_API md5_t*
md5_allocate(void);

/*! Deallocate MD5 block
\param digest MD5 block */
FOUNDATION_API void
md5_deallocate(md5_t* digest);

/*! Initialize MD5 block. Must be called before each block of digest operations
with #md5_digest
\param digest MD5 block */
FOUNDATION_API void
md5_initialize(md5_t* digest);

/*! Finalize MD5 block previously initialized with #md5_initialize. After this call the
block may no longer be used until a new #md5_initialize call is made.
\param digest MD5 block */
FOUNDATION_API void
md5_finalize(md5_t* digest);

/*! Digest a raw data buffer.
\param digest MD5 block
\param buffer Data to digest
\param size Size of buffer
\return MD5 block */
FOUNDATION_API md5_t*
md5_digest(md5_t* digest, const void* buffer, size_t size);

/*! Finalize digest. Must be called between digesting data with #md5_digest
and getting the final message digest with #md5_get_digest/#md5_get_digest_raw. If a new
digest sequence is required the block must be re-initialized with a call to #md5_initialize.
\param digest MD5 block */
FOUNDATION_API void
md5_digest_finalize(md5_t* digest);

/*! Get digest as string. Before getting the digest string the MD5 block must be
finalized with a call to #md5_digest_finalize.
\param digest MD5 block
\param buffer String buffer
\param capacity Capacity of string buffer
\return Message digest string */
FOUNDATION_API string_t
md5_get_digest(const md5_t* digest, char* buffer, size_t capacity);

/*! Get digest as raw 128-bit value. Before getting the raw digest the MD5 block must be
finalized with a call to #md5_digest_finalize.
\param digest MD5 block
\return Message digest */
FOUNDATION_API uint128_t
md5_get_digest_raw(const md5_t* digest);
