/* foundation.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <foundation/internal.h>


//Make artificial reference to main entry point
#if FOUNDATION_PLATFORM_ANDROID
struct android_app;
extern void android_main( struct android_app* );
#elif FOUNDATION_PLATFORM_PNACL
#include <foundation/pnacl.h>
FOUNDATION_EXTERN PP_EXPORT int32_t PPP_InitializeModule( PP_Module module_id, PPB_GetInterface get_browser );
FOUNDATION_EXTERN PP_EXPORT const void* PPP_GetInterface( const char* interface_name );
FOUNDATION_EXTERN PP_EXPORT void PPP_ShutdownModule();
#else
extern int main( int, char** );
#endif

foundation_config_t _foundation_def;

static bool _foundation_initialized;


static void foundation_initialize_config( const foundation_config_t config )
{
	_foundation_def.thread_max             = config.thread_max            ? config.thread_max            : 128;
	_foundation_def.library_max            = config.library_max           ? config.library_max           : 32;
	_foundation_def.memory_tracker_max     = config.memory_tracker_max    ? config.memory_tracker_max    : ( 32 * 1024 );
	_foundation_def.temporary_memory       = config.temporary_memory      ? config.temporary_memory      : ( 512 * 1024 );
	_foundation_def.fs_monitor_max         = config.fs_monitor_max        ? config.fs_monitor_max        : 16;
	_foundation_def.error_context_depth    = config.error_context_depth   ? config.error_context_depth   : 32;
	_foundation_def.memory_context_depth   = config.memory_context_depth  ? config.memory_context_depth  : 32;
	_foundation_def.stacktrace_depth       = config.stacktrace_depth      ? config.stacktrace_depth      : 32;
	_foundation_def.hash_store_size        = config.hash_store_size;
	_foundation_def.event_block_chunk      = config.event_block_chunk     ? config.event_block_chunk     : ( 8 * 1024 );
	_foundation_def.event_block_limit      = config.event_block_limit     ? config.event_block_limit     : ( 512 * 1024 );
	_foundation_def.thread_stack_size      = config.thread_stack_size     ? config.thread_stack_size     : 0x8000;
}


#define SUBSYSTEM_INIT( system ) if( ret == 0 ) ret = _##system##_initialize()
#define SUBSYSTEM_INIT_ARGS( system, ... ) if( ret == 0 ) ret = _##system##_initialize( __VA_ARGS__ )

int foundation_initialize( const memory_system_t memory, const application_t application, const foundation_config_t config )
{
	int ret = 0;

	if( _foundation_initialized )
		return 0;

	foundation_initialize_config( config );

	SUBSYSTEM_INIT( atomic );
	SUBSYSTEM_INIT_ARGS( memory, memory );
	SUBSYSTEM_INIT( static_hash );
	SUBSYSTEM_INIT( log );
	SUBSYSTEM_INIT( time );
	SUBSYSTEM_INIT( thread );
	SUBSYSTEM_INIT( random );
	SUBSYSTEM_INIT( stream );
	SUBSYSTEM_INIT( fs );
	SUBSYSTEM_INIT_ARGS( environment, application );
	SUBSYSTEM_INIT( library );
	SUBSYSTEM_INIT( system );
	SUBSYSTEM_INIT( config );

	if( ret )
		return ret;

	//Parse built-in command line options
	{
		const string_const_t* cmdline = environment_command_line();
		size_t iarg, argsize;
		for( iarg = 0, argsize = array_size( cmdline ); iarg < argsize; ++iarg )
		{
			string_const_t arg = cmdline[iarg];
			if( string_equal( arg.str, arg.length, STRING_CONST( "--log-debug" ) ) )
				log_set_suppress( 0, ERRORLEVEL_NONE );
			else if( string_equal( arg.str, arg.length, STRING_CONST( "--log-info" ) ) )
				log_set_suppress( 0, ERRORLEVEL_DEBUG );
		}

		config_parse_commandline( cmdline, array_size( cmdline ) );
	}

	//Artificial references
#if FOUNDATION_PLATFORM_ANDROID
	android_main( 0 );
#elif FOUNDATION_PLATFORM_PNACL
	if( ( (uintptr_t)PPP_InitializeModule < 1 ) || ( (uintptr_t)PPP_GetInterface < 1 ) || ( (uintptr_t)PPP_ShutdownModule < 1 ) )
		return -1;
#else
	if( (uintptr_t)main < 1 )
		return -1;
#endif

	_foundation_initialized = true;

	return 0;
}


void foundation_startup( void )
{
	_memory_preallocate();
}


void foundation_shutdown( void )
{
	_foundation_initialized = false;

	profile_shutdown();

	_config_shutdown();
	_fs_shutdown();
	_stream_shutdown();
	_system_shutdown();
	_library_shutdown();
	_environment_shutdown();
	_random_shutdown();
	_thread_shutdown();
	_time_shutdown();
	_log_shutdown();
	_static_hash_shutdown();
	_memory_shutdown();
	_atomic_shutdown();
}


bool foundation_is_initialized( void )
{
	return _foundation_initialized;
}
