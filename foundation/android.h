/* android.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file android.h
\brief Android system header

Safe inclusion of Android specific headers. Application glue entry and exit
functions.

The Android implementation of foundation library is built on the Android NDK
and requires the android_native_app_glue and cpu-features NDK libraries, which will be
automatically included into the foundation library sources. If you use
another build system these libraries must NOT be included in the build. */

#include <foundation/platform.h>

#if FOUNDATION_PLATFORM_ANDROID

#if FOUNDATION_COMPILER_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#if __has_warning("-Wsign-conversion")
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif
#if __has_warning("-Wundef")
#pragma clang diagnostic ignored "-Wundef"
#endif
#if __has_warning("-Wpedantic")
#pragma clang diagnostic ignored "-Wpedantic"
#endif
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#endif

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <android_native_app_glue.h>

#if FOUNDATION_COMPILER_GCC
#pragma GCC diagnostic pop
#endif

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

/*! Entry point for android native applications. Called internally at process launch,
unless you write your own main wrapper you do not need to call this function.
\param app Native glue application data */
FOUNDATION_API void
android_entry(struct android_app* app);

/*! Initialize android integration, such as initial event processing and sensor setup.
Called internally after main_initialize(), unless you write your own main wrapper
you do not need to call this function.
\return 0 on success, <0 if error */
FOUNDATION_API int
android_initialize(void);

/*! Shutdown and cleanup android integration. Called internally after #main_finalize,
unless you write your own main wrapper you do not need to call this function. */
FOUNDATION_API void
android_finalize(void);

/*! Access the native glue application data. See the Android NDK documentation and
headers for details about this structure.
\return Native glue application data */
FOUNDATION_API struct android_app*
android_app(void);

/*! Process callback for native application commands. Unless you write your own
application wrapper you do not need to call or use this function.
\param app Native glue application data
\param cmd Application command */
FOUNDATION_API void
android_handle_cmd(struct android_app* app, int32_t cmd);

/*! Process callback for sensor events. Unless you write your own application
wrapper you do not need to call or use this function.
\param fd      Associated file descriptor
\param events  Events bitmask
\param data    Event data
\return        1, to continue propagation */
FOUNDATION_API int
android_sensor_callback(int fd, int events, void* data);

#endif
