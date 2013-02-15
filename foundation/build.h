/* build.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

	You can also in the build configurations #define BUILD_DEPLOY 1 for making a final build for deployment,
	which will disable asserts, most debug output (all output for some platforms) and enable final optimizations
	
	Other configurations can be made with various BUILD_ENABLE_[...] at the end of this file

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

#  if BUILD_RELEASE || BUILD_PROFILE || BUILD_DEPLOY
#  undef DEBUG
#  undef _DEBUG
#  undef NDEBUG
#  define NDEBUG 1
#else
#  undef DEBUG
#  undef _DEBUG
#  undef NDEBUG
#endif


//Configurable choises
#define BUILD_ENABLE_RELEASE_ASSERT           1
#define BUILD_ENABLE_DEPLOY_ASSERT            1

#define BUILD_ENABLE_DEPLOY_ERROR             1

#define BUILD_ENABLE_LOG                      1
#define BUILD_ENABLE_DEBUG_LOG                1

#define BUILD_ERROR_CONTEXT_MAX_DEPTH         32

#define BUILD_ENABLE_PROFILE                  1


// Allocation sizes
#define BUILD_THREAD_MAP_SIZE                 256
#define BUILD_LIBRARY_MAP_SIZE                64

//! Default size for thread stacks
#define BUILD_DEFAULT_THREAD_STACK_SIZE       0x8000
