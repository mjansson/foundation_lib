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

#include <foundation/platform.h>
#include <foundation/types.h>


FOUNDATION_API const string_const_t*  environment_command_line( void );
FOUNDATION_API string_const_t         environment_executable_name( void );
FOUNDATION_API string_const_t         environment_executable_directory( void );
FOUNDATION_API string_const_t         environment_executable_path( void );
FOUNDATION_API string_const_t         environment_initial_working_directory( void );
FOUNDATION_API string_const_t         environment_current_working_directory( void );
FOUNDATION_API void                   environment_set_current_working_directory( string_const_t path );
FOUNDATION_API string_const_t         environment_home_directory( void );
FOUNDATION_API string_const_t         environment_temporary_directory( void );
FOUNDATION_API string_const_t         environment_variable( string_const_t var );
FOUNDATION_API const application_t*   environment_application( void );


#if FOUNDATION_PLATFORM_APPLE

FOUNDATION_API void                   environment_bundle_identifier( string_t buffer );

#endif
