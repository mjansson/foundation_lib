/* tizen.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file tizen.h
\brief Tizen platform safe header inclusion and entry points

Tizen platform safe header inclusion and entry points */

#include <foundation/platform.h>

#if FOUNDATION_PLATFORM_TIZEN

#ifndef __error_t_defined
#define __error_t_defined 1
#endif

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <appfw/app.h>
#include <system/system_settings.h>
#include <dlog/dlog.h>

/*! Initialize Tizen app
\return 0 if success, <0 if error */
FOUNDATION_API int
tizen_initialize(void);

/*! Finalize Tizen app */
FOUNDATION_API void
tizen_finalize(void);

/*! Start main application thread */
FOUNDATION_API void
tizen_start_main_thread(void);

/*! Main entry point
\param argc Argument count
\param argb Argument array
\return Application return code */
FOUNDATION_API int
tizen_app_main(int argc, char** argv);

#endif
