/* assetstream.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file assetstream.h
\brief Stream for Android application assets

Stream for reading Android application assets, only available on Android platform.
Asset streams are read only streams stored in the application package. For more information
check the Android developer portal at
http://developer.android.com/intl/ru/tools/projects/index.html#ApplicationModules

Streams are not inherently thread safe, synchronization in a multithread use case must be done
by caller. */

#include <foundation/platform.h>
#include <foundation/types.h>

#if FOUNDATION_PLATFORM_ANDROID

/*! Open an Android application asset as a stream. Assets are read only, no writing
or creation of new streams is possible.
\param path   Asset path within the package asset paths
\param length Length of asset path
\param mode   Open mode
\return       New stream, null if path not found or invalid mode */
FOUNDATION_API stream_t*
asset_stream_open(const char* path, size_t length, unsigned int mode);

#endif
