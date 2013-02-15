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
    MD5 message-digest algorithm. Inspired by Alexander Peslyak's public domain implementation
    available at http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5 */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Allocate md5 block
    \return                  New md5 block */
FOUNDATION_API md5_t*        md5_allocate( void );

/*! Deallocate md5 block
    \param digest            md5 block */
FOUNDATION_API void          md5_deallocate( md5_t* digest );

/*! Initialize md5 block
    \param digest            md5 block */
FOUNDATION_API void          md5_initialize( md5_t* digest );

/*! Digest a string
    \param digest            md5 block
    \param message           Message to digest
    \return                  md5 block */
FOUNDATION_API md5_t*        md5_digest( md5_t* digest, const char* message );

/*! Digest a raw data buffer
    \param digest            md5 block
    \param buffer            Data to digest
    \param size              Size of buffer
    \return                  md5 block */
FOUNDATION_API md5_t*        md5_digest_raw( md5_t* digest, const void* buffer, size_t size );

/*! Finalize digest
    \param digest            md5 block */
FOUNDATION_API void          md5_finalize( md5_t* digest );

/*! Get digest as string
    \param digest            md5 block
    \return                  Message digest string */
FOUNDATION_API char*         md5_get_digest( const md5_t* digest );

/*! Get digest as raw 128-bit value
    \param digest            md5 block
    \return                  Message digest */
FOUNDATION_API uint128_t     md5_get_digest_raw( const md5_t* digest );
