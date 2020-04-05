/* profile.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file profile.h
\brief Profiling

Profiling, collecting function level profiling data. Available if #BUILD_ENABLE_PROFILE is set,
otherwise all profiling function calls will evaluate to void and not generate any code. */

#include <foundation/platform.h>
#include <foundation/types.h>

#if BUILD_ENABLE_PROFILE

/*! Initialize profiling module.

The string passed to the function must be constant over the execution of the application
(until #profile_finalize is called) and should identify the application/build/machine or
whatever you want it to identify.

Memory buffer should be large enough to hold data for ~100ms to avoid excessive calls to
output flush function. The profile subsystem will not allocate any memory, it only uses
the passed in work buffer. Recommended size is at least 256KiB. Maximum usable size is
4MiB, since the profiling system can only use 65k blocks of 64 bytes each.
\param identifier Application identifier
\param length Length of identifier
\param buffer Work temporary buffer
\param capacity Size of work buffer in number of bytes */
FOUNDATION_API void
profile_initialize(const char* identifier, size_t length, void* buffer, size_t capacity);

/*! Finalize profiling module and free resources. Will call the writer callback with a null
pointer and zero size to indicate end of transmission. */
FOUNDATION_API void
profile_finalize(void);

/*! Control profiling operation. A true value enables all profiling calls, a false value
disables all calls.
\param enable Enable flag */
FOUNDATION_API void
profile_enable(bool enable);

/*! Set output function. The function will be called with blocks of data to write to whatever
output stream you want.
\param writer New writer function */
FOUNDATION_API void
profile_set_output(profile_write_fn writer);

/*! Control profile output rate by setting time between flushes in milliseconds. Default is
100ms. Decresee time (increase rate) when passing a smaller buffer to initialization, or
increase time (decrease rate) if passing a larger buffer.
\param ms Wait time in milliseconds */
FOUNDATION_API void
profile_set_output_wait(unsigned int ms);

/*! End a frame. Inserts a token into the profiling stream that identifies the end of a frame,
effectively grouping profile information together in a block.
\param counter Frame counter */
FOUNDATION_API void
profile_end_frame(uint64_t counter);

/*! Begin a named profile timing block. The string passed to this function must be constant
until the block is written to the output stream. Every call to #profile_begin_block must
be matched with a call to #profile_end_block
\param message Block message (name)
\param length Length of message */
FOUNDATION_API void
profile_begin_block(const char* message, size_t length);

/*! Update the current active block. Call this function regularly for blocks
running for a prolonged time. */
FOUNDATION_API void
profile_update_block(void);

/*! End the current active block */
FOUNDATION_API void
profile_end_block(void);

/*! Insert log message. The string passed to this function must be constant until the block
is written to the output stream.
\param message Message to insert into the profile stream
\param length Length of message */
FOUNDATION_API void
profile_log(const char* message, size_t length);

/*! Lock notification. Call this method right before the thread tries to acquire a lock
on a mutually exclusive resource. The string passed to this function must be constant
until the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_trylock(const char* name, size_t length);

/*! Lock notification. Call this method right after the thread has acquired a lock on a
mutually exclusive resource. The string passed to this function must be constant until
the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_lock(const char* name, size_t length);

/*! Lock notification. Call this method right after the thread has released a lock on a
mutually exclusive resource. The string passed to this function must be constant until
the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_unlock(const char* name, size_t length);

/*! Wait notification. Call this method right before the thread enters a wait state on a
mutually exclusive resource. The string passed to this function must be constant until
the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_wait(const char* name, size_t length);

/*! Signal notification. Call this method right before the thread signals state on a
mutually exclusive resource. The string passed to this function must be constant until
the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_signal(const char* name, size_t length);

/*! Get profile identifier
\return Identifier given to profile_initialize */
FOUNDATION_API string_const_t
profile_identifier(void);

#else

#define profile_initialize(...)                 \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_finalize() \
	do {                   \
	} while (0)
#define profile_enable(...)                     \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_set_output(fn)       \
	do {                             \
		profile_write_fn tmpfn = fn; \
		FOUNDATION_UNUSED(tmpfn);    \
	} while (0)
#define profile_set_output_wait(...)            \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_end_frame(...)                  \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_begin_block_(...)               \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_begin_block(...) profile_begin_block_(__VA_ARGS__)
#define profile_update_block(...) \
	do {                          \
	} while (0)
#define profile_end_block(...) \
	do {                       \
	} while (0)

#define profile_log_(...)                       \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_log(...) profile_log_(__VA_ARGS__)
#define profile_trylock_(...)                   \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_trylock(...) profile_trylock_(__VA_ARGS__)
#define profile_lock_(...)                      \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_lock(...) profile_lock_(__VA_ARGS__)
#define profile_unlock_(...)                    \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_unlock(...) profile_unlock_(__VA_ARGS__)
#define profile_wait_(...)                      \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_wait(...) profile_wait_(__VA_ARGS__)
#define profile_signal_(...)                    \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define profile_signal(...) profile_signal_(__VA_ARGS__)
#define profile_identifier() string_null()

#endif
