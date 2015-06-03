/* environment.m  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation/platform.h>
#include <foundation/string.h>
#include <foundation/array.h>
#include <foundation/environment.h>

#import <CoreFoundation/CFString.h>

#import <Foundation/NSString.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSProcessInfo.h>


extern void _environment_ns_command_line( string_t** argv );
extern void _environment_ns_home_directory( char*, size_t );
extern void _environment_ns_temporary_directory( char*, size_t );


string_t environment_bundle_identifier( char* target, size_t maxlength )
{
	@autoreleasepool
	{
		NSString* bundle_identifier = [[NSBundle mainBundle] bundleIdentifier];
		const char* bundlestr = [bundle_identifier UTF8String];
		return string_copy( target, maxlength, bundlestr, string_length( bundlestr ) );
	}
}


void _environment_ns_command_line( string_t** argv )
{
	@autoreleasepool
	{
		char buffer[BUILD_MAX_PATHLEN];
		NSArray* arguments = [[NSProcessInfo processInfo] arguments];
		for( id arg in arguments )
		{
			CFStringRef argref = (__bridge CFStringRef)arg;
			CFStringGetCString( argref, buffer, BUILD_MAX_PATHLEN, kCFStringEncodingUTF8 );
			array_push( *argv, string_clone( buffer, string_length( buffer ) ) );
		}
	}
}


void _environment_ns_home_directory( char* buffer, size_t capacity )
{
	@autoreleasepool
	{
		NSString* homestr = NSHomeDirectory();
		CFStringRef home = (__bridge CFStringRef)homestr;
		CFStringGetCString( home, buffer, (CFIndex)capacity, kCFStringEncodingUTF8 );
	}
}


void _environment_ns_temporary_directory( char* buffer, size_t capacity )
{
	@autoreleasepool
	{
		NSString* tmpstr = NSTemporaryDirectory();
		CFStringRef tmp = (__bridge CFStringRef)tmpstr;
		CFStringGetCString( tmp, buffer, (CFIndex)capacity, kCFStringEncodingUTF8 );
	}
}
