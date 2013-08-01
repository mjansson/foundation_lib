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


void* event_thread( object_t thread, void* arg )
{
	event_block_t* block;
	event_t* event = 0;

	while( !thread_should_terminate( thread ) )
	{
		block = event_stream_process( system_event_stream() );

		while( ( event = event_next( block, 0 ) ) )
		{
			if( event->system == SYSTEM_FOUNDATION ) switch( event->id )
			{
				case FOUNDATIONEVENT_TERMINATE:
					log_warnf( WARNING_SUSPICIOUS, "Terminating tests due to event" );
					process_exit( -2 );
					break;

				default:
					break;
			}
		}

		thread_sleep( 10 );
	}

	return 0;
}


int main_initialize( void )
{
	application_t application = {0};
	application.name = "Foundation string test";
	application.short_name = "test_all";
	application.config_dir = "test_all";
	application.flags = APPLICATION_UTILITY;
	
	return foundation_initialize( memory_system_malloc(), application );
}


int main_run( void* main_arg )
{
	const char* pattern = 0;
	char** exe_paths = 0;
	unsigned int iexe, exesize;
	object_t library = 0;
	void* library_initialize = 0;
	void* library_run = 0;
	process_t* process = 0;
	char* process_path = 0;
	int process_result = 0;
	object_t thread = 0;
	
	thread = thread_create( event_thread, "event_thread", THREAD_PRIORITY_NORMAL, 0 );
	thread_start( thread, 0 );
	
	//Find all test executables in the current executable directory
#if FOUNDATION_PLATFORM_WINDOWS
	pattern = "test-*.exe";
#elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	pattern = "test-*";
#elif FOUNDATION_PLATFORM_ANDROID
	pattern = "libtest-*.so";
	log_infof( "Executable dir : %s", environment_executable_directory() );
	log_infof( "Executable name: %s", environment_executable_name() );
#elif FOUNDATION_PLATFORM_POSIX
	pattern = "test-*";
#else
#  error Not implemented
#endif
	exe_paths = fs_matching_files( environment_executable_directory(), pattern, false );
#if FOUNDATION_PLATFORM_ANDROID
	log_infof( "Found %d test libraries", array_size( exe_paths ) );
#endif
	for( iexe = 0, exesize = array_size( exe_paths ); iexe < exesize; ++iexe )
	{
#if FOUNDATION_PLATFORM_ANDROID
		log_infof( "Found test library: %s", exe_paths[iexe] );

		if( string_equal_substr( exe_paths[iexe], environment_executable_name(), string_length( environment_executable_name() ) ) )
			continue; //Don't run self
		
		library = library_load( exe_paths[iexe] );
		if( !library )
		{
			library_unload( library );
			log_warnf( WARNING_SUSPICIOUS, "Tests failed, unable to load test library" );
			process_set_exit_code( -1 );
			goto exit;
		}
		else
		{
			library_initialize = library_symbol( library, "main_initialize" );
			library_run = library_symbol( library, "main_run" );
			if( library_initialize && library_run )
			{
				int (*fn_initialize)(void) = library_initialize;
				int (*fn_run)(void*) = library_run;
				
				log_infof( "Running test library: %s", exe_paths[iexe] );
				process_result = fn_initialize();
				if( process_result >= 0 )
					process_result = fn_run( 0 );

				library_unload( library );

				if( process_result != 0 )
				{
					log_warnf( WARNING_SUSPICIOUS, "Tests failed with exit code %d", process_result );
					process_set_exit_code( -1 );
					goto exit;
				}
			}
			else
			{
				library_unload( library );
				log_warnf( WARNING_SUSPICIOUS, "Tests failed, test library does not export expected symbols" );
				process_set_exit_code( -1 );
				goto exit;
			}
		}
			
		library_unload( library );
#else
		if( string_equal_substr( exe_paths[iexe], environment_executable_name(), string_length( environment_executable_name() ) ) )
			continue; //Don't run self

		process_path = path_merge( environment_executable_directory(), exe_paths[iexe] );

		process = process_allocate();

		process_set_executable_path( process, process_path );
		process_set_working_directory( process, environment_executable_directory() );
		process_set_flags( process, PROCESS_ATTACHED );
		
		log_infof( "Running test executable: %s", exe_paths[iexe] );

		process_result = process_spawn( process );
		while( process_result == PROCESS_WAIT_INTERRUPTED )
		{
			thread_sleep( 10 );
			process_result = process_wait( process );
		}
		process_deallocate( process );

		string_deallocate( process_path );

		if( process_result != 0 )
		{
			if( process_result >= PROCESS_INVALID_ARGS )
				log_warnf( WARNING_SUSPICIOUS, "Tests failed, process terminated with error %x", process_result );
			else
				log_warnf( WARNING_SUSPICIOUS, "Tests failed with exit code %d", process_result );
			process_set_exit_code( -1 );
			goto exit;
		}
#endif

		log_infof( "All tests from %s passed (%d)", exe_paths[iexe], process_result );
	}

	log_infof( "All tests passed" );

exit:

	if( exe_paths )
		string_array_deallocate( exe_paths );
	
	thread_terminate( thread );
	thread_destroy( thread );
	while( thread_is_running( thread ) )
		thread_sleep( 10 );
	
	return process_result;
}


void main_shutdown( void )
{
	foundation_shutdown();
}

