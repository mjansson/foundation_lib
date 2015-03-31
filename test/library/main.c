/* main.c  -  Foundation library test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_library_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = "Foundation library tests";
	app.short_name = "test_library";
	app.config_dir = "test_library";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_library_memory_system( void )
{
	return memory_system_malloc();
}


static int test_library_initialize( void )
{
	return 0;
}


static void test_library_shutdown( void )
{
}


DECLARE_TEST( library, lookup )
{
	object_t lib = 0;
	object_t otherlib = 0;
	void* symbol = 0;
	const char* libraryname;
	const char* symbolname;

	if( system_platform() == PLATFORM_PNACL )
		return 0;

#if FOUNDATION_PLATFORM_WINDOWS
	libraryname = "kernel32";
	symbolname = "ExitProcess";
#elif FOUNDATION_PLATFORM_APPLE
	libraryname = "dl";
	symbolname = "dlsym";
#elif FOUNDATION_PLATFORM_BSD
	libraryname = "execinfo";
	symbolname = "backtrace";
#elif FOUNDATION_PLATFORM_POSIX
	libraryname = "dl";
	symbolname = "dlsym";
#else
	libraryname = "somelib";
	symbolname = "somesym";
#endif

	lib = library_load( libraryname );
	EXPECT_NE( lib, 0 );

	otherlib = library_load( libraryname );
	EXPECT_EQ( lib, otherlib );

	library_unload( otherlib );
	library_unload( 0 );
	otherlib = 0;

	log_set_suppress( 0, ERRORLEVEL_WARNING );
	EXPECT_EQ( library_load( "this_library_should_not_exist" ), 0 );
	log_set_suppress( 0, ERRORLEVEL_DEBUG );

	EXPECT_TRUE( library_valid( lib ) );
	EXPECT_FALSE( library_valid( 0 ) );

	symbol = library_symbol( lib, symbolname );
	EXPECT_NE( symbol, 0 );

	EXPECT_EQ( library_symbol( 0, symbolname ), 0 );

	library_unload( lib );
	EXPECT_EQ( library_symbol( lib, symbolname ), 0 );

	EXPECT_FALSE( library_valid( lib ) );

	return 0;
}


static void test_library_declare( void )
{
	ADD_TEST( library, lookup );
}


test_suite_t test_library_suite = {
	test_library_application,
	test_library_memory_system,
	test_library_declare,
	test_library_initialize,
	test_library_shutdown
};


#if BUILD_MONOLITHIC

int test_library_run( void );
int test_library_run( void )
{
	test_suite = test_library_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_library_suite;
}

#endif
