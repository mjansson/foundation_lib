/* test.m  -  Foundation test library  -  Public Domain  -  2014 Mattias Jansson / Rampant Pixels
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

#include <foundation/foundation.h>
#include <foundation/apple.h>

#include <test/test.h>


#if FOUNDATION_PLATFORM_IOS

# import <UIKit/UITextView.h>


void test_text_view_append( void* window, int tag, const char* msg )
{
	if( !window )
		return;

	__strong UITextView* textview = (UITextView*)[(__bridge UIWindow*)window viewWithTag:tag];
	if( !textview )
		return;

	@autoreleasepool
	{
		NSString* nsmsg = [NSString stringWithUTF8String:msg];
		dispatch_async( dispatch_get_main_queue(), ^{ @autoreleasepool {
			textview.text = [textview.text stringByAppendingString:nsmsg];
		}});
	}
}

#endif
