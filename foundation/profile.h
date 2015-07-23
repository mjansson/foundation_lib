/* profile.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file profile.h
\brief Profiling API
\details Profiling API collecting function level profiling data. Available
if #BUILD_ENABLE_PROFILE is set, otherwise all profiling function calls will
evaluate to void and not generate any code. */

#include <foundation/platform.h>
#include <foundation/types.h>

#if BUILD_ENABLE_PROFILE

/*! \brief Initialize profiling
Initialize profiling module. The string passed to the function must be constant over the
execution of the application (until profile_shutdown() is called) and should identify the
application/build/machine or whatever you want it to identify. Memory buffer should be large
enough to hold data for ~100ms to avoid excessive calls to output flush function. The profile
subsystem will not allocate any memory, it only uses the passed in work buffer. Recommended
size is at least 256KiB. Maximum usable size is 4MiB, since the profiling system can only use
65k blocks of 64 bytes each.
\param identifier Application identifier
\param length Length of identifier
\param buffer Work temporary buffer
\param size Size of work buffer */
FOUNDATION_API void
profile_initialize(const char* identifier, size_t length, void* buffer, size_t size);

/*! \brief Finalize profiling
Finalize profiling module and free resources. Will call the writer callback with a null
pointer and zero size to indicate end of transmission. */
FOUNDATION_API void
profile_finalize(void);

/*! \brief Control profiling
Control profiling operation. A true value enables all profiling calls, a false value disables
all calls.
\param enable Enable flag */
FOUNDATION_API void
profile_enable(bool enable);

/*! \brief Set output function
Set output function. The function will be called with blocks of data to write to whatever
output stream you want.
\param writer New writer function */
FOUNDATION_API void
profile_set_output(profile_write_fn writer);

/*! \brief Control output rate
Control profile output rate by setting time between flushes in milliseconds. Default is
100ms. Increase rate when passing a smaller buffer to initialization, decrease rate if
passing a larger buffer.
\param ms Wait time in milliseconds */
FOUNDATION_API void
profile_set_output_wait(unsigned int ms);

/*! \brief End frame
End a frame. Inserts a token into the profiling stream that identifies the end of a frame,
effectively grouping profile information together in a block.
\param counter Frame counter */
FOUNDATION_API void
profile_end_frame(uint64_t counter);

/*! \brief Begin block
Begin a named profile timing block. The string passed to this function must be constant
until the block is written to the output stream. Every call to #profile_begin_block must
be matched with a call to #profile_end_block
\param message Block message (name)
\param length Length of message */
FOUNDATION_API void
profile_begin_block(const char* message, size_t length);

/*! \brief Update block
Update the current active block. Call this function regularly for blocks
running for a prolonged time. */
FOUNDATION_API void
profile_update_block(void);

/*! \brief End block
End the current active block */
FOUNDATION_API void
profile_end_block(void);

/*! \brief Insert log message
Insert log message. The string passed to this function must be constant until the block
is written to the output stream.
\param message Message to insert into the profile stream
\param length Length of message */
FOUNDATION_API void
profile_log(const char* message, size_t length);

/*! \brief Lock notification
Lock notification. Call this method right before the thread tries to acquire a lock
on a mutually exclusive resource. The string passed to this function must be constant
until the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_trylock(const char* name, size_t length);

/*! \brief Lock notification
Lock notification. Call this method right after the thread has acquired a lock on a
mutually exclusive resource. The string passed to this function must be constant until
the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_lock(const char* name, size_t length);

/*! \brief Lock notification
Lock notification. Call this method right after the thread has released a lock on a
mutually exclusive resource. The string passed to this function must be constant until
the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_unlock(const char* name, size_t length);

/*! \brief Wait notification
Wait notification. Call this method right before the thread enters a wait state on a
mutually exclusive resource. The string passed to this function must be constant until
the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_wait(const char* name, size_t length);

/*! \brief Signal notification
Signal notification. Call this method right before the thread signals state on a
mutually exclusive resource. The string passed to this function must be constant until
the block is written to the output stream.
\param name Lock name
\param length Length of lock name */
FOUNDATION_API void
profile_signal(const char* name, size_t length);

#else

#define profile_initialize( ... ) do { FOUNDATION_UNUSED_ARGS_4( __VA_ARGS__ ); } while(0)
#define profile_finalize() do {} while(0)
#define profile_enable( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define profile_set_output( ... ) do { /*FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ );*/ } while(0)
#define profile_set_output_wait( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define profile_end_frame( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define profile_begin_block_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_begin_block( ... ) profile_begin_block_( __VA_ARGS__ )
#define profile_update_block( ... ) do {} while(0)
#define profile_end_block( ... ) do {} while(0)

#define profile_log_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_log( ... ) profile_log_( __VA_ARGS__ )
#define profile_trylock_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_trylock( ... ) profile_trylock_( __VA_ARGS__ )
#define profile_lock_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_lock( ... ) profile_lock_( __VA_ARGS__ )
#define profile_unlock_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_unlock( ... ) profile_unlock_( __VA_ARGS__ )
#define profile_wait_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_wait( ... ) profile_wait_( __VA_ARGS__ )
#define profile_signal_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_signal( ... ) profile_signal_( __VA_ARGS__ )

#endif
