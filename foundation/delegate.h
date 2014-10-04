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


/*! MacOS X and iOS only. Start the main thread as a separate thread. The process entry thread will go on
    and run the main Cocoa event loop. */
FOUNDATION_API void delegate_start_main_ns_thread( void );

/*! MacOS X and iOS only. Ensures all delegate classes and methods have references to prevent from
    being stripped in optimization passes during compilation and linking */
FOUNDATION_API void delegate_reference_classes( void );

/*! MacOS X only. Get the currently assigned window from the delegate
    object window outlet.
    \return Window object */
FOUNDATION_API void* delegate_nswindow( void );

/*! \fn delegate_uiwindow
    iOS only. Get the main UI application window.
    \return Windows object*/
FOUNDATION_API void* delegate_uiwindow( void );


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

#endif

