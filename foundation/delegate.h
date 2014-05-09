/* delegate.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file delegate.h
    Application delegate and entry points for OSX & iOS */

#include <foundation/platform.h>
#include <foundation/types.h>

#include <foundation/apple.h>


/*! \fn delegate_start_main_ns_thread
    Start the main thread as a separate thread. The process entry thread will go on
    and run the main Cocoa event loop. */

#if FOUNDATION_PLATFORM_APPLE

FOUNDATION_API void delegate_start_main_ns_thread( void );


#if FOUNDATION_PLATFORM_MACOSX

#ifdef __OBJC__

@interface FoundationAppDelegate : NSObject <NSApplicationDelegate>
{
    NSWindow* window;
}
@property (nonatomic, retain) IBOutlet NSWindow *window;
+ (void)referenceClass;
@end

#endif

/*! Get the window associated with the application delegate
    \return Window */
FOUNDATION_API void* delegate_nswindow( void );


#elif FOUNDATION_PLATFORM_IOS

#ifdef __OBJC__

@interface FoundationAppDelegate : NSObject <UIApplicationDelegate>
{
    UIWindow* window;
}
@property (nonatomic, retain) IBOutlet UIWindow *window;
+ (void)referenceClass;
@end

@interface FoundationAlertViewDelegate : NSObject <UIAlertViewDelegate>
{
}
@end

#endif

/*! Get the window associated with the application delegate
    \return Window */
FOUNDATION_API void* delegate_uiwindow( void );

#endif

#endif
