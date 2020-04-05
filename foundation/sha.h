/* sha.h  -  Foundation library  -  Public Domain  -  2016 Mattias Jansson
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

/*! \file sha.h
\brief SHA-2 256/512 bit algorithms

SHA-2 secure hash algorithm. Inspired by Daniel Andersson's public domain implementation
available at https://github.com/kalven/sha-2

Normal use case is to first allocate/initialize the sha block, then do any number of
initialize-digest-finalize call sequences:

<pre>sha512_initialize()
sha512_digest()
sha512_digest()
... //More digest operations
sha512_digest_finalize()
sha512_get_digest()
sha512_get_digest_raw()
... //More initialize, digest sequences
sha512_finalize()</pre> */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate a new SHA-256 block and initialize for digestion.
\return New SHA-256 block */
FOUNDATION_API sha256_t*
sha256_allocate(void);

/*! Deallocate SHA-256 block
\param digest SHA-256 block */
FOUNDATION_API void
sha256_deallocate(sha256_t* digest);

/*! Initialize SHA-256 block. Must be called before each block of digest operations
with #sha256_digest
\param digest SHA-256 block */
FOUNDATION_API void
sha256_initialize(sha256_t* digest);

/*! Finalize SHA-256 block previously initialized with #sha256_initialize. After this call the
block may no longer be used until a new #sha256_initialize call is made.
\param digest SHA-256 block */
FOUNDATION_API void
sha256_finalize(sha256_t* digest);

/*! Digest a raw data buffer.
\param digest SHA-256 block
\param buffer Data to digest
\param size Size of buffer
\return SHA-256 block */
FOUNDATION_API sha256_t*
sha256_digest(sha256_t* digest, const void* buffer, size_t size);

/*! Finalize digest. Must be called between digesting data with #sha256_digest
and getting the final message digest with #sha256_get_digest/#sha256_get_digest_raw. If a new
digest sequence is required the block must be re-initialized with a call to #sha256_initialize.
\param digest SHA-256 block */
FOUNDATION_API void
sha256_digest_finalize(sha256_t* digest);

/*! Get digest as string. Before getting the digest string the SHA-256 block must be
finalized with a call to #sha256_digest_finalize.
\param digest SHA-256 block
\param str String buffer
\param length Length of string buffer
\return Message digest string */
FOUNDATION_API string_t
sha256_get_digest(const sha256_t* digest, char* str, size_t length);

/*! Get digest as raw 256-bit value. Before getting the raw digest the SHA-256 block must be
finalized with a call to #sha256_digest_finalize.
\param digest SHA-256 block
\return Message digest */
FOUNDATION_API uint256_t
sha256_get_digest_raw(const sha256_t* digest);

/*! Allocate a new SHA-512 block and initialize for digestion.
\return New SHA-512 block */
FOUNDATION_API sha512_t*
sha512_allocate(void);

/*! Deallocate SHA-512 block
\param digest SHA-512 block */
FOUNDATION_API void
sha512_deallocate(sha512_t* digest);

/*! Initialize SHA-512 block. Must be called before each block of digest operations
with #sha512_digest
\param digest SHA-512 block */
FOUNDATION_API void
sha512_initialize(sha512_t* digest);

/*! Finalize SHA-512 block previously initialized with #sha512_initialize. After this call the
block may no longer be used until a new #sha512_initialize call is made.
\param digest SHA-512 block */
FOUNDATION_API void
sha512_finalize(sha512_t* digest);

/*! Digest a raw data buffer.
\param digest SHA-512 block
\param buffer Data to digest
\param size Size of buffer
\return SHA-512 block */
FOUNDATION_API sha512_t*
sha512_digest(sha512_t* digest, const void* buffer, size_t size);

/*! Finalize digest. Must be called between digesting data with #sha512_digest
and getting the final message digest with #sha512_get_digest/#sha512_get_digest_raw. If a new
digest sequence is required the block must be re-initialized with a call to #sha512_initialize.
\param digest SHA-512 block */
FOUNDATION_API void
sha512_digest_finalize(sha512_t* digest);

/*! Get digest as string. Before getting the digest string the SHA-512 block must be
finalized with a call to #sha512_digest_finalize.
\param digest SHA-512 block
\param buffer String buffer
\param capacity Capacity of string buffer
\return Message digest string */
FOUNDATION_API string_t
sha512_get_digest(const sha512_t* digest, char* buffer, size_t capacity);

/*! Get digest as raw 512-bit value. Before getting the raw digest the SHA-512 block must be
finalized with a call to #sha512_digest_finalize.
\param digest SHA-512 block
\return Message digest */
FOUNDATION_API uint512_t
sha512_get_digest_raw(const sha512_t* digest);
