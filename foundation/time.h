/* time.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file time.h
\brief Time measurements

Time measurements. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Get current timestamp, in ticks of system-specific frequency (queryable with
#time_ticks_per_second), measured from some system-specific base timestamp and not in sync
with other timestamps.
\return Current timestamp */
FOUNDATION_API tick_t
time_current(void);

/*! Calculate time difference.
\param from Start timestamp
\param to End timestamp
\return Elapsed time (difference) in ticks */
FOUNDATION_API tick_t
time_diff(const tick_t from, const tick_t to);

/*! Get elapsed time since given timestamp.
\param since Timestamp
\return Number of seconds elapsed */
FOUNDATION_API deltatime_t
time_elapsed(const tick_t since);

/*! Get elapsed ticks since given timestamp.
\param since Timestamp
\return Number of ticks elapsed */
FOUNDATION_API tick_t
time_elapsed_ticks(const tick_t since);

/*! Get time frequency, as number of ticks per second.
\return Ticks per second */
FOUNDATION_API tick_t
time_ticks_per_second(void);

/*! Get ticks as seconds (effectively calculating <code>ticks / time_ticks_per_second()</code>).
\param dt Deltatime in ticks
\return Deltatime in seconds */
FOUNDATION_API deltatime_t
time_ticks_to_seconds(const tick_t dt);

/*! Get ticks as milliseconds (effectively calculating <code>1000.0 * (ticks /
time_ticks_per_second())</code>). \param dt Deltatime in ticks \return Deltatime in milliseconds */
FOUNDATION_API deltatime_t
time_ticks_to_milliseconds(const tick_t dt);

/*! Get startup timestamp.
\return Startup timestamp */
FOUNDATION_API tick_t
time_startup(void);

/*! Get system time, in milliseconds since the epoch (UNIX time).
\return Current timestamp, in milliseconds */
FOUNDATION_API tick_t
time_system(void);
