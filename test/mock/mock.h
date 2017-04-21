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

#if FOUNDATION_PLATFORM_POSIX

#include <sys/mman.h>
#include <unistd.h>
#include <setjmp.h>

void
mmap_mock(void* return_value, int err);

void
mmap_unmock(void);

void
munmap_mock(int return_value, int err);

void
munmap_unmock(void);

void
fork_mock(pid_t return_value, int err);

void
fork_unmock(void);

void
waitpid_mock(pid_t return_value, int err);

void
waitpid_unmock(void);

void
execv_mock(int return_value, int err);

void
execv_unmock(void);

void
dup2_mock(int return_value, int err);

void
dup2_unmock(void);

void
exit_mock(jmp_buf target);

void
exit_unmock(void);

#elif FOUNDATION_PLATFORM_WINDOWS


#endif
