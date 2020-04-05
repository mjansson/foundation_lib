/* environment.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file environment.h
\brief Application environment

Application environment with access to command line, executable information, working
directory handling, standard path queries and environment variables.

Environment functions are thread safe unless noted otherwise. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Get command line as a string array. Returned array of strings must not be modified.
or deallocated.
\return Command line array */
FOUNDATION_API const string_const_t*
environment_command_line(void);

/*! Get executable name. Returned string must not be modified or deallocated.
\return Executable name */
FOUNDATION_API string_const_t
environment_executable_name(void);

/*! Get executable directory (directory containing the executable binary).
Returned string must not be modified or deallocated.
\return Executable directory */
FOUNDATION_API string_const_t
environment_executable_directory(void);

/*! Get executable full path. Returned string must not be modified or deallocated.
\return Executable path */
FOUNDATION_API string_const_t
environment_executable_path(void);

/*! Get initial working directory. Returned string must not be modified or deallocated.
\return Initial working directory */
FOUNDATION_API string_const_t
environment_initial_working_directory(void);

/*! Get current working directory. Returned string must not be modified or deallocated.
\return Current working directory */
FOUNDATION_API string_const_t
environment_current_working_directory(void);

/*! Change the current working directory.
\param path Path of new working directory
\param length Length of new working directory path
\return true if successful, false if error or not allowed by platform */
FOUNDATION_API bool
environment_set_current_working_directory(const char* path, size_t length);

/*! Get application storage directory. Returned string must not be modified or deallocated.
Application directory is platform specific and is a suitable path for preference and per-user
storage. \return User home directory */
FOUNDATION_API string_const_t
environment_application_directory(void);

/*! Get temporary directory. Returned string must not be modified or deallocated.
Temporary directory is platform specific and only guaranteed to exist during the execution
of the process.
\return Temporary directory */
FOUNDATION_API string_const_t
environment_temporary_directory(void);

/*! Get environment variable. Returned string must not be modified or deallocated.
\param var    Variable name
\param length Length of variable name
\return       Variable value */
FOUNDATION_API string_const_t
environment_variable(const char* var, size_t length);

/*! Get the application declaration as set by the application implementation.
\return Application declaration */
FOUNDATION_API const application_t*
environment_application(void);

#if FOUNDATION_PLATFORM_APPLE

/*! macOS and iOS only. Get the application bundle identifier.
\param buffer   Buffer receiving string
\param capacity Size of buffer in bytes
\return         Bundle identifier (stored in buffer) */
FOUNDATION_API string_t
environment_bundle_identifier(char* buffer, size_t capacity);

/*! macOS and iOS only. Get the application bundle directory.
 \param buffer   Buffer receiving string
 \param capacity Size of buffer in bytes
 \return         Bundle directory (stored in buffer) */
FOUNDATION_API string_t
environment_bundle_path(char* buffer, size_t capacity);

#endif
