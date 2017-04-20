/* test.c  -  Foundation test library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include "mock.h"

#if FOUNDATION_PLATFORM_POSIX

#include <sys/mman.h>
#include <dlfcn.h>

static bool  mmap_is_mocked;
static void* mmap_return_value;
static int   mmap_errno;

static bool  munmap_is_mocked;
static int   munmap_return_value;
static int   munmap_errno;

void*
mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset) {
	if (mmap_is_mocked) {
		errno = mmap_errno;
		return mmap_return_value;
	}
#if defined(__linux__) || defined(__APPLE__)
	char fname[] = "mmap";
#else
	char fname[] = "_mmap";
#endif
	static void* real_mmap = 0;
	if (!real_mmap)
		real_mmap = dlsym(RTLD_NEXT, fname);
	return (*(void* (*)(void*, size_t, int, int, int, off_t))real_mmap)(addr, len, prot, flags, fd, offset);
}

void
mmap_mock(void* return_value, int err) {
	mmap_is_mocked = true;
	mmap_return_value = return_value;
	mmap_errno = err;
}

void
mmap_unmock(void) {
	mmap_is_mocked = false;
}

int
munmap(void* addr, size_t size) {
	if (munmap_is_mocked) {
		errno = munmap_errno;
		return munmap_return_value;
	}
#if defined(__linux__) || defined(__APPLE__)
	char fname[] = "munmap";
#else
	char fname[] = "_munmap";
#endif
	static void* real_munmap = 0;
	if (!real_munmap)
		real_munmap = dlsym(RTLD_NEXT, fname);
	return (*(int (*)(void*, size_t))real_munmap)(addr, size);
}

void
munmap_mock(int return_value, int err) {
	munmap_is_mocked = true;
	munmap_return_value = return_value;
	munmap_errno = err;
}

void
munmap_unmock(void) {
	munmap_is_mocked = false;
}

#elif FOUNDATION_PLATFORM_WINDOWS




#endif
