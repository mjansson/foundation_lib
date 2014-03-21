/* main.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file main.h
    Application entry points */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Main entry point. This must be implemented by the application.
    The expectation is that this function will call foundation_initialize
    to initialize the foundation library, and return an error code
    to indicate success or failure
    \return        0 for success, <0 for error */
extern int         main_initialize( void );

/*! Main loop. This must be implementation by the application and should
    contain the main run loop. Once this function returns the application
    will terminate. The return code is used as the process exit code
    for normal termination. Will be called after main_initialize()
    \param arg     Unused, set to 0
    \return        Process exit code */
extern int         main_run( void* arg );

/*! Main exit point. This must be implemented by the application.
    The expectation is that this function will call foundation_shutdown
    to terminate the foundation library. Will be called after main_run() */
extern void        main_shutdown( void );
