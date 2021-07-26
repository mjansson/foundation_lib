/* beacon.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file beacon.h
\brief Beacon for event wait

A beacon can wait on a single or multiple event sources. Note that there can be spurious
wakeups when waiting on events even if no event fired. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate beacon
\return New beacon */
FOUNDATION_API beacon_t*
beacon_allocate(void);

/*! Initialize beacon
\param beacon Beacon to initialize */
FOUNDATION_API void
beacon_initialize(beacon_t* beacon);

/*! Finalize beacon
\param beacon Beacon */
FOUNDATION_API void
beacon_finalize(beacon_t* beacon);

/*! Deallocate beacon
\param beacon Beacon to deallocate */
FOUNDATION_API void
beacon_deallocate(beacon_t* beacon);

/*! Wait on beacon, for an indefinite amount of time
\param beacon Beacon to wait for
\return index of event causing the beacon to fire, negative if error */
FOUNDATION_API int
beacon_wait(beacon_t* beacon);

/*! Wait on beacon for the given amount of time
\param beacon Beacon to wait for
\param milliseconds Timeout in milliseconds, 0 meaning no wait
\return index of event causing the beacon to fire, negative if timeout or error */
FOUNDATION_API int
beacon_try_wait(beacon_t* beacon, unsigned int milliseconds);

/*! Fire the beacon, using event zero
\param beacon Beacon to fire */
FOUNDATION_API void
beacon_fire(beacon_t* beacon);

/*! Add another beacon as event source to the beacon
\param beacon Beacon
\param remote Remote beacon to add as source
\return index of remote beacon in beacon, negative if error */
FOUNDATION_API int
beacon_add_beacon(beacon_t* beacon, beacon_t* remote);

/*! Remove another beacon as event source from the beacon.
\param beacon Beacon
\param remote Remote beacon to remove as source */
FOUNDATION_API void
beacon_remove_beacon(beacon_t* beacon, beacon_t* remote);

#if FOUNDATION_PLATFORM_WINDOWS

/*! Add another event source to the beacon, for example a semaphore,
a pipe or another beacon. Any handle that can be passed to
WaitForMultipleEvents can be added to the beacon.
\param beacon Beacon
\param handle Handle to add
\return index of handle in beacon, negative if error */
FOUNDATION_API int
beacon_add_handle(beacon_t* beacon, void* handle);

/*! Remove another event source from the beacon.
\param beacon Beacon
\param handle Handle to remove */
FOUNDATION_API void
beacon_remove_handle(beacon_t* beacon, void* handle);

#endif

#if FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_APPLE || \
    FOUNDATION_PLATFORM_BSD || FOUNDATION_PLATFORM_WINDOWS

/*! Add another event source to the beacon, for example a socket,
a pipe or another beacon. Any file descriptor handle that can be used
in a select/kevent call can be added to the beacon.
\param beacon Beacon
\param fd File descriptor to add
\return index of file descriptor in beacon, negative if error */
FOUNDATION_API int
beacon_add_fd(beacon_t* beacon, int fd);

/*! Remove another event source from the beacon.
\param beacon Beacon
\param fd File descriptor to remove */
FOUNDATION_API void
beacon_remove_fd(beacon_t* beacon, int fd);

#endif
