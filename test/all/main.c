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
	application.name = "Foundation library test suite";
	application.short_name = "test_all";
	application.config_dir = "test_all";
	application.flags = APPLICATION_UTILITY;
	
	return foundation_initialize( memory_system_malloc(), application );
}


#if FOUNDATION_PLATFORM_ANDROID
#  include <foundation/android.h>
extern int test_app_run( void );
extern int test_array_run( void );
extern int test_atomic_run( void );
extern int test_base64_run( void );
extern int test_blowfish_run( void );
extern int test_bufferstream_run( void );
extern int test_config_run( void );
extern int test_crash_run( void );
extern int test_environment_run( void );
extern int test_error_run( void );
extern int test_event_run( void );
extern int test_fs_run( void );
extern int test_hash_run( void );
extern int test_hashmap_run( void );
extern int test_hashtable_run( void );
extern int test_library_run( void );
extern int test_math_run( void );
extern int test_md5_run( void );
extern int test_mutex_run( void );
extern int test_objectmap_run( void );
extern int test_path_run( void );
extern int test_radixsort_run( void );
extern int test_random_run( void );
extern int test_ringbuffer_run( void );
extern int test_semaphore_run( void );
extern int test_stacktrace_run( void );
extern int test_string_run( void );
extern int test_uuid_run( void );
typedef int (*test_run_fn)( void );
#endif

int main_run( void* main_arg )
{
	const char* pattern = 0;
	char** exe_paths = 0;
	unsigned int iexe, exesize;
	process_t* process = 0;
	char* process_path = 0;
	int process_result = 0;
	object_t thread = 0;
	
	thread = thread_create( event_thread, "event_thread", THREAD_PRIORITY_NORMAL, 0 );
	thread_start( thread, 0 );

#if FOUNDATION_PLATFORM_ANDROID
	
	int test_fn = 0;
	test_run_fn tests[] = {
		//test_app_run
		test_array_run,
		test_atomic_run,
		test_base64_run,
		test_blowfish_run,
		test_bufferstream_run,
		test_config_run,
		test_crash_run,
		test_environment_run,
		test_error_run,
		test_event_run,
		test_fs_run,
		test_hash_run,
		test_hashmap_run,
		test_hashtable_run,
		test_library_run,
		test_math_run,
		test_md5_run,
		test_mutex_run,
		test_objectmap_run,
		test_path_run,
		test_radixsort_run,
		test_random_run,
		test_ringbuffer_run,
		test_semaphore_run,
		//test_stacktrace_run, 
		test_string_run,
		test_uuid_run,
		0
	};

	do
	{
		if( process_result >= 0 )
		{
			if( ( process_result = tests[test_fn]() ) >= 0 )
				log_infof( "All tests passed (%d)", process_result );
		}
		++test_fn;
	} while( tests[test_fn] && ( process_result >= 0 ) );

	if( process_result != 0 )
	{
		log_warnf( WARNING_SUSPICIOUS, "Tests failed with exit code %d", process_result );
	}
	
#else
	
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
	exe_paths = fs_matching_files( environment_executable_directory(), pattern, true );
	for( iexe = 0, exesize = array_size( exe_paths ); iexe < exesize; ++iexe )
	{
		bool is_self = false;
		char* exe_file_name = path_base_file_name( exe_paths[iexe] );
		if( string_equal( exe_file_name, environment_executable_name() ) )
			is_self = true;
		string_deallocate( exe_file_name );
		if( is_self )
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

		log_infof( "All tests from %s passed (%d)", exe_paths[iexe], process_result );
	}

	log_infof( "All tests passed" );

exit:

	if( exe_paths )
		string_array_deallocate( exe_paths );

#endif
	
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

