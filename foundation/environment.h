/* environment.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file environment.h
    Application environment */

#include <foundation/platform.h>
#include <foundation/types.h>


//! Get command line as a string array. Returned array of strings must not be modified or deallocated.
FOUNDATION_API const char* const*     environment_command_line( void );

//! Get executable name. Returned string must not be modified or deallocated.
FOUNDATION_API const char*            environment_executable_name( void );

//! Get executable directory. Returned string must not be modified or deallocated.
FOUNDATION_API const char*            environment_executable_directory( void );

//! Get executable full path. Returned string must not be modified or deallocated.
FOUNDATION_API const char*            environment_executable_path( void );

//! Get initial working directory. Returned string must not be modified or deallocated.
FOUNDATION_API const char*            environment_initial_working_directory( void );

//! Get current working directory. Returned string must not be modified or deallocated.
FOUNDATION_API const char*            environment_current_working_directory( void );

//! Set current working directory
FOUNDATION_API void                   environment_set_current_working_directory( const char* path );

//! Get user home directory. Returned string must not be modified or deallocated.
FOUNDATION_API const char*            environment_home_directory( void );

//! Get temporary directory. Returned string must not be modified or deallocated.
FOUNDATION_API const char*            environment_temporary_directory( void );

//! Get environment variable. Returned string must not be modified or deallocated.
FOUNDATION_API const char*            environment_variable( const char* var );

//! Get the application declaration
FOUNDATION_API const application_t*   environment_application( void );


#if FOUNDATION_PLATFORM_APPLE

//! Get the application bundle identifier
FOUNDATION_API void                   environment_bundle_identifier( char* buffer );

#endif
