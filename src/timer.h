/* timer.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file timer.h
    Time measurements */

#include <platform.h>
#include <types.h>


/*! Get current timestamp, in ticks of system-specific frequency (queryable with timer_ticks_per_second), measured from some system-specific base timestamp
    and not in sync with other timestamps
    \return                   Current timestamp */
FOUNDATION_API tick_t         timer_current( void );

/*! Get elapsed time since given timestamp
    \param t                  Timestamp
    \return                   Number of seconds elapsed */
FOUNDATION_API deltatime_t    timer_elapsed( const tick_t t );

/*! Get elapsed ticks since given timestamp
    \param t                  Timestamp
    \return                   Number of ticks elapsed */
FOUNDATION_API tick_t         timer_elapsed_ticks( const tick_t t );

/*! Get timer frequency, as number of ticks per second
    \return                   Ticks per second */
FOUNDATION_API tick_t         timer_ticks_per_second( void );

/*! Get ticks as seconds (effectively calculating ticks/timer_ticks_per_second())
	\param dt                 Deltatime in ticks
    \return                   Deltatime in seconds */
FOUNDATION_API deltatime_t    timer_ticks_to_seconds( const tick_t dt );

/*! Get startup timestamp
    \return                   Startup timestamp */
FOUNDATION_API tick_t         timer_startup( void );

/*! Get system time, in milliseconds since the epoch (UNIX time)
    \return  Current timestamp, in milliseconds */
FOUNDATION_API tick_t         timer_system( void );

