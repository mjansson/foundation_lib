/* exception.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file exception.h
\brief Exception handling

Exception handling for structured exception handling and signals. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Guard the given method and generate a dump if the function generates an exception,
such as a termination signal or structured exception.
\param fn       Function to wrap
\param data     Argument
\param handler  Exception handler
\param name     Dump name prefix
\param length   Length of dump name prefix
\return         Function return value, FOUNDATION_EXCEPTION_DUMP_GENERATED if dump was generated */
FOUNDATION_API int
exception_try(exception_try_fn fn, void* data, exception_handler_fn handler, const char* name, size_t length);

/*! Set the global exception handler and dump name prefix. This will be used or all new
threads created.
\param handler  Exception handler
\param name     Dump name prefix
\param length   Length of dump name prefix */
FOUNDATION_API void
exception_set_handler(exception_handler_fn handler, const char* name, size_t length);

/*! Get current global exception handler
\return Exception handler function */
FOUNDATION_API exception_handler_fn
exception_handler(void);

/*! Get current global dump name prefix
\return Dump name prefix */
FOUNDATION_API string_const_t
exception_dump_name(void);

/*! Raise a debug break exception that will be caught by any attached debugger */
FOUNDATION_API void
exception_raise_debug_break(void) FOUNDATION_ATTRIBUTE(noreturn);

/*! Raise exception by performing an illegal instruction, causing the process to
core dump */
FOUNDATION_API void
exception_raise_abort(void) FOUNDATION_ATTRIBUTE(noreturn);
