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
#include <test/test.h>

volatile bool _test_should_start = false;
volatile bool _test_should_terminate = false;
volatile bool _test_have_focus = false;

static void* event_thread( object_t thread, void* arg )
{
	event_block_t* block;
	event_t* event = 0;
	FOUNDATION_UNUSED( arg );

	while( !thread_should_terminate( thread ) )
	{
		block = event_stream_process( system_event_stream() );
		event = 0;

		while( ( event = event_next( block, event ) ) )
		{
			switch( event->id )
			{
				case FOUNDATIONEVENT_START:
#if FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
					log_debugf( HASH_TEST, "Application start event received" );
					_test_should_start = true;
#endif
					break;

				case FOUNDATIONEVENT_TERMINATE:
#if FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
					log_debugf( HASH_TEST, "Application stop/terminate event received" );
					_test_should_terminate = true;
#else
					log_warn( HASH_TEST, WARNING_SUSPICIOUS, "Terminating tests due to event" );
					process_exit( -2 );
#endif
					break;

				case FOUNDATIONEVENT_FOCUS_GAIN:
					_test_have_focus = true;
					break;

				case FOUNDATIONEVENT_FOCUS_LOST:
					_test_have_focus = false;
					break;

				default:
					break;
			}
		}

		thread_sleep( 10 );
	}

	log_debugf( HASH_TEST, "Application event thread exiting" );

	return 0;
}


#if ( FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID ) && BUILD_ENABLE_LOG

#if FOUNDATION_PLATFORM_ANDROID
#include <foundation/android.h>
#include <android/native_activity.h>
#endif

#include <foundation/delegate.h>
#include <test/test.h>

static void test_log_callback( uint64_t context, int severity, const char* msg )
{
	FOUNDATION_UNUSED( context );
	FOUNDATION_UNUSED( severity );

	if( _test_should_terminate )
		return;

#if FOUNDATION_PLATFORM_IOS
	test_text_view_append( delegate_uiwindow(), 1 , msg );
#elif FOUNDATION_PLATFORM_ANDROID
	jclass _test_log_class = 0;
	jmethodID _test_log_append = 0;
	const struct JNINativeInterface** jnienv = thread_attach_jvm();
	_test_log_class = (*jnienv)->GetObjectClass( jnienv, android_app()->activity->clazz );
	if( _test_log_class )
		_test_log_append = (*jnienv)->GetMethodID( jnienv, _test_log_class, "appendLog", "(Ljava/lang/String;)V" );
	if( _test_log_append )
	{
		jstring jstr = (*jnienv)->NewStringUTF( jnienv, msg );
		(*jnienv)->CallVoidMethod( jnienv, android_app()->activity->clazz, _test_log_append, jstr );
		(*jnienv)->DeleteLocalRef( jnienv, jstr );
	}
	thread_detach_jvm();
#endif
}

#endif

#if !BUILD_MONOLITHIC

void test_crash_handler( const char* dump_file )
{
	FOUNDATION_UNUSED( dump_file );
	log_error( HASH_TEST, ERROR_EXCEPTION, "Test crashed" );
	process_exit( -1 );
}

#endif


int main_initialize( void )
{
	application_t application;
	memset( &application, 0, sizeof( application ) );
	application.name = "Foundation library test suite";
	application.short_name = "test_all";
	application.config_dir = "test_all";
	application.version = foundation_version();
	application.flags = APPLICATION_UTILITY;
	application.dump_callback = test_crash_handler;

	log_set_suppress( 0, ERRORLEVEL_INFO );

#if ( FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID ) && BUILD_ENABLE_LOG
	log_set_callback( test_log_callback );
#endif

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_ANDROID && !FOUNDATION_PLATFORM_PNACL

	_test_should_start = true;

#endif

	return foundation_initialize( memory_system_malloc(), application );
}


#if FOUNDATION_PLATFORM_ANDROID
#  include <foundation/android.h>
#endif

#if BUILD_MONOLITHIC
extern int test_app_run( void );
extern int test_array_run( void );
extern int test_atomic_run( void );
extern int test_base64_run( void );
extern int test_bitbuffer_run( void );
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
extern int test_pipe_run( void );
extern int test_profile_run( void );
extern int test_radixsort_run( void );
extern int test_random_run( void );
extern int test_regex_run( void );
extern int test_ringbuffer_run( void );
extern int test_semaphore_run( void );
extern int test_stacktrace_run( void );
extern int test_stream_run( void );
extern int test_string_run( void );
extern int test_system_run( void );
extern int test_time_run( void );
extern int test_uuid_run( void );
typedef int (*test_run_fn)( void );

static void* test_runner( object_t obj, void* arg )
{
	FOUNDATION_UNUSED( obj );
	test_run_fn* tests = (test_run_fn*)arg;
	int test_fn = 0;
	int process_result = 0;

	while( tests[test_fn] && ( process_result >= 0 ) )
	{
		if( ( process_result = tests[test_fn]() ) >= 0 )
			log_infof( HASH_TEST, "All tests passed (%d)", process_result );
		++test_fn;
	}

	return (void*)(intptr_t)process_result;
}

#endif

int main_run( void* main_arg )
{
#if !BUILD_MONOLITHIC
	const char* pattern = 0;
	char** exe_paths = 0;
	unsigned int iexe, exesize;
	process_t* process = 0;
	char* process_path = 0;
	unsigned int* exe_flags = 0;
#endif
#if FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_PNACL
	int remain_counter = 0;
#endif
#if BUILD_DEBUG
	const char* build_name = "debug";
#elif BUILD_RELEASE
	const char* build_name = "release";
#elif BUILD_PROFILE
	const char* build_name = "profile";
#elif BUILD_DEPLOY
	const char* build_name = "deploy";
#endif
	int process_result = 0;
	object_t thread = 0;
	FOUNDATION_UNUSED( main_arg );
	FOUNDATION_UNUSED( build_name );

	log_set_suppress( HASH_TEST, ERRORLEVEL_DEBUG );

	log_infof( HASH_TEST, "Foundation library v%s built for %s using %s (%s)", string_from_version_static( foundation_version() ), FOUNDATION_PLATFORM_DESCRIPTION, FOUNDATION_COMPILER_DESCRIPTION, build_name );

	thread = thread_create( event_thread, "event_thread", THREAD_PRIORITY_NORMAL, 0 );
	thread_start( thread, 0 );
	while( !thread_is_running( thread ) )
		thread_sleep( 10 );

#if FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
	while( !_test_should_start )
	{
#if FOUNDATION_PLATFORM_ANDROID
		system_process_events();
#endif
		thread_sleep( 100 );
	}
#endif

	fs_remove_directory( environment_temporary_directory() );

#if BUILD_MONOLITHIC

	test_run_fn tests[] = {
		test_app_run,
		test_array_run,
		test_atomic_run,
		test_base64_run,
		test_bitbuffer_run,
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
		test_pipe_run,
		test_profile_run,
		test_radixsort_run,
		test_random_run,
		test_regex_run,
		test_ringbuffer_run,
		test_semaphore_run,
		test_stacktrace_run,
		test_stream_run, //stream test closes stdin
		test_string_run,
		test_system_run,
		test_time_run,
		test_uuid_run,
		0
	};

#if FOUNDATION_PLATFORM_ANDROID

	object_t test_thread = thread_create( test_runner, "test_runner", THREAD_PRIORITY_NORMAL, 0 );
	thread_start( test_thread, tests );

	log_debugf( HASH_TEST, "Starting test runner thread" );

	while( !thread_is_running( test_thread ) )
	{
		system_process_events();
		thread_sleep( 10 );
	}

	while( thread_is_running( test_thread ) )
	{
		system_process_events();
		thread_sleep( 10 );
	}

	process_result = (int)(intptr_t)thread_result( test_thread );
	thread_destroy( test_thread );

	while( thread_is_thread( test_thread ) )
	{
		system_process_events();
		thread_sleep( 10 );
	}

#else

	process_result = (int)(intptr_t)test_runner( 0, tests );

#endif

	if( process_result != 0 )
		log_warnf( HASH_TEST, WARNING_SUSPICIOUS, "Tests failed with exit code %d", process_result );

#if FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_PNACL

	while( !_test_should_terminate && _test_have_focus && ( remain_counter < 50 ) )
	{
		system_process_events();
		thread_sleep( 100 );
		++remain_counter;
	}

#endif

	log_debug( HASH_TEST, "Exiting main loop" );

#else // !BUILD_MONOLITHIC

	//Find all test executables in the current executable directory
#if FOUNDATION_PLATFORM_WINDOWS
	pattern = "^test-.*\\.exe$";
#elif FOUNDATION_PLATFORM_MACOSX
	pattern = "^test-.*$";
#elif FOUNDATION_PLATFORM_POSIX
	pattern = "^test-.*$";
#else
#  error Not implemented
#endif
	exe_paths = fs_matching_files( environment_executable_directory(), pattern, false );
	array_resize( exe_flags, array_size( exe_paths ) );
	memset( exe_flags, 0, sizeof( unsigned int ) * array_size( exe_flags ) );
#if FOUNDATION_PLATFORM_MACOSX
	//Also search for test applications
	const char* app_pattern = "^test-.*\\.app$";
	regex_t* app_regex = regex_compile( app_pattern );
	char** subdirs = fs_subdirs( environment_executable_directory() );
	for( int idir = 0, dirsize = array_size( subdirs ); idir < dirsize; ++idir )
	{
		if( regex_match( app_regex, subdirs[idir], string_length( subdirs[idir] ), 0, 0 ) )
		{
			array_push( exe_paths, string_substr( subdirs[idir], 0, string_length( subdirs[idir] ) - 4 ) );
			array_push( exe_flags, PROCESS_OSX_USE_OPENAPPLICATION );
		}
	}
	string_array_deallocate( subdirs );
	regex_deallocate( app_regex );
#endif
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
		process_set_flags( process, PROCESS_ATTACHED | exe_flags[iexe] );

		log_infof( HASH_TEST, "Running test executable: %s", exe_paths[iexe] );

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
				log_warnf( HASH_TEST, WARNING_SUSPICIOUS, "Tests failed, process terminated with error %x", process_result );
			else
				log_warnf( HASH_TEST, WARNING_SUSPICIOUS, "Tests failed with exit code %d", process_result );
			process_set_exit_code( -1 );
			goto exit;
		}

		log_infof( HASH_TEST, "All tests from %s passed (%d)", exe_paths[iexe], process_result );
	}

	log_info( HASH_TEST, "All tests passed" );

exit:

	if( exe_paths )
		string_array_deallocate( exe_paths );
	array_deallocate( exe_flags );

#endif

	thread_terminate( thread );
	thread_destroy( thread );
	while( thread_is_running( thread ) )
		thread_sleep( 10 );
	while( thread_is_thread( thread ) )
		thread_sleep( 10 );

	log_infof( HASH_TEST, "Tests exiting: %d", process_result );

	return process_result;
}


void main_shutdown( void )
{
#if FOUNDATION_PLATFORM_ANDROID
	thread_detach_jvm();
#endif

	foundation_shutdown();
}

