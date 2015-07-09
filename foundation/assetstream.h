/* assetstream.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file assetstream.h
\brief Stream for Android application assets
\details Stream for reading Android application assets */

#include <foundation/platform.h>
#include <foundation/types.h>

#if FOUNDATION_PLATFORM_ANDROID

/*! \brief Open an Android application asset as a stream
Open an Android application asset as a stream. Assets can only be read, not written.
\param path   Asset path within the package asset paths
\param mode   Open mode
\return       New stream, null if path not found or invalid mode */
FOUNDATION_API stream_t*
asset_stream_open(const char* path, size_t length, unsigned int mode);

#endif
