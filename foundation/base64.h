/* base64.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file base64.h
    Base64 encoding and decoding */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! \brief Encode in base64
    \param src                   Source data
    \param dst                   Destination string
    \param srcsize               Size of source data in bytes
    \param dstsize               Size of destination string in bytes (assumed infinite if 0)
	\return                      Number of bytes written to destination string including terminting zero */
FOUNDATION_API unsigned int      base64_encode( const void* src, char* dst, unsigned int srcsize, unsigned int dstsize );

/*! \brief Decode base64-encoded data. Any invalid characters, linebreaks and noise will be silently discarded.
    \param src                   Source string
    \param dst                   Destination array
    \param srcsize               Size of source data in bytes (run until zero termination if 0)
    \param dstsize               Destination size of data array in bytes (assumed infinite if 0)
	\return                      Number of bytes written to destination array */
FOUNDATION_API unsigned int      base64_decode( const char* src, void* dst, unsigned int srcsize, unsigned int dstsize );
