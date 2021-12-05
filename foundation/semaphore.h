/* semaphore.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file semaphore.h
\brief Semaphore

Semaphore for thread synchronization and notification. For more information, see
https://en.wikipedia.org/wiki/Semaphore_(programming) */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Initialize unnamed semaphore with the given value.
\param semaphore Semaphore
\param value Initial value
\return true if successful, false if error */
FOUNDATION_API bool
semaphore_initialize(semaphore_t* semaphore, uint value);

/*! Initialize a named semaphore with the given value.
\param semaphore Semaphore
\param name Global name
\param length Length of name
\param value Initial value
\return true if successful, false if error */
FOUNDATION_API bool
semaphore_initialize_named(semaphore_t* semaphore, const char* name, size_t length, uint value);

/*! Finalize semaphore. The semaphore value must be >= than the value it was created with,
or it will be considered to be still in use (on OSX/iOS targets this will cause a debug abort).
\param semaphore Semaphore */
FOUNDATION_API void
semaphore_finalize(semaphore_t* semaphore);

/*! Wait on semaphore indefinitely
\param semaphore Semaphore
\return true if successful, false if error or interrupted */
FOUNDATION_API bool
semaphore_wait(semaphore_t* semaphore);

/*! Try waiting on semaphore for a given amount of time.
\param semaphore Semaphore
\param milliseconds Timeout in milliseconds, 0 means no wait
\return true if successful, false if timeout or error/interrupted */
FOUNDATION_API bool
semaphore_try_wait(semaphore_t* semaphore, uint milliseconds);

/*! Post semaphore.
\param semaphore Semaphore */
FOUNDATION_API void
semaphore_post(semaphore_t* semaphore);

/*! Post semaphore multiple times
\param semaphore Semaphore
\param count Number of times */
FOUNDATION_API void
semaphore_post_multiple(semaphore_t* semaphore, uint count);

#if FOUNDATION_PLATFORM_WINDOWS

/*! Windows only, get OS handle for semaphore
\param semaphore Semaphore
\return Object handle */
FOUNDATION_API void*
semaphore_event_handle(semaphore_t* semaphore);

#endif
