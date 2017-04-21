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

#include <foundation/posix.h>

#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/wait.h>

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic ignored "-Winvalid-noreturn"
#endif

#define ADD_MOCK_BASE(fn, ret) \
static bool fn##_is_mocked; \
static ret  fn##_return_value; \
static int  fn##_errno

#define ADD_MOCK_NORET_BASE(fn) \
static bool fn##_is_mocked; \
static int  fn##_errno

#define ADD_MOCK_LOOKUP(fn) \
	static void* real_##fn = 0; \
	if (!real_##fn) \
		real_##fn = dlsym(RTLD_NEXT, #fn)

#define ADD_MOCK_IMPL(fn) \
	if (fn##_is_mocked) { \
		errno = fn##_errno; \
		return fn##_return_value; \
	} \
	ADD_MOCK_LOOKUP(fn)

#define ADD_MOCK_NORET_IMPL(fn) \
	if (fn##_is_mocked) { \
		errno = fn##_errno; \
		return; \
	} \
	ADD_MOCK_LOOKUP(fn)

#define ADD_MOCK_TOGGLES(fn, ret) \
void \
fn##_mock(ret return_value, int err) { \
	fn##_is_mocked = true; \
	fn##_return_value = return_value; \
	fn##_errno = err; \
} \
void \
fn##_unmock(void) { \
	fn##_is_mocked = false; \
}

#define ADD_MOCK_NORET_TOGGLES(fn) \
void \
fn##_mock(int err) { \
	fn##_is_mocked = true; \
	fn##_errno = err; \
} \
void \
fn##_unmock(void) { \
	fn##_is_mocked = false; \
}

#define ADD_MOCK_0(fn, ret) \
ADD_MOCK_BASE(fn, ret); \
ret \
fn(void) { \
	ADD_MOCK_IMPL(fn); \
	return (*(ret (*)())real_##fn)(); \
} \
ADD_MOCK_TOGGLES(fn, ret)
/*
#define ADD_MOCK_1(fn, ret, type0) \
ADD_MOCK_BASE(fn, ret); \
ret \
fn(type0 arg0) { \
	ADD_MOCK_IMPL(fn); \
	return (*(ret (*)(type0))real_##fn)(arg0); \
} \
ADD_MOCK_TOGGLES(fn, ret)
*/
#define ADD_MOCK_NORET_1(fn, type0) \
ADD_MOCK_NORET_BASE(fn); \
void \
fn(type0 arg0) { \
	ADD_MOCK_NORET_IMPL(fn); \
	(*(void (*)(type0))real_##fn)(arg0); \
} \
ADD_MOCK_NORET_TOGGLES(fn)

#define ADD_MOCK_2(fn, ret, type0, type1) \
ADD_MOCK_BASE(fn, ret); \
ret \
fn(type0 arg0, type1 arg1) { \
	ADD_MOCK_IMPL(fn); \
	return (*(ret (*)(type0, type1))real_##fn)(arg0, arg1); \
} \
ADD_MOCK_TOGGLES(fn, ret)

#define ADD_MOCK_3(fn, ret, type0, type1, type2) \
ADD_MOCK_BASE(fn, ret); \
ret \
fn(type0 arg0, type1 arg1, type2 arg2) { \
	ADD_MOCK_IMPL(fn); \
	return (*(ret (*)(type0, type1, type2))real_##fn)(arg0, arg1, arg2); \
} \
ADD_MOCK_TOGGLES(fn, ret)
/*
#define ADD_MOCK_4(fn, ret, type0, type1, type2, type3) \
ADD_MOCK_BASE(fn, ret); \
ret \
fn(type0 arg0, type1 arg1, type2 arg2, type3 arg3) { \
	ADD_MOCK_IMPL(fn); \
	return (*(ret (*)(type0, type1, type2, type3))real_##fn)(arg0, arg1, arg2, arg3); \
} \
ADD_MOCK_TOGGLES(fn, ret)

#define ADD_MOCK_5(fn, ret, type0, type1, type2, type3, type4) \
ADD_MOCK_BASE(fn, ret); \
ret \
fn(type0 arg0, type1 arg1, type2 arg2, type3 arg3, type4 arg4) { \
	ADD_MOCK_IMPL(fn); \
	return (*(ret (*)(type0, type1, type2, type3, type4))real_##fn)(arg0, arg1, arg2, arg3, arg4); \
} \
ADD_MOCK_TOGGLES(fn, ret)
*/
#define ADD_MOCK_6(fn, ret, type0, type1, type2, type3, type4, type5) \
ADD_MOCK_BASE(fn, ret); \
ret \
fn(type0 arg0, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) { \
	ADD_MOCK_IMPL(fn); \
	return (*(ret (*)(type0, type1, type2, type3, type4, type5))real_##fn)(arg0, arg1, arg2, arg3, arg4, arg5); \
} \
ADD_MOCK_TOGGLES(fn, ret)

ADD_MOCK_6(mmap, void*, void*, size_t, int, int, int, off_t)
ADD_MOCK_2(munmap, int, void*, size_t)

ADD_MOCK_0(fork, pid_t)
ADD_MOCK_2(execv, int, const char*, char* const*)
void tmpexit(int);
void tmpexit_mock(int);
void tmpexit_unmock(void);
ADD_MOCK_NORET_1(tmpexit, int)
ADD_MOCK_3(waitpid, pid_t, pid_t, int*, int)

static bool exit_is_mocked;
static jmp_buf exit_jmp;
void
exit(int arg0) {
	if (exit_is_mocked) {
		longjmp(exit_jmp, arg0);
	}
	ADD_MOCK_LOOKUP(exit);
	(*(void (*)(int))real_exit)(arg0);
}

void
exit_mock(jmp_buf target) {
	exit_is_mocked = true;
	memcpy(exit_jmp, target, sizeof(jmp_buf));
}

void
exit_unmock(void) {
	exit_is_mocked = false;
}

#elif FOUNDATION_PLATFORM_WINDOWS




#endif
