/* apple.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file apple.h
\brief Apple system header includes

Safe inclusion of mach and Apple headers for both OSX and iOS targets. Use this header
instead of direct inclusion of mach/Apple headers to avoid compilation problems with
multiple or missing definitions.

\internal NOTE - The base of all header problems with XCode is that
<code>#include <Foundation/Foundation.h></code> in system headers will actually map
to our foundation/foundation.h \endinternal */

// We need to pull in all foundation headers here to make sure everything is included
// before overriding types, in case the system framework includes maps back to
// our headers.
#include <foundation/foundation.h>

#if FOUNDATION_PLATFORM_APPLE

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#define __error_t_defined 1

#define semaphore_t __system_semaphore_t
#define _UUID_T
#define uuid_generate_random __system_uuid_generate_random
#define uuid_generate_time __system_uuid_generate_time
#define uuid_is_null __system_uuid_is_null
#define semaphore_wait __system_semaphore_wait
#define semaphore_destroy __system_semaphore_destroy
#define thread_create __system_thread_create
#define thread_terminate __system_thread_terminate
#define task_t __system_task_t
#define thread_t __system_thread_t
#define uuid_t __darwin_uuid_t

#include <mach/mach_types.h>
#include <mach/mach_interface.h>

#undef semaphore_wait
#undef semaphore_destroy
#undef thread_create
#undef thread_terminate

#ifdef __OBJC__
#include_next <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#if FOUNDATION_PLATFORM_MACOS
#import <AppKit/AppKit.h>
#elif FOUNDATION_PLATFORM_IOS
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#endif
#else
#include <CoreFoundation/CoreFoundation.h>
#if FOUNDATION_PLATFORM_MACOS
#include <CoreServices/CoreServices.h>
#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#undef semaphore_t
#undef _UUID_T
#undef uuid_generate_random
#undef uuid_generate_time
#undef uuid_is_null
#undef task_t
#undef thread_t
#undef uuid_t

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

#endif
