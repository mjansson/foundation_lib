/* foundation.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file foundation.h
    Wrapper for foundation library headers */

#include <platform.h>

#include <build.h>
#include <types.h>
#include <bits.h>
#include <assert.h>
#include <memory.h>
#include <atomic.h>
#include <error.h>
#include <thread.h>
#include <log.h>

#include <hash.h>
#include <base64.h>
#include <array.h>
#include <utf.h>

#include <mathcore.h>

#include <objectmap.h>
#include <timer.h>


//! Entry point
FOUNDATION_API int         foundation_initialize( const memory_system_t memory );

//! Exit point
FOUNDATION_API void        foundation_shutdown( void );
