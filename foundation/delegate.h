/* delegate.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file delegate.h
\brief Application delegate

Application delegate and entry points for macOS and iOS */

#include <foundation/platform.h>
#include <foundation/types.h>

#include <foundation/apple.h>

#if FOUNDATION_PLATFORM_MACOS || FOUNDATION_PLATFORM_IOS

/*! macOS and iOS only. Start the main thread as a separate thread. The process entry thread
will go on and run the main Cocoa event loop. */
FOUNDATION_API void
delegate_start_main_ns_thread(void);

/*! macOS and iOS only. Ensures all delegate classes and methods have references to prevent
from being stripped in optimization passes during compilation and linking */
FOUNDATION_API void
delegate_reference_classes(void);

#endif

#if FOUNDATION_PLATFORM_MACOS

/*! macOS only. Get the currently assigned window from the delegate object window outlet.
\return Window object */
FOUNDATION_API void*
delegate_window(void);

#if defined(__OBJC__) && !defined(FOUNDATION_NO_INTERFACE)

/*! Application delegate for foundation library projects. Assign the main application window
object to the window outlet for automatic integration between foundation library and
windowing services */
@interface FoundationAppDelegate : NSObject <NSApplicationDelegate>
@property (nonatomic, retain) IBOutlet NSWindow* window;
+ (void)referenceClass;
@end

#endif

#endif

#if FOUNDATION_PLATFORM_IOS

/*! iOS only. Get the main UI application window.
\return Windows object */
FOUNDATION_API void*
delegate_window(void);

#if defined(__OBJC__) && !defined(FOUNDATION_NO_INTERFACE)

@interface FoundationAppDelegate : NSObject <UIApplicationDelegate>
@property (nonatomic, retain) IBOutlet UIWindow* window;
+ (void)referenceClass;
@end

/*! UI alert view delegate used by system debug message alert views */
@interface FoundationAlertViewDelegate : NSObject <UIAlertViewDelegate>
@end

#endif

#endif
