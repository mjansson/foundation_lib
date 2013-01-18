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

#include <platform.h>

#if defined( NDEBUG )
#  undef DEBUG
#  undef _DEBUG
#  undef NDEBUG
#  undef BUILD_DEBUG
#  undef BUILD_RELEASE

#  define NDEBUG 1
#  define BUILD_RELEASE 1
#else
#  undef DEBUG
#  undef _DEBUG
#  undef NDEBUG
#  undef BUILD_DEBUG
#  undef BUILD_RELEASE
#  undef BUILD_DEPLOY

#  define BUILD_DEBUG 1
#endif

#ifndef BUILD_DEPLOY
#  define BUILD_DEPLOY 0
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
#define BUILD_LIBRARY_MAP_SIZE                64
