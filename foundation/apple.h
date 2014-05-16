/* apple.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file apple.h
    Safe inclusion of mach and Apple headers for both OSX and iOS targets. Use this header
    instead of direct inclusion of mach/Apple headers to avoid compilation problems with
    multiple or missing definitions. */

#include <foundation/platform.h>
#include <foundation/types.h>

//NOTE - The base of all header problems with XCode is that
//       #include <Foundation/Foundation.h>
//       in system headers will actually map to our foundation/foundation.h

#include <foundation/uuid.h>
#include <foundation/radixsort.h>
#include <foundation/semaphore.h>


#if FOUNDATION_PLATFORM_APPLE

#define semaphore_t __system_semaphore_t
#define radixsort __stdlib_radixsort
#define __error_t_defined 1

#define _UUID_T
#define uuid_generate_random __system_uuid_generate_random
#define uuid_generate_time __system_uuid_generate_time
#define uuid_is_null __system_uuid_is_null

#include <mach/mach_types.h>
#include <CoreFoundation/CoreFoundation.h>

#ifdef __OBJC__
#  import <Foundation/NSObject.h>
#  import <Foundation/NSString.h>
#  import <Foundation/NSThread.h>
#  import <Foundation/NSProcessInfo.h>
#  import <Foundation/NSString.h>
#  import <Foundation/NSSet.h>
#  import <Foundation/NSArray.h>
#  import <Foundation/NSTimer.h>
#  import <Foundation/NSUndoManager.h>
#  if FOUNDATION_PLATFORM_MACOSX
#    import <Foundation/NSRunLoop.h>
#    import <AppKit/NSApplication.h>
#    import <AppKit/NSAlert.h>
#    import <AppKit/NSWindow.h>
#  elif FOUNDATION_PLATFORM_IOS
#    import <Foundation/NSUUID.h>
#    import <Foundation/NSCoder.h>
#    import <Foundation/NSAttributedString.h>
#    import <UIKit/UIApplication.h>
#    import <UIKit/UIWindow.h>
#  endif
#else
#  if FOUNDATION_PLATFORM_MACOSX
#    include <Carbon/Carbon.h>
#  endif
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#undef radixsort
#undef semaphore_t

#undef _UUID_T
#undef uuid_generate_random
#undef uuid_generate_time
#undef uuid_is_null

#endif
