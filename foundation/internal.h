/* internal.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file internal.h
\brief Internal types and function

Internal types and functions which are not for public use. The symbols defined in this
file may change at any time. */

#include <foundation/platform.h>
#include <foundation/types.h>

// Internal entry points

FOUNDATION_API int
internal_atomic_initialize(void);

FOUNDATION_API void
internal_atomic_finalize(void);

FOUNDATION_API int
internal_assert_initialize(void);

FOUNDATION_API void
internal_assert_finalize(void);

FOUNDATION_API void
internal_ringbuffer_stream_initialize(void);

FOUNDATION_API void
internal_buffer_stream_initialize(void);

#if FOUNDATION_PLATFORM_ANDROID
FOUNDATION_API void
internal_asset_stream_initialize(void);
#endif

FOUNDATION_API void
internal_pipe_stream_initialize(void);

FOUNDATION_API int
internal_log_initialize(void);

FOUNDATION_API void
internal_log_finalize(void);

FOUNDATION_API int
internal_memory_initialize(const memory_system_t memory);

FOUNDATION_API void
internal_memory_finalize(void);

FOUNDATION_API int
internal_time_initialize(void);

FOUNDATION_API void
internal_time_finalize(void);

FOUNDATION_API int
internal_thread_initialize(void);

FOUNDATION_API void
internal_thread_finalize(void);

FOUNDATION_API int
internal_environment_initialize(const application_t application);

FOUNDATION_API void
internal_environment_finalize(void);

FOUNDATION_API int
internal_library_initialize(void);

FOUNDATION_API void
internal_library_finalize(void);

FOUNDATION_API int
internal_system_initialize(void);

FOUNDATION_API void
internal_system_finalize(void);

FOUNDATION_API int
internal_stream_initialize(void);

FOUNDATION_API void
internal_stream_finalize(void);

FOUNDATION_API int
internal_fs_initialize(void);

FOUNDATION_API void
internal_fs_finalize(void);

FOUNDATION_API int
internal_random_initialize(void);

FOUNDATION_API void
internal_random_finalize(void);

FOUNDATION_API int
internal_static_hash_initialize(void);

FOUNDATION_API void
internal_static_hash_finalize(void);

FOUNDATION_API int
internal_stacktrace_initialize(void);

FOUNDATION_API void
internal_stacktrace_finalize(void);

FOUNDATION_API int
internal_exception_initialize(void);

FOUNDATION_API void
internal_exception_finalize(void);

// Internal functions

FOUNDATION_API uint32_t
hashtable32_raw(hashtable32_t* table, size_t slot);

FOUNDATION_API uint64_t
hashtable64_raw(hashtable64_t* table, size_t slot);

FOUNDATION_API void
environment_main_args(int argc, const char* const* argv);

FOUNDATION_API void
profile_thread_finalize(void);
