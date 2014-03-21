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
	usable with "#if BUILD_[type]" conditionals

	BUILD_DEBUG   - Debug build
	BUILD_RELEASE - Release build
	BUILD_PROFILE - Deploy build with profiling
	BUILD_DEPLOY  - Deploy build

	Other configurations can be made with various BUILD_ENABLE_[...] at the end of this file. There are also
	a number of BUILD_SIZE_[...] defines for preallocated buffer sizes used.

	*/

#include <foundation/platform.h>

/*! \def BUILD_DEBUG
    Set to 1 for debug build, 0 otherwise. Debug builds are by default unoptimized with full debugging functionality enabled such as asserts, logging, memory tracking and other features */

/*! \def BUILD_RELEASE
    Set to 1 for release build, 0 otherwise. Release builds are by default optimized but with a normal set of debugging functionality enabled such as asserts, some logging and memory tracking */

/*! \def BUILD_PROFILE
    Set to 1 for profile build, 0 otherwise. Profile builds are by default fully optimized and all debugging functionality disabled, but with profiling calls enabled. */

/*! \def BUILD_DEPLOY
    Set to 1 for deploy build, 0 otherwise. Deploy builds are by default fully optimized and have all debugging and profiling functionality disabled. */

/*! \def BUILD_ENABLE_ASSERT
    Set to 1 if asserts are enabled, 0 if disabled */

/*! \def BUILD_ENABLE_ERROR_CONTEXT
    Set to 1 if error context tracking is enabled, 0 if disabled */

/*! \def BUILD_ENABLE_LOG
    Set to 1 if logging is enabled, 0 if disabled */

/*! \def BUILD_ENABLE_DEBUG_LOG
    Set to 1 if debug logging is enabled, 0 if disabled */

/*! \def BUILD_ENABLE_DEBUG_CONFIG
    Set to 1 if config system debugging is enabled, 0 if disabled */

/*! \def BUILD_ENABLE_PROFILE
    Set to 1 if profiling hooks and system is enabled, 0 if disabled */

/*! \def BUILD_ENABLE_MEMORY_CONTEXT
    Set to 1 if memory context tracking is enabled, 0 if disabled */

/*! \def BUILD_ENABLE_MEMORY_TRACKER
    Set to 1 if memory allocation tracking is enabled, 0 if disabled */

/*! \def BUILD_ENABLE_STATIC_HASH_DEBUG
    Set to 1 if static string hash debugging is enabled allowing reverse hash lookups, 0 if disabled */

/*! \def BUILD_DEFAULT_STREAM_BYTEORDER
    The default stream byte order used if the byte order is not explicitly set on a stream */

/*! \def BUILD_SIZE_THREAD_MAP
    The maximum number of concurrently allocated thread objects */

/*! \def BUILD_SIZE_LIBRARY_MAP
    The maximum number of concurrently allocated dynamic library objects */

/*! \def BUILD_SIZE_TEMPORARY_MEMORY
    Default size of temporary (linear) memory allocator buffer */

/*! \def BUILD_SIZE_EVENT_BLOCK_LIMIT
    Maximum allowed size for an event block (memory used by events between process calls) */

/*! BUILD_SIZE_DEFAULT_THREAD_STACK
    Default size for thread stacks */

/*! \def BUILD_SIZE_ERROR_CONTEXT_DEPTH
    Maximum error context depth */

/*! \def BUILD_SIZE_MEMORY_CONTEXT_DEPTH
    Maximum memory context depth */

/*! \def BUILD_SIZE_STACKTRACE_DEPTH
    Maximum stacktrace depth */

/*! \def BUILD_SIZE_FS_MONITORS
    Maximum number of filesystem monitors */

/*! \def BUILD_SIZE_STATIC_HASH_STORE
    Maximum number of statically hashed strings stored in lookup (if BUILD_ENABLE_STATIC_HASH_DEBUG is enabled) */


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

#define BUILD_ENABLE_DEBUG_CONFIG             0

#ifndef BUILD_ENABLE_PROFILE
#if BUILD_DEBUG || BUILD_RELEASE || BUILD_PROFILE
#define BUILD_ENABLE_PROFILE                  1
#else
#define BUILD_ENABLE_PROFILE                  0
#endif
#endif

#ifndef BUILD_ENABLE_MEMORY_CONTEXT
#if BUILD_DEBUG || BUILD_RELEASE
#define BUILD_ENABLE_MEMORY_CONTEXT           1
#else
#define BUILD_ENABLE_MEMORY_CONTEXT           0
#endif
#endif

#ifndef BUILD_ENABLE_MEMORY_TRACKER
#if BUILD_DEBUG || BUILD_RELEASE
#define BUILD_ENABLE_MEMORY_TRACKER           1
#else
#define BUILD_ENABLE_MEMORY_TRACKER           0
#endif
#endif

#ifndef BUILD_ENABLE_STATIC_HASH_DEBUG
#if !BUILD_DEPLOY && FOUNDATION_PLATFORM_FAMILY_DESKTOP
#define BUILD_ENABLE_STATIC_HASH_DEBUG        1
#else
#define BUILD_ENABLE_STATIC_HASH_DEBUG        0
#endif
#endif


#define BUILD_DEFAULT_STREAM_BYTEORDER        BYTEORDER_LITTLEENDIAN


#define BUILD_SIZE_THREAD_MAP                 256
#define BUILD_SIZE_LIBRARY_MAP                64

#define BUILD_SIZE_TEMPORARY_MEMORY           2 * 1024 * 1024

#define BUILD_SIZE_EVENT_BLOCK_LIMIT          ( 1 * 1024 * 1024 )

#define BUILD_SIZE_DEFAULT_THREAD_STACK       0x8000

#define BUILD_SIZE_ERROR_CONTEXT_DEPTH        32

#define BUILD_SIZE_MEMORY_CONTEXT_DEPTH       32

#define BUILD_SIZE_STACKTRACE_DEPTH           32

#define BUILD_SIZE_FS_MONITORS                32

#define BUILD_SIZE_STATIC_HASH_STORE          4192
