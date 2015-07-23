/* semaphore.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file semaphore.h
\brief Semaphore
\details Semaphore for thread synchronization and notification */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! \brief Initialize semaphore
Initialize unnamed semaphore with the given value.
\param semaphore Semaphore
\param value Initial value */
FOUNDATION_API void
semaphore_initialize(semaphore_t* semaphore, unsigned int value);

/*! \brief Initialize named semaphore
Initialize a named semaphore with the given value.
\param semaphore Semaphore
\param name  Global name
\param value Initial value */
FOUNDATION_API void
semaphore_initialize_named(semaphore_t* semaphore, const char* name, size_t length,
                           unsigned int value);

/*! \brief Finalize semaphore
Finalize semaphore. The semaphore value must be >= than the value it was created with,
or it will be considered to be still in use (on OSX/iOS targets this will cause a debug abort).
\param semaphore Semaphore */
FOUNDATION_API void
semaphore_finalize(semaphore_t* semaphore);

/*! \brief Wait on semaphore
Wait on semaphore.
\param semaphore Semaphore
\return true if successful, false if error or interrupted */
FOUNDATION_API bool
semaphore_wait(semaphore_t* semaphore);

/*! \brief Try wait on semaphore
Try waiting on semaphore.
\param semaphore Semaphore
\param milliseconds Timeout in milliseconds
\return true if successful, false if timeout or error/interrupted */
FOUNDATION_API bool
semaphore_try_wait(semaphore_t* semaphore, unsigned int milliseconds);

/*! \brief Post semaphore
Post semaphore
\param semaphore Semaphore */
FOUNDATION_API void
semaphore_post(semaphore_t* semaphore);

