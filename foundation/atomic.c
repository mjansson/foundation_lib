/* atomic.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#include <foundation/internal.h>

#if FOUNDATION_COMPILER_MSVC

#include <foundation/windows.h>

void
internal_atomic_thread_fence_sequentially_consistent() {
	MemoryBarrier();
}

#endif

int
internal_atomic_initialize(void) {
	return 0;
}

void
internal_atomic_finalize(void) {
}
