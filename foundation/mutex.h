/* mutex.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file mutex.h
\brief Mutex for thread synchronization and signalling.

Mutex for thread synchronization and signalling.

A mutex (mutually exclusive) provides the means to synchronize threads. A mutex
can only be locked by one thread at any given time, and another thread that
tries to acquire a lock will be suspended until the lock is released by the
first thread.

A mutex can also be used to signal a thread to wake up from a sleeping state.
The sleeper thread should use the wait method, and then the signal method
can be used to trigger the thread to wake up. Locks on the mutex can be used
to synchronize execution between multiple sleeper threads.

Foundation mutexes are reentrant, a thread can call lock multiple times from
the same thread without blocking. The same number of matching unlock calls
have to be done before mutex is actually unlocked for other threads to access. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate new mutex and allocate system resources
\param name Mutex name (does not have to be unique)
\param length Length of mutex name
\return New mutex */
FOUNDATION_API mutex_t*
mutex_allocate(const char* name, size_t length);

/*! Deallocate mutex and free system resources
\param mutex Mutex */
FOUNDATION_API void
mutex_deallocate(mutex_t* mutex);

/*! Get mutex name
\param mutex Mutex
\return Mutex name, empty string if invalid mutex or no name */
FOUNDATION_API string_const_t
mutex_name(mutex_t* mutex);

/*! Try to lock mutex but do not block for any amount of time.
\param mutex Mutex
\return true if mutex was locked, false if mutex already locked by another thread */
FOUNDATION_API bool
mutex_try_lock(mutex_t* mutex);

/*! Lock mutex and block if unavailable for an indefinite amount of time.
\param mutex Mutex
\return true if mutex was locked, false if error */
FOUNDATION_API bool
mutex_lock(mutex_t* mutex);

/*! Unlock mutex
\param mutex Mutex
\return true if mutex was unlocked, false if error or mutex still locked or if
        the mutex was not locked by the calling thread */
FOUNDATION_API bool
mutex_unlock(mutex_t* mutex);

/*! Block and wait for signal for an indefinite amount of time. If a signal was
received and this function returns true, the mutex will be locked and must be
unlocked when not needed anymore. If the function returns false, an error occurred
and the mutex will NOT be locked.
\param mutex Mutex
\return true if signal received, false if error */
FOUNDATION_API bool
mutex_wait(mutex_t* mutex);

/*! Block and wait for signal. If a signal was received and this function returns
true, the mutex will be locked and must be unlocked when not needed anymore.
If the function returns false, a timeout or error occurred and the mutex will
NOT be locked.
\param mutex Mutex
\param milliseconds Timeout in milliseconds, 0 means no wait
\return true if signal received, false if timeout or error */
FOUNDATION_API bool
mutex_try_wait(mutex_t* mutex, unsigned int milliseconds);

/*! Signal mutex and wake up threads waiting for a signal on the mutex.
\param mutex Mutex */
FOUNDATION_API void
mutex_signal(mutex_t* mutex);

#if FOUNDATION_PLATFORM_WINDOWS

/*! Windows only, get OS handle for event object
\param mutex Mutex
\return Event object handle */
FOUNDATION_API void*
mutex_event_handle(mutex_t* mutex);

#endif
