/* md5.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file md5.h
\brief MD5 algorithm
\details MD5 message-digest algorithm. Inspired by Alexander Peslyak's public domain implementation
available at http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5

Normal use case is to first allocate the md5 block, then do any number of initialize-digest-finalize
call sequences:

<pre>md5_initialize()
md5_digest()
md5_digest_raw()
...
md5_finalize()
md5_get_digest()
md5_get_digest_raw()</pre> */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! \brief Allocate block
Allocate a new MD5 block and initialize for digestion.
\return New MD5 block */
FOUNDATION_API md5_t*
md5_allocate(void);

/*!\brief Deallocate block
Deallocate MD5 block
\param digest MD5 block */
FOUNDATION_API void
md5_deallocate(md5_t* digest);

/*! \brief Initialize block
Initialize MD5 block. Must be called before each block of digest operations
with #md5_digest and #md5_digest_raw
\param digest MD5 block */
FOUNDATION_API void
md5_initialize(md5_t* digest);

/*! \brief Finalize block
Finalize MD5 block previously initialized with #md5_initialize
\param digest MD5 block */
FOUNDATION_API void
md5_finalize(md5_t* digest);

/*! \brief Digest data buffer
Digest a raw data buffer.
\param digest MD5 block
\param buffer Data to digest
\param size Size of buffer
\return MD5 block */
FOUNDATION_API md5_t*
md5_digest(md5_t* digest, const void* buffer, size_t size);

/*! \brief Finalize digest
Finalize digest. Must be called between digesting data with #md5_digest/#md5_digest_raw and getting the
final message digest with #md5_get_digest/#md5_get_digest_raw
\param digest MD5 block */
FOUNDATION_API void
md5_digest_finalize(md5_t* digest);

/*! \brief Get digest string
Get digest as string. Before getting the digest string the MD5 block must be
finalized with a call to #md5_digest_finalize.
\param digest MD5 block
\param str String buffer
\param length Length of string buffer
\return Message digest string */
FOUNDATION_API string_t
md5_get_digest(const md5_t* digest, char* str, size_t length);

/*! \brief Get raw digest
Get digest as raw 128-bit value. Before getting the raw digest the MD5 block must be
finalized with a call to #md5_digest_finalize.
\param digest MD5 block
\return Message digest */
FOUNDATION_API uint128_t
md5_get_digest_raw(const md5_t* digest);
