/* posix.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

/*! \file posix
    Safe inclusion of posix headers */

#include <foundation/platform.h>
#include <foundation/types.h>


#if FOUNDATION_PLATFORM_POSIX

#define radixsort __stdlib_radixsort
#define __error_t_defined 1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sched.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <sys/types.h>
#include <sys/wait.h>

#undef radixsort

#endif
