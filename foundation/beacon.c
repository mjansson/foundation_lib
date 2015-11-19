/* beacon.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <foundation/windows.h>

beacon_t*
beacon_allocate(void) {
	beacon_t* beacon = memory_allocate(0, sizeof(beacon_t*), 0, MEMORY_PERSISTENT);
	beacon_initialize(beacon);
	return beacon;
}

void
beacon_initialize(beacon_t* beacon) {
	memset(beacon, 0, sizeof(beacon_t));
#if FOUNDATION_PLATFORM_WINDOWS
	beacon->event = CreateEventA(nullptr, FALSE, FALSE, nullptr);
	beacon->all[0] = beacon->event;
	beacon->dynamic = beacon->all;
	beacon->count = 1;
#endif
}

void
beacon_finalize(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	CloseHandle(beacon->event);
	if (beacon->dynamic != beacon->all)
		array_deallocate(beacon->dynamic);
#endif
}

void
beacon_deallocate(beacon_t* beacon) {
	beacon_finalize(beacon);
}

int
beacon_wait(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	return beacon_try_wait(beacon, INFINITE);
#else
	FOUNDATION_UNUSED(beacon);
	return -1;
#endif
}

int
beacon_try_wait(beacon_t* beacon, unsigned int milliseconds) {
#if FOUNDATION_PLATFORM_WINDOWS
	unsigned int wait_status = WaitForMultipleObjects((DWORD)beacon->count, beacon->dynamic, FALSE,
	                                                  milliseconds);
	if ((wait_status >= WAIT_OBJECT_0) && (wait_status < (WAIT_OBJECT_0 + beacon->count)))
		return (int)(wait_status - WAIT_OBJECT_0);
#else
	FOUNDATION_UNUSED(beacon);
#endif
	return -1;
}

void
beacon_fire(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	SetEvent(beacon->event);
#else
	FOUNDATION_UNUSED(beacon);
#endif
}

#if FOUNDATION_PLATFORM_WINDOWS

int
beacon_add(beacon_t* beacon, void* handle) {
	const size_t fixedsize = (sizeof(beacon->all)/sizeof(beacon->all[0]));
	if (beacon->count < fixedsize)
		beacon->all[beacon->count++] = handle;
	else {
		if (beacon->count == fixedsize) {
			size_t ihandle;
			beacon->dynamic = 0;
			array_reserve(beacon->dynamic, fixedsize*2);
			array_resize(beacon->dynamic, fixedsize);
			beacon->count = array_size(beacon->dynamic);
			for (ihandle = 0; ihandle < beacon->count; ++ihandle)
				beacon->dynamic[ihandle] = beacon->all[ihandle];
		}
		array_push(beacon->dynamic, handle);
		beacon->count = array_size(beacon->dynamic);
	}
	return (int)beacon->count - 1;
}

void
beacon_remove(beacon_t* beacon, void* handle) {
	const size_t fixedsize = (sizeof(beacon->all)/sizeof(beacon->all[0]));
	size_t ihandle;
	void** arr = beacon->all;
	if (beacon->count >= fixedsize)
		arr = beacon->dynamic;
	for (ihandle = 0; ihandle < beacon->count; ++ihandle) {
		if (arr[ihandle] == handle)
			arr[ihandle] = arr[--beacon->count];
	}
}

void*
beacon_event_object(beacon_t* beacon) {
	return beacon->event;
}

#endif

#if FOUNDATION_PLATFORM_POSIX

bool
beacon_add(beacon_t* beacon, int fd) {
	FOUNDATION_UNUSED(beacon);
	FOUNDATION_UNUSED(fd);
	return false;
}

void
beacon_remove(beacon_t* beacon, int fd) {
	FOUNDATION_UNUSED(beacon);
	FOUNDATION_UNUSED(fd);
}

#endif
