/* environment.m  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <foundation/foundation.h>
#include <foundation/posix.h>
#include <foundation/apple.h>

extern void
environment_ns_command_line(string_t** argv);
extern string_t
environment_ns_home_directory(char*, size_t);
extern string_t
environment_ns_temporary_directory(char*, size_t);
extern string_t
environment_ns_current_working_directory(char* buffer, size_t capacity);
extern bool
environment_ns_set_current_working_directory(const char* buffer, size_t length);

string_t
environment_bundle_identifier(char* target, size_t maxlength) {
	@autoreleasepool {
		NSString* bundle_identifier = [[NSBundle mainBundle] bundleIdentifier];
		const char* bundlestr = [bundle_identifier UTF8String];
		return string_copy(target, maxlength, bundlestr, string_length(bundlestr));
	}
}

string_t
environment_bundle_path(char* buffer, size_t capacity) {
	@autoreleasepool {
		NSString* bundle_path = [[NSBundle mainBundle] bundlePath];
		const char* bundlestr = [bundle_path UTF8String];
		return string_copy(buffer, capacity, bundlestr, string_length(bundlestr));
	}
}

void
environment_ns_command_line(string_t** argv) {
	@autoreleasepool {
		char buffer[BUILD_MAX_PATHLEN];
		NSArray* arguments = [[NSProcessInfo processInfo] arguments];
		for (id arg in arguments) {
			CFStringRef argref = (__bridge CFStringRef)arg;
			CFStringGetCString(argref, buffer, BUILD_MAX_PATHLEN, kCFStringEncodingUTF8);
			array_push(*argv, string_clone(buffer, string_length(buffer)));
		}
	}
}

string_t
environment_ns_home_directory(char* buffer, size_t capacity) {
	@autoreleasepool {
		NSString* homestr = NSHomeDirectory();
		CFStringRef home = (__bridge CFStringRef)homestr;
		if (home && CFStringGetCString(home, buffer, (CFIndex)capacity, kCFStringEncodingUTF8))
			return (string_t){buffer, string_length(buffer)};
	}
	return (string_t){buffer, 0};
}

string_t
environment_ns_temporary_directory(char* buffer, size_t capacity) {
	@autoreleasepool {
		NSString* tmpstr = NSTemporaryDirectory();
		CFStringRef tmp = (__bridge CFStringRef)tmpstr;
		if (tmp && CFStringGetCString(tmp, buffer, (CFIndex)capacity, kCFStringEncodingUTF8))
			return (string_t){buffer, string_length(buffer)};
	}
	return (string_t){buffer, 0};
}

string_t
environment_ns_current_working_directory(char* buffer, size_t capacity) {
	@autoreleasepool {
		NSFileManager* filemgr = [[NSFileManager alloc] init];
		NSString* tmpstr = [filemgr currentDirectoryPath];
		CFStringRef tmp = (__bridge CFStringRef)tmpstr;
		if (tmp && CFStringGetCString(tmp, buffer, (CFIndex)capacity, kCFStringEncodingUTF8))
			return (string_t){buffer, string_length(buffer)};
	}
	return (string_t){buffer, 0};
}

bool
environment_ns_set_current_working_directory(const char* buffer, size_t length) {
	if (!buffer || !length)
		return false;
	@autoreleasepool {
		NSFileManager* filemgr = [[NSFileManager alloc] init];
		NSString* path = [[NSString alloc] initWithBytes:buffer
		                                          length:length
		                                        encoding:NSUTF8StringEncoding];
		if ([filemgr changeCurrentDirectoryPath:path])
			return true;
	}
	return false;
}
