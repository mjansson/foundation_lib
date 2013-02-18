/* main.c  -  Foundation test launcher  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


int main_initialize( void )
{
	application_t application = {0};
	application.name = "Foundation string test";
	application.short_name = "test_string";
	application.config_dir = "test_string";
	
	return foundation_initialize( memory_system_malloc(), application );
}


int main_run( void* main_arg )
{
	const char* pattern = 0;
	char** exe_paths = 0;
	unsigned int iexe, exesize;
	process_t* process = 0;
	char* process_path = 0;
	int process_result = 0;

	//Find all test executables in the current executable directory
#if FOUNDATION_PLATFORM_WINDOWS
	pattern = "test-*.exe";
#elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	pattern = "test-*";
#elif FOUNDATION_PLATFORM_POSIX
	pattern = "test-*";
#else
#  error Not implemented
#endif
	exe_paths = fs_matching_files( environment_executable_directory(), pattern, false );
	for( iexe = 0, exesize = array_size( exe_paths ); iexe < exesize; ++iexe )
	{
		if( string_equal( exe_paths[iexe], environment_executable_name() ) )
			continue; //Don't run self

		process_path = path_merge( environment_executable_directory(), exe_paths[iexe] );

		process = process_allocate();

		process_set_executable_path( process, process_path );
		process_set_working_directory( process, environment_executable_directory() );
		process_set_flags( process, PROCESS_ATTACHED );
		
		log_infof( "Running test executable: %s (%s)", exe_paths[iexe], environment_executable_name() );

		process_result = process_spawn( process );
		process_deallocate( process );

		string_deallocate( process_path );

		if( process_result < 0 )
		{
			log_warnf( WARNING_SUSPICIOUS, "Tests failed with exit code %d", process_result );
			goto exit;
		}
	}

	log_infof( "All tests passed" );

exit:

	return process_result;
}


void main_shutdown( void )
{
	foundation_shutdown();
}

