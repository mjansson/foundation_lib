/* main.c  -  Foundation environment test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation/foundation.h>
#include <test/test.h>


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation environment tests";
	app.short_name = "test_environment";
	app.config_dir = "test_environment";
	return app;
}


DECLARE_TEST( environment, builtin )
{
	/*const char* const*     environment_command_line( void );
	const char*            environment_executable_name( void );
	const char*            environment_executable_directory( void );
	const char*            environment_initial_working_directory( void );
	const char*            environment_current_working_directory( void );
	void                   environment_set_current_working_directory( const char* path );
	const char*            environment_home_directory( void );
	const char*            environment_temporary_directory( void );
	const char*            environment_variable( const char* var );
	const application_t*   environment_application( void );*/

	return 0;
}


void test_declare( void )
{
	ADD_TEST( environment, builtin );
}
