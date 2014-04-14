/* atomic.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


#if FOUNDATION_ARCH_ARM && FOUNDATION_ARCH_THUMB

__asm__(
	"   .align 2\n"
	"   .globl _atomic_thread_fence_acquire\n"
	"   .globl _atomic_thread_fence_release\n"
	"   .globl _atomic_thread_fence_sequentially_consistent\n"
	"_atomic_thread_fence_acquire:\n"
	"_atomic_thread_fence_release:\n"
	"_atomic_thread_fence_sequentially_consistent:\n"
	"   mov r0, #0\n"
	"   mcr p15, 0, r0, c7, c10, 5\n"
	"   bx lr\n"
);

#endif
