/* apple.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file apple.h
    Safe inclusion of mach headers */

#include <foundation/platform.h>
#include <foundation/types.h>


#if FOUNDATION_PLATFORM_APPLE

#define semaphore_t __system_semaphore_t
#define radixsort __stdlib_radixsort
#define __error_t_defined 1

#define _UUID_T
#define uuid_generate_random __system_uuid_generate_random
#define uuid_generate_time __system_uuid_generate_time
#define uuid_is_null __system_uuid_is_null

#include <mach/mach_types.h>

#include <CoreFoundation/CoreFoundation.h>
#if FOUNDATION_PLATFORM_MACOSX
#include <Carbon/Carbon.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#undef radixsort
#undef semaphore_t

#undef _UUID_T
#undef uuid_generate_random
#undef uuid_generate_time
#undef uuid_is_null

#endif
