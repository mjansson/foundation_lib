/* main.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <foundation/main.h>

FOUNDATION_EXTERN void foundation_startup( void );


#if FOUNDATION_PLATFORM_WINDOWS

#  include <foundation/windows.h>


BOOL STDCALL _main_console_handler( DWORD control_type )
{
	const char* control_name = "UNKNOWN";
	bool post_terminate = false;
	bool handled = true;

	switch( control_type )
	{
		case CTRL_C_EVENT:         control_name = "CTRL_C"; post_terminate = true; break;
		case CTRL_BREAK_EVENT:     control_name = "CTRL_BREAK"; break;
		case CTRL_CLOSE_EVENT:     control_name = "CTRL_CLOSE"; post_terminate = true; break;
		case CTRL_LOGOFF_EVENT:    control_name = "CTRL_LOGOFF"; post_terminate = !config_bool( HASH_APPLICATION, HASH_DAEMON ); break;
		case CTRL_SHUTDOWN_EVENT:  control_name = "CTRL_SHUTDOWN"; post_terminate = true; break;
		default:                   handled = false; break;
	}
	log_infof( "Caught console control: %s (%d)", control_name, control_type );
	if( post_terminate )
	{
		unsigned long level = 0, flags = 0;

		system_post_event( FOUNDATIONEVENT_TERMINATE );
		
		GetProcessShutdownParameters( &level, &flags );
		SetProcessShutdownParameters( level, SHUTDOWN_NORETRY );

		thread_sleep( 1000 );
	}
	return handled;
}


/*! Win32 UI application entry point, credits to Microsoft for ignoring yet another standard... */
#  if FOUNDATION_COMPILER_MSVC
FOUNDATION_API int APIENTRY WinMain( HINSTANCE, HINSTANCE, LPSTR, int );
#  endif

int STDCALL WinMain( HINSTANCE instance, HINSTANCE previnst, LPSTR cline, int cmd_show )
{
	int ret = -1;

	if( main_initialize() < 0 )
		return -1;

	SetConsoleCtrlHandler( _main_console_handler, TRUE );

	thread_set_main();

	foundation_startup();

#if BUILD_DEBUG
	ret = main_run( 0 );
#else
	{
		char* name = 0;
		const application_t* app = environment_application();
		{
			const char* aname = app->short_name;
			name = string_clone( aname ? aname : "unknown" );
			name = string_append( name, "-" );
			name = string_append( name, string_from_version_static( app->version ) );
		}

		if( app->dump_callback )
			crash_guard_set( app->dump_callback, name );

		ret = crash_guard( main_run, 0, app->dump_callback, name );

		string_deallocate( name );
	}
#endif

	main_shutdown();

	return ret;
}


#elif FOUNDATION_PLATFORM_ANDROID

#include <foundation/android.h>

#elif FOUNDATION_PLATFORM_POSIX

#include <foundation/posix.h>

#include <signal.h>
#include <stdio.h>

#if FOUNDATION_PLATFORM_MACOSX
extern int NSApplicationMain( int argc, const char *argv[] );
#elif FOUNDATION_PLATFORM_IOS
extern int UIApplicationMain ( int argc, char *argv[], void *principalClassName, void *delegateClassName );
#endif

void sighandler( int sig )
{
	const char* signame = "UNKNOWN";

	switch( sig )
	{
		case SIGKILL: signame = "SIGKILL"; break;
		case SIGTERM: signame = "SIGTERM"; break;
		case SIGQUIT: signame = "SIGQUIT"; break;
		case SIGINT:  signame = "SIGINT"; break;
		default: break;
	}
	log_infof( "Caught signal: %s (%d)", signame, sig );
	system_post_event( FOUNDATIONEVENT_TERMINATE );
	//process_exit( -1 );
}

#endif


/*! Normal entry point for all platforms */

#if FOUNDATION_PLATFORM_ANDROID
void android_main( struct android_app* app )
#else
int main( int argc, char **argv )
#endif
{
	int ret = -1;

#if FOUNDATION_PLATFORM_ANDROID
	if( android_initialize( app ) < 0 )
		return;
	if( main_initialize() < 0 )
		return;
#else
	if( main_initialize() < 0 )
		return ret;
#endif

#if FOUNDATION_PLATFORM_POSIX && !FOUNDATION_PLATFORM_ANDROID
	
	//Set signal handlers
	{
		struct sigaction action;
		memset( &action, 0, sizeof( action ) );

		//Signals we process globally
		action.sa_handler = sighandler;
		sigaction( SIGKILL, &action, 0 );
		sigaction( SIGTERM, &action, 0 );
		sigaction( SIGQUIT, &action, 0 );
		sigaction( SIGINT,  &action, 0 );

		//Ignore sigpipe
		action.sa_handler = SIG_IGN;
		sigaction( SIGPIPE, &action, 0 );
	}

#endif

#if FOUNDATION_PLATFORM_WINDOWS

	SetConsoleCtrlHandler( _main_console_handler, TRUE );

#endif

	thread_set_main();

	foundation_startup();

#if FOUNDATION_PLATFORM_APPLE
	if( !( environment_application()->flags & APPLICATION_UTILITY ) )
	{
#if FOUNDATION_PLATFORM_MACOSX

		//Fire up new thread to continue engine, then run Cocoa event loop in main thread
		//_app_start_main_ns_thread( argc, argv );
		ret = NSApplicationMain( argc, (const char**)argv );

#elif FOUNDATION_PLATFORM_IOS

		ret = UIApplicationMain( argc, (char**)argv, 0, 0 );

#endif
		//NSApplicationMain and UIApplicationMain never returns though
		return ret;
	}
#endif

#if BUILD_DEBUG
	ret = main_run( 0 );
#else
	{
		char* name = 0;
		const application_t* app = environment_application();
		{
			const char* aname = app->short_name;
			name = string_clone( aname ? aname : "unknown" );
			name = string_append( name, "-" );
			name = string_append( name, string_from_version_static( app->version ) );
		}

		if( app->dump_callback )
			crash_guard_set( app->dump_callback, name );

		ret = crash_guard( main_run, 0, app->dump_callback, name );

		string_deallocate( name );
	}
#endif

	main_shutdown();

#if FOUNDATION_PLATFORM_ANDROID

	android_shutdown();
	
#else

	return ret;

#endif
}
