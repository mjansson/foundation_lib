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
    \param arr             Data array
    \param size            Size of data array in bytes
    \return                Base64 encoded string */
FOUNDATION_API char*       base64_encode( const void* arr, unsigned int size );

/*! \brief Decode base64-encoded data. Any invalid characters, linebreaks and noise will be silently discarded.
    \param data            Data string in base64 format, null terminated
    \param arr             Destination array
    \param size            Destination size of data array in bytes */
FOUNDATION_API void        base64_decode( const char* data, void* arr, unsigned int* size );
