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
	app.name = "Foundation process tests";
	app.short_name = "test_process";
	app.config_dir = "test_process";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_process_memory_system( void )
{
	return memory_system_malloc();
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
	const char* prog = environment_variable( "comspec" );
	const char* args[] = { "/C", "dir", 0 };
#elif FOUNDATION_PLATFORM_POSIX
	const char* prog = "/bin/ls";
	const char* args[] = { "-l", "-a", 0 };
#else
	const char* prog = "notimplemented";
	const char* args[] = { "", "", 0 };
#endif
	char* tmp_path;
	char* full_path;
	stream_t* tmp_file;
	const char* file_name = "test-file";

	if( ( system_platform() == PLATFORM_IOS ) || ( system_platform() == PLATFORM_ANDROID ) || ( system_platform() == PLATFORM_PNACL ) )
		return 0;

	tmp_path = path_merge( environment_temporary_directory(), "path with space " );
	tmp_path = string_append( tmp_path, string_from_uint_static( (uint32_t)random32(), false, 0, '0' ) );

	EXPECT_TRUE( fs_make_directory( tmp_path ) );

	full_path = path_merge( tmp_path, file_name );
	tmp_file = fs_open_file( full_path, STREAM_CREATE | STREAM_OUT );
	EXPECT_NE( tmp_file, 0 );
	stream_deallocate( tmp_file );

	args[2] = tmp_path;

	proc = process_allocate();

	process_set_working_directory( proc, "/" );
	process_set_executable_path( proc, prog );
	process_set_arguments( proc, args, sizeof( args ) / sizeof( args[0] ) );
	process_set_flags( proc, PROCESS_DETACHED | PROCESS_CONSOLE | PROCESS_STDSTREAMS );
	process_set_verb( proc, "open" );

	ret = process_spawn( proc );
	EXPECT_INTEQ( ret, PROCESS_STILL_ACTIVE );

	out = process_stdout( proc );
	in = process_stdin( proc );

	EXPECT_NE( out, 0 );
	EXPECT_NE( in, 0 );

	stream_write_string( in, "testing" );

	found_expected = false;
	found_file = false;
	num_lines = 0;
	do
	{
		stream_read_line_buffer( out, line_buffer, 512, '\n' );
		string_strip( line_buffer, "\n\r" );
		if( string_length( line_buffer ) )
		{
			++num_lines;
#if FOUNDATION_PLATFORM_WINDOWS
			if( ( string_find_string( line_buffer, "File(s)", 0 ) != STRING_NPOS ) && ( string_find_string( line_buffer, "bytes", 0 ) != STRING_NPOS ) )
				found_expected = true;
#else
			if( ( string_find_string( line_buffer, "drwx", 0 ) != STRING_NPOS ) && ( string_find_string( line_buffer, "..", 0 ) != STRING_NPOS ) )
				found_expected = true;
#endif
			if( string_find_string( line_buffer, file_name, 0 ) != STRING_NPOS )
				found_file = true;
			log_debugf( HASH_TEST, "%s", line_buffer );
		}
	} while( !stream_eos( out ) );

	EXPECT_GE( num_lines, 4 );
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

	args[0] = tmp_path;

	process_set_working_directory( proc, "/" );
	process_set_executable_path( proc, "/System/Library/CoreServices/Finder.app" );
	process_set_flags( proc, PROCESS_DETACHED | PROCESS_MACOSX_USE_OPENAPPLICATION );
	process_set_arguments( proc, args, 1 );

	ret = process_spawn( proc );
	EXPECT_INTEQ( ret, PROCESS_STILL_ACTIVE );

#endif

	process_deallocate( proc );

	process_set_exit_code( -1 );
	EXPECT_EQ( process_exit_code(), -1 );
	process_set_exit_code( 0 );

	string_deallocate( tmp_path );
	string_deallocate( full_path );

	return 0;
}


static void test_process_declare( void )
{
	ADD_TEST( process, spawn );
}


test_suite_t test_process_suite = {
	test_process_application,
	test_process_memory_system,
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
