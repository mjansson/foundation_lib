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

typedef struct _test_log test_log_t;

struct _test_log
{
	void*        view;
	char         msg[];
};


void* test_view_from_tag( void* window, int tag )
{
	if( !window )
		return 0;
	
	return [(UIWindow*)window viewWithTag:tag];
}


void test_text_view_append( void* view, const char* msg )
{
	if( !view )
		return;

	int msglen = string_length( msg );
	test_log_t* log = memory_allocate_context( HASH_TEST, sizeof( test_log_t ) + msglen + 1, 0, MEMORY_PERSISTENT );
	
	log->view = view;
	string_copy( log->msg, msg, msglen + 1 );
	
	dispatch_async( dispatch_get_main_queue(), ^{ @autoreleasepool {
		UITextView* textview = (UITextView*)log->view;
		textview.text = [textview.text stringByAppendingString:[NSString stringWithUTF8String:log->msg]];
		memory_deallocate( log );
	}});
}

#endif
