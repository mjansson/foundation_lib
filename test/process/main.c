/* main.c  -  Foundation time test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_process_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = string_const( STRING_CONST( "Foundation process tests" ) );
	app.short_name = string_const( STRING_CONST( "test_process" ) );
	app.config_dir = string_const( STRING_CONST( "test_process" ) );
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_process_memory_system( void )
{
	return memory_system_malloc();
}


static foundation_config_t test_process_config( void )
{
	foundation_config_t config;
	memset( &config, 0, sizeof( config ) );
	return config;
}


static int test_process_initialize( void )
{
	return 0;
}


static void test_process_shutdown( void )
{
}


DECLARE_TEST( process, spawn )
{
	process_t* proc;
	stream_t* in;
	stream_t* out;
	int exit_code, ret, num_lines;
	bool found_expected;
	bool found_file;
	char line_buffer[512];
#if FOUNDATION_PLATFORM_WINDOWS
	string_const_t prog = environment_variable( "comspec" );
	string_const_t args[] = { string_const( STRING_CONST( "/C" ) ), string_const( STRING_CONST( "dir" ) ), string_null() };
#elif FOUNDATION_PLATFORM_POSIX
	string_const_t prog = string_const( STRING_CONST( "/bin/ls" ) );
	string_const_t args[] = { string_const( STRING_CONST( "-1" ) ), string_const( STRING_CONST( "-la" ) ), string_null() };
#else
	string_const_t prog = string_const( STRING_CONST( "notimplemented" ) );
	string_const_t args[] = { string_const( STRING_CONST( "" ) ), string_const( STRING_CONST( "" ) ), string_null() };
#endif
	string_t tmp_path;
	string_t full_path;
	stream_t* tmp_file;
	string_const_t fname;
	string_const_t file_name = string_const( STRING_CONST( "test-file" ) );

	if( ( system_platform() == PLATFORM_IOS ) || ( system_platform() == PLATFORM_ANDROID ) || ( system_platform() == PLATFORM_PNACL ) )
		return 0;

	fname = string_from_uint_static( (uint32_t)random32(), false, 0, '0' );
	tmp_path = path_allocate_concat_varg( STRING_ARGS( environment_temporary_directory() ), STRING_CONST( "path with space " ),
	  STRING_ARGS( fname ), nullptr );

	EXPECT_TRUE( fs_make_directory( STRING_ARGS( tmp_path ) ) );

	full_path = path_allocate_concat( STRING_ARGS( tmp_path ), STRING_ARGS( file_name ) );
	tmp_file = fs_open_file( STRING_ARGS( full_path ), STREAM_CREATE | STREAM_OUT );
	EXPECT_NE( tmp_file, 0 );
	stream_deallocate( tmp_file );

	args[2] = string_to_const( tmp_path );

	proc = process_allocate();

	process_set_working_directory( proc, STRING_CONST( "/" ) );
	process_set_executable_path( proc, STRING_ARGS( prog ) );
	process_set_arguments( proc, args, sizeof( args ) / sizeof( args[0] ) );
	process_set_flags( proc, PROCESS_DETACHED | PROCESS_CONSOLE | PROCESS_STDSTREAMS );
	process_set_verb( proc, STRING_CONST( "open" ) );

	ret = process_spawn( proc );
	EXPECT_INTEQ( ret, PROCESS_STILL_ACTIVE );

	out = process_stdout( proc );
	in = process_stdin( proc );

	EXPECT_NE( out, 0 );
	EXPECT_NE( in, 0 );

	stream_write_string( in, STRING_CONST( "testing" ) );

	found_expected = false;
	found_file = false;
	num_lines = 0;
	do
	{
		string_t fline = stream_read_line_buffer( out, line_buffer, 512, '\n' );
		string_const_t line = string_strip( STRING_ARGS( fline ), STRING_CONST( "\n\r" ) );
		if( line.length )
		{
			++num_lines;
#if FOUNDATION_PLATFORM_WINDOWS
			if( ( string_find_string( STRING_ARGS( line ), STRING_CONST( "File(s)" ), 0 ) != STRING_NPOS ) && ( string_find_string( STRING_ARGS( line ), STRING_CONST( "bytes" ), 0 ) != STRING_NPOS ) )
				found_expected = true;
#else
			if( ( string_find_string( STRING_ARGS( line ), STRING_CONST( "drwx" ), 0 ) != STRING_NPOS ) && ( string_find_string( STRING_ARGS( line ), STRING_CONST( ".." ), 0 ) != STRING_NPOS ) )
				found_expected = true;
#endif
			if( string_find_string( STRING_ARGS( line ), STRING_ARGS( file_name ), 0 ) != STRING_NPOS )
				found_file = true;
			log_debugf( HASH_TEST, STRING_CONST( "%.*s" ), STRING_FORMAT( fline ) );
		}
	} while( !stream_eos( out ) );

	EXPECT_INTGE( num_lines, 4 );
	EXPECT_TRUE( found_expected );
	EXPECT_TRUE( found_file );

	do
	{
		exit_code = process_wait( proc );
	} while( exit_code == PROCESS_STILL_ACTIVE );

	EXPECT_EQ( exit_code, 0 );

#if FOUNDATION_PLATFORM_WINDOWS

	// PROCESS_WINDOWS_USE_SHELLEXECUTE

#endif

#if FOUNDATION_PLATFORM_MACOSX

	process_finalize( proc );
	process_initialize( proc );

	args[0] = string_to_const( tmp_path );

	process_set_working_directory( proc, STRING_CONST( "/" ) );
	process_set_executable_path( proc, STRING_CONST( "/System/Library/CoreServices/Finder.app" ) );
	process_set_flags( proc, PROCESS_DETACHED | PROCESS_MACOSX_USE_OPENAPPLICATION );
	process_set_arguments( proc, args, 1 );

	ret = process_spawn( proc );
	EXPECT_INTEQ( ret, PROCESS_STILL_ACTIVE );

#endif

	process_deallocate( proc );

	process_set_exit_code( -1 );
	EXPECT_EQ( process_exit_code(), -1 );
	process_set_exit_code( 0 );

	string_deallocate( tmp_path.str );
	string_deallocate( full_path.str );

	return 0;
}


static void test_process_declare( void )
{
	ADD_TEST( process, spawn );
}


static test_suite_t test_process_suite = {
	test_process_application,
	test_process_memory_system,
	test_process_config,
	test_process_declare,
	test_process_initialize,
	test_process_shutdown
};


#if BUILD_MONOLITHIC

int test_process_run( void );
int test_process_run( void )
{
	test_suite = test_process_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_process_suite;
}

#endif
