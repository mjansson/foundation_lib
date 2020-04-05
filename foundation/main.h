/* main.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file main.h
\brief Main entry points

Main application entry points. The library implements the platform specific
entry points into the application (like <code>int main(...)</code> for normal
C library based applications) and handles the initial setup.

The foundation library then calls into these entry points to provide the
application with a unified single entry point for each application lifetime
event (initialize, run and finalize). */

#include <foundation/platform.h>
#include <foundation/types.h>

#if !BUILD_DYNAMIC_LINK

/*! Main initialization entry point. This must be implemented by the application.
The expectation is that this function will call foundation_initialize
to initialize the foundation library, and return an error code
to indicate success or failure. If this method returns an error initialization
will abort and the program will terminate.
\return 0 for success, <0 for error */
FOUNDATION_EXTERN int
main_initialize(void);

/*! Main loop. This must be implementation by the application and should
contain the main run loop. Once this function returns the application
will terminate. The return code is used as the process exit code
for normal termination. Will be called after #main_initialize
\param arg Unused, set to 0
\return Process exit code */
FOUNDATION_EXTERN int
main_run(void* arg);

/*! Main exit point. This must be implemented by the application.
The expectation is that this function will call foundation_finalize
to terminate the foundation library. Will be called after #main_run */
FOUNDATION_EXTERN void
main_finalize(void);

#endif
