/* foundation.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file foundation.h
\brief Foundation library main header and entry points

Wrapper for foundation library headers and main entry/exit points */

#include <foundation/platform.h>

#include <foundation/build.h>
#include <foundation/types.h>
#include <foundation/bits.h>
#include <foundation/assert.h>
#include <foundation/memory.h>
#include <foundation/atomic.h>
#include <foundation/error.h>
#include <foundation/thread.h>
#include <foundation/mutex.h>
#include <foundation/semaphore.h>
#include <foundation/beacon.h>
#include <foundation/library.h>
#include <foundation/system.h>
#include <foundation/process.h>
#include <foundation/uuid.h>
#include <foundation/log.h>
#include <foundation/version.h>

#include <foundation/hash.h>
#include <foundation/hashstrings.h>
#include <foundation/base64.h>
#include <foundation/md5.h>
#include <foundation/array.h>
#include <foundation/bitbuffer.h>
#include <foundation/bucketarray.h>
#include <foundation/virtualarray.h>
#include <foundation/hashmap.h>
#include <foundation/uuidmap.h>
#include <foundation/hashtable.h>
#include <foundation/ringbuffer.h>
#include <foundation/string.h>
#include <foundation/path.h>
#include <foundation/locale.h>

#include <foundation/math.h>
#include <foundation/random.h>
#include <foundation/radixsort.h>

#include <foundation/objectmap.h>
#include <foundation/event.h>
#include <foundation/time.h>
#include <foundation/profile.h>

#include <foundation/environment.h>
#include <foundation/stream.h>
#include <foundation/fs.h>
#include <foundation/bufferstream.h>
#include <foundation/assetstream.h>
#include <foundation/pipe.h>
#include <foundation/json.h>

#include <foundation/exception.h>
#include <foundation/stacktrace.h>

#include <foundation/blowfish.h>
#include <foundation/regex.h>
#include <foundation/sha.h>

#define FOUNDATION_NO_INTERFACE
#include <foundation/delegate.h>
#include <foundation/main.h>
#undef FOUNDATION_NO_INTERFACE

/*! Main entry point. Call this in your #main_initialize function to bootstrap the foundation
library and initialize all functionality.
\param memory       Memory system declaration (use memory_system_malloc for
                    default system allocator)
\param application  Application declaration
\param config       Application configuration
\return             0 if initialization successful, <0 if error */
FOUNDATION_API int
foundation_initialize(const memory_system_t memory, const application_t application, const foundation_config_t config);

/*! Main exit point. Call this in your #main_finalize function to cleanup the foundation library
and terminate all functionality. */
FOUNDATION_API void
foundation_finalize(void);

/*! Query if foundation library is initialized properly
\return true if initialized, false if not */
FOUNDATION_API bool
foundation_is_initialized(void);

/*! Query foundation config
\return Foundation library config */
FOUNDATION_API foundation_config_t
foundation_config(void);

/*! Query foundation library build version
\return Version of foundation library */
FOUNDATION_API version_t
foundation_version(void);
