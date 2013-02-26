/* build.h  -  Foundation library build setup  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file build.h
    Build setup. This header unifies the debug/release build macros and provides the following macro idenfiers,
	usable with #if BUILD_DEBUG / #if BUILD_RELEASE

	BUILD_DEBUG   - Debug build
	BUILD_RELEASE - Release build
	BUILD_PROFILE - Deploy build with profiling
	BUILD_DEPLOY  - Deploy build

	Other configurations can be made with various BUILD_ENABLE_[...] at the end of this file. There are also
	a number of BUILD_SIZE_[...] defines for preallocated buffer sizes used.

	*/

#include <foundation/platform.h>


#ifndef BUILD_DEBUG
#  define BUILD_DEBUG 0
#endif

#ifndef BUILD_RELEASE
#  define BUILD_RELEASE 0
#endif

#ifndef BUILD_PROFILE
#  define BUILD_PROFILE 0
#endif

#ifndef BUILD_DEPLOY
#  define BUILD_DEPLOY 0
#endif

#if !BUILD_DEBUG && !BUILD_RELEASE && !BUILD_PROFILE && !BUILD_DEPLOY
#  if defined( NDEBUG )
#    undef  BUILD_RELEASE
#    define BUILD_RELEASE 1
#  else
#    undef  BUILD_DEBUG
#    define BUILD_DEBUG   1
#  endif
#endif

#if BUILD_DEBUG
#  undef DEBUG
#  undef _DEBUG
#  undef NDEBUG
#  define _DEBUG 1
#else
#  undef DEBUG
#  undef _DEBUG
#  undef NDEBUG
#  define NDEBUG 1
#endif


// Configurable choises
#ifndef BUILD_ENABLE_ASSERT
#if BUILD_DEBUG || BUILD_RELEASE
#define BUILD_ENABLE_ASSERT                   1
#else
#define BUILD_ENABLE_ASSERT                   0
#endif
#endif

#ifndef BUILD_ENABLE_ERROR_CONTEXT
#if BUILD_DEBUG || BUILD_RELEASE
#define BUILD_ENABLE_ERROR_CONTEXT            1
#else
#define BUILD_ENABLE_ERROR_CONTEXT            0
#endif
#endif

#ifndef BUILD_ENABLE_LOG
#if ( FOUNDATION_PLATFORM_FAMILY_CONSOLE || FOUNDATION_PLATFORM_FAMILY_MOBILE ) && ( BUILD_DEPLOY || BUILD_PROFILE )
#define BUILD_ENABLE_LOG                      0
#else
#define BUILD_ENABLE_LOG                      1
#endif
#endif

#ifndef BUILD_ENABLE_DEBUG_LOG
#if BUILD_DEBUG
#define BUILD_ENABLE_DEBUG_LOG                1
#else
#define BUILD_ENABLE_DEBUG_LOG                0
#endif
#endif

#ifndef BUILD_ENABLE_PROFILE
#if BUILD_PROFILE
#define BUILD_ENABLE_PROFILE                  1
#else
#define BUILD_ENABLE_PROFILE                  1
#endif
#endif


// Allocation sizes
#define BUILD_SIZE_THREAD_MAP                 256
#define BUILD_SIZE_LIBRARY_MAP                64

// Default size for thread stacks
#define BUILD_SIZE_DEFAULT_THREAD_STACK       0x8000

// Maximum error context depth
#define BUILD_SIZE_ERROR_CONTEXT_DEPTH        32
