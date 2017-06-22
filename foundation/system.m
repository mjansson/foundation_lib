/* system.m  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <foundation/apple.h>

unsigned int
_system_process_info_processor_count(void);

int
_system_show_alert(const char* title, size_t title_length, const char* message,
                   size_t message_length, int cancel_button);

unsigned int
_system_process_info_processor_count(void) {
	/* [[NSProcessInfo processInfo] activeProcessorCount]; */
	@autoreleasepool { return (unsigned int)[[NSProcessInfo processInfo] processorCount]; }
}

int
_system_show_alert(const char* title, size_t title_length, const char* message,
                   size_t message_length, int cancel_button) {
	//TODO: Use cancel_button control
	FOUNDATION_UNUSED(cancel_button);
	@autoreleasepool {
#if FOUNDATION_PLATFORM_MACOS

		NSAlert* alert = [NSAlert
		                  alertWithMessageText:[[NSString alloc] initWithBytes:title length:title_length encoding:
		                                        NSUTF8StringEncoding]
		                  defaultButton:[NSString stringWithCString:"OK" encoding:NSASCIIStringEncoding]
		                  alternateButton:[NSString stringWithCString:"Cancel" encoding:NSASCIIStringEncoding]
		                  otherButton:nil
		                  informativeTextWithFormat:@"%@",
		                  [[NSString alloc] initWithBytes:message length:message_length encoding:NSUTF8StringEncoding]];
		NSInteger button = [alert runModal];
		return (button == 1/*NSAlertDefaultReturn*/) ? 1 : 0;

#elif FOUNDATION_PLATFORM_IOS

		UIAlertController* alert = [UIAlertController
		                            alertControllerWithTitle:[[NSString alloc] initWithBytes:title length:title_length encoding:
		                                                      NSUTF8StringEncoding]
		                            message:[[NSString alloc] initWithBytes:message length:message_length encoding:NSUTF8StringEncoding]
		                            preferredStyle:UIAlertControllerStyleAlert];

		[[(__bridge UIWindow*)delegate_window() rootViewController] presentViewController:alert animated:YES completion:nil];
		return 1;

#endif
	}
}
