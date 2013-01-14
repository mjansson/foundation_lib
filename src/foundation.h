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
#include <assert.h>

#include <timer.h>


//! Entry point
FOUNDATION_API int         foundation_initialize( void );

//! Exit point
FOUNDATION_API void        foundation_shutdown( void );
