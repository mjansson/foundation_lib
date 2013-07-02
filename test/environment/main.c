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
	app.flags = APPLICATION_UTILITY;
	return app;
}


int test_initialize( void )
{
	return 0;
}


void test_shutdown( void )
{
}


DECLARE_TEST( environment, builtin )
{
	char const* const* cmdline = environment_command_line();

	EXPECT_GE( array_size( cmdline ), 1 );
	EXPECT_NE( string_find_string( cmdline[0], "test-environment", 0 ), STRING_NPOS );

	EXPECT_STREQ( environment_executable_name(), "test-environment" );
	EXPECT_NE( environment_initial_working_directory(), 0 );
	EXPECT_NE( string_length( environment_initial_working_directory() ), 0 );
	EXPECT_STREQ( environment_initial_working_directory(), environment_current_working_directory() );
	
	EXPECT_NE( environment_home_directory(), 0 );
	EXPECT_NE( string_length( environment_home_directory() ), 0 );

	EXPECT_NE( environment_temporary_directory(), 0 );
	EXPECT_NE( string_length( environment_temporary_directory() ), 0 );

	EXPECT_NE( environment_variable( "PATH" ), 0 );
	EXPECT_NE( string_length( environment_variable( "PATH" ) ), 0 );
	
	return 0;
}

DECLARE_TEST( environment, workingdir )
{
	const char* working_dir = environment_current_working_directory();

	char* new_working_dir = path_path_name( working_dir );

	environment_set_current_working_directory( new_working_dir );
	EXPECT_STREQ( environment_current_working_directory(), new_working_dir );

	environment_set_current_working_directory( working_dir );	
	EXPECT_STREQ( environment_current_working_directory(), working_dir );

	string_deallocate( new_working_dir );
	
	return 0;
}


void test_declare( void )
{
	ADD_TEST( environment, builtin );
	ADD_TEST( environment, workingdir );
}
