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

#import <Foundation/NSString.h>
#import <Foundation/NSBundle.h>


void environment_bundle_identifier( char* target )
{
	NSString* bundle_identifier = [[NSBundle mainBundle] bundleIdentifier];
	string_copy( target, [bundle_identifier UTF8String], FOUNDATION_MAX_PATHLEN );
}

