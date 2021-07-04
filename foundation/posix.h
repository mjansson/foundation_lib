/* posix.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#pragma once

/*! \file posix.h
\brief Safe inclusion of posix headers

Safe inclusion of posix headers */

#include <foundation/platform.h>
#include <foundation/types.h>

#if FOUNDATION_PLATFORM_POSIX

#if FOUNDATION_COMPILER_GCC
#pragma GCC diagnostic push
#if FOUNDATION_GCC_VERSION > 40700
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#elif FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#if __has_warning("-Wpedantic")
#pragma clang diagnostic ignored "-Wpedantic"
#endif
#if __has_warning("-Wundef")
#pragma clang diagnostic ignored "-Wundef"
#endif
#if __has_warning("-Wpacked")
#pragma clang diagnostic ignored "-Wpacked"
#endif
#endif

#define radixsort __stdlib_radixsort
#ifndef __error_t_defined
#define __error_t_defined 1
#endif

#if FOUNDATION_PLATFORM_APPLE
#define _UUID_T
#define _UUID_UUID_H
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sched.h>
#include <pwd.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/time.h>

#if FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
#include <sys/eventfd.h>
#include <sys/epoll.h>
#endif

#if !FOUNDATION_PLATFORM_ANDROID
#include <execinfo.h>
#endif

#undef radixsort

#if FOUNDATION_PLATFORM_APPLE
#undef _UUID_T
#undef _UUID_UUID_H
#endif

#if FOUNDATION_COMPILER_GCC
#pragma GCC diagnostic pop
#elif FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

#endif
