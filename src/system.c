/* system.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation.h>

#if FOUNDATION_PLATFORM_POSIX
#  include <sched.h>
#  include <unistd.h>
#  include <stdlib.h>
#  define __error_t_defined 1
#  include <errno.h>
#endif

static event_stream_t* _system_event_stream = 0;

typedef struct _foundation_platform_info
{
	platform_t      platform;
	architecture_t  architecture;
	byteorder_t     byteorder;
} platform_info_t;

static platform_info_t _platform_info = {

#if FOUNDATION_PLATFORM_WINDOWS
	PLATFORM_WINDOWS,
#elif FOUNDATION_PLATFORM_LINUX
	PLATFORM_LINUX,
#elif FOUNDATION_PLATFORM_MACOSX
	PLATFORM_MACOSX,
#elif FOUNDATION_PLATFORM_IOS
	PLATFORM_IOS,
#elif FOUNDATION_PLATFORM_ANDROID
	PLATFORM_ANDROID,
#else
#  error Unknown platform
#endif

#if FOUNDATION_PLATFORM_ARCH_X86_64
ARCHITECTURE_X86_64,
#elif FOUNDATION_PLATFORM_ARCH_X86
ARCHITECTURE_X86,
#elif FOUNDATION_PLATFORM_ARCH_PPC_64
ARCHITECTURE_PPC_64,
#elif FOUNDATION_PLATFORM_ARCH_PPC
ARCHITECTURE_PPC,
#elif FOUNDATION_PLATFORM_ARCH_ARM8
ARCHITECTURE_ARM8,
#elif FOUNDATION_PLATFORM_ARCH_ARM7
ARCHITECTURE_ARM7,
#elif FOUNDATION_PLATFORM_ARCH_ARM6
ARCHITECTURE_ARM6,
#else
#  error Unknown architecture
#endif

#if FOUNDATION_PLATFORM_ENDIAN_LITTLE
BYTEORDER_LITTLEENDIAN
#else
BYTEORDER_BIGENDIAN
#endif

};


platform_t system_platform()
{
	return _platform_info.platform;
}


architecture_t system_architecture()
{
	return _platform_info.architecture;
}


byteorder_t system_byteorder()
{
	return _platform_info.byteorder;
}


#if FOUNDATION_PLATFORM_WINDOWS

#include <safewindows.h>

object_t _system_library_iphlpapi = 0;


int _system_initialize( void )
{
	_system_event_stream = event_stream_allocate( 128 );
	return 0;
}


void _system_shutdown( void )
{
	if( _system_library_iphlpapi )
		library_unload( _system_library_iphlpapi );
	_system_library_iphlpapi = 0;

	event_stream_deallocate( _system_event_stream );
	_system_event_stream = 0;
}


const char* system_error_message( int code )
{
	static THREADLOCAL char errmsg[256];

	if( !code )
		code = GetLastError();
	if( !code )
		return "";

	errmsg[0] = errmsg[255] = 0;
	FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, code & 0xBFFFFFFF, 0/*LANG_SYSTEM_DEFAULT*//*MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT )*/, errmsg, 255, 0 );
	string_strip( errmsg, STRING_WHITESPACE );

	return errmsg;
}


const char* system_hostname( void )
{
	unsigned int size = 255;
	static char hostname[256] = {0};
	if( hostname[0] )
		return hostname;
	strcpy( hostname, "<unknown>" );
	GetComputerNameA( hostname, &size );
	hostname[255] = 0;
	return hostname;
}


uint64_t system_hostid( void )
{
	unsigned char hostid[8] = {0};
	IP_ADAPTER_INFO adapter_info[16];
	unsigned int status, buffer_length, i, j;
	DWORD (__stdcall *fn_get_adapters_info)( PIP_ADAPTER_INFO, PULONG ) = 0;

	if( !_system_library_iphlpapi )
	{
		_system_library_iphlpapi = library_load( "iphlpapi" );
		fn_get_adapters_info = (DWORD (__stdcall *)( PIP_ADAPTER_INFO, PULONG ))library_symbol( _system_library_iphlpapi, "GetAdaptersInfo" );
	}
	if( !fn_get_adapters_info )
		return 0;
	
	buffer_length = sizeof( adapter_info );  // Save memory size of buffer
	memset( adapter_info, 0, sizeof( adapter_info ) );
	status = fn_get_adapters_info( adapter_info, &buffer_length );
	if( status == ERROR_SUCCESS ) for( i = 0; i < 16; ++i )
	{
		if( adapter_info[i].Type == MIB_IF_TYPE_ETHERNET )
		{
			for( j = 0; j < 6; ++j )
				hostid[5-j] = adapter_info[i].Address[j];
			break;
		}
	}
	return *(uint64_t*)hostid;
}


const char* system_username( void )
{
	unsigned int size = 255;
	static char username[256] = {0};
	if( username[0] )
		return username;
	strcpy( username, "<unknown>" );
	GetUserNameA( username, &size );
	username[255] = 0;
	return username;
}


unsigned int system_hardware_threads( void )
{
	SYSTEM_INFO system_info;
	GetSystemInfo( &system_info );
	return system_info.dwNumberOfProcessors;
}


void system_process_events( void )
{
}


bool system_debugger_attached( void )
{
	return IsDebuggerPresent();
}


void system_pause( void )
{
	system( "pause" );
}


static const char* _system_default_locale( void )
{
	return "enUS";
}


#elif FOUNDATION_PLATFORM_POSIX


int _system_initialize( void )
{
	_system_event_stream = event_stream_allocate( 128 );
	return 0;
}


void _system_shutdown( void )
{
	event_stream_deallocate( _system_event_stream );
	_system_event_stream = 0;
}


const char* system_error_message( int code )
{
	if( !code )
		code = errno;
	if( !code )
		return "";
#if FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
	static char buffer[256]; //TODO: Thread safety
#else
	static THREADLOCAL char buffer[256];
#endif
	strerror_r( code, buffer, 256 );
	return buffer;
}


const char* system_hostname( void )
{
	static char hostname[256] = {0};
	if( hostname[0] )
		return hostname;
	strcpy( hostname, "<unknown>" );
	gethostname( hostname, 256 );
	hostname[255] = 0;
	return hostname;
}


const char* system_username( void )
{
	static char username[64] = {0};
	if( username[0] )
		return username;
	strcpy( username, "<unknown>" );
#if FOUNDATION_PLATFORM_ANDROID
	strncpy( username, getlogin(), 64 );
#else
	getlogin_r( username, 64 );
#endif
	username[63] = 0;
	return username;	
}


uint64_t system_hostid( void )
{
	/*int s;
	struct ifreq buffer;
	s = socket( PF_INET, SOCK_DGRAM, 0 );
	memset( &buffer, 0, sizeof( buffer ) );
	strcpy(buffer.ifr_name, "eth0");
	ioctl(s, SIOCGIFHWADDR, &buffer);
	close(s);
	printf("%.2X ", (unsigned char)buffer.ifr_hwaddr.sa_data[s]);*/
#if FOUNDATION_PLATFORM_ANDROID
	return 0;
#else
	return gethostid();
#endif
}


unsigned int system_hardware_threads( void )
{
#if FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_MACOSX
	return _ns_process_info_processor_count();
#elif FOUNDATION_PLATFORM_ANDROID
	return android_getCpuCount();
#else
	cpu_set_t prevmask, testmask;
	CPU_ZERO( &prevmask );
	CPU_ZERO( &testmask );
	sched_getaffinity( 0, sizeof( prevmask ), &prevmask ); //Get current mask
	sched_setaffinity( 0, sizeof( testmask ), &testmask ); //Set zero mask
	sched_getaffinity( 0, sizeof( testmask ), &testmask ); //Get mask for all CPUs
	sched_setaffinity( 0, sizeof( prevmask ), &prevmask ); //Reset current mask
	unsigned int num = CPU_COUNT( &testmask );
	return ( num > 1 ? num : 1 );
#endif
}


void system_process_events( void )
{
#if FOUNDATION_PLATFORM_ANDROID
	profile_begin_block( "system events" );

	int ident = 0;
	int events = 0;
	int nummsg = 0;
	struct android_poll_source* source = 0;
	struct android_app* app = _global_app;

	while( ( ident = ALooper_pollAll( 0, 0, &events, (void**)&source ) ) >= 0 )
	{
		// Process this event.
		if( source )
			source->process( app, source );
		++nummsg;
	}
	
	profile_end_block();
#endif
}


#if FOUNDATION_PLATFORM_ANDROID

void android_handle_cmd( struct android_app* app, int32_t cmd )
{
    switch( cmd )
	{
		case APP_CMD_INPUT_CHANGED:
		{
			info_logf( "Got APP_CMD_INPUT_CHANGED" );
            break;
		}

		case APP_CMD_INIT_WINDOW:
		{
			if( app->window )
			{
				int w = 0, h = 0;
				w = ANativeWindow_getWidth( app->window );
				h = ANativeWindow_getHeight( app->window );
				info_logf( "Got APP_CMD_INIT_WINDOW dimensions %dx%d", w, h );
			}
            break;
		}
        
		case APP_CMD_TERM_WINDOW:
		{
			info_logf( "Got APP_CMD_TERM_WINDOW" );
            break;
		}

    	case APP_CMD_WINDOW_RESIZED:
		{
			info_logf( "Got APP_CMD_WINDOW_RESIZED" );
            break;
		}

		case APP_CMD_WINDOW_REDRAW_NEEDED:
		{
			info_logf( "Got APP_CMD_WINDOW_REDRAW_NEEDED" );
            break;
		}

		case APP_CMD_CONTENT_RECT_CHANGED:
		{
			info_logf( "Got APP_CMD_CONTENT_RECT_CHANGED" );
            break;
		}
        
		case APP_CMD_GAINED_FOCUS:
		{
			info_logf( "Got APP_CMD_GAINED_FOCUS" );
			_app_enable_sensor( ASENSOR_TYPE_ACCELEROMETER );
            break;
		}

		case APP_CMD_LOST_FOCUS:
		{
			info_logf( "Got APP_CMD_LOST_FOCUS" );
			_app_disable_sensor( ASENSOR_TYPE_ACCELEROMETER );
            break;
		}

		case APP_CMD_CONFIG_CHANGED:
		{
			info_logf( "Got APP_CMD_CONFIG_CHANGED" );
            break;
		}

		case APP_CMD_LOW_MEMORY:
		{
			info_logf( "Got APP_CMD_LOW_MEMORY" );
            break;
		}

		case APP_CMD_START:
		{
			info_logf( "Got APP_CMD_START" );
            break;
		}

		case APP_CMD_RESUME:
		{
			info_logf( "Got APP_CMD_RESUME" );
			app_reset_frame_time();
			app_main_loop_resume();
            break;
		}

		case APP_CMD_SAVE_STATE:
		{
			info_logf( "Got APP_CMD_SAVE_STATE" );
            break;
		}

		case APP_CMD_PAUSE:
		{
			info_logf( "Got APP_CMD_PAUSE" );
			app_main_loop_suspend();
            break;
		}

		case APP_CMD_STOP:
		{
			info_logf( "Got APP_CMD_STOP" );
            break;
		}
		
		case APP_CMD_DESTROY:
		{
			info_logf( "Got APP_CMD_DESTROY" );
			system_event_post( COREEVENT_TERMINATE );
            break;
		}

		default:
			break;
    }
}

#endif


bool system_debugger_attached( void )
{
	return false;
}


void system_pause( void )
{
}


static const char* _system_default_locale( void )
{
	return "enUS";
}


void system_browser_open( const char* url )
{

}


#endif


uint32_t system_locale( void )
{
	uint32_t localeval = 0;
	const char* locale = config_string( HASH_ENGINE, HASH_LOCALE );
	if( !locale || ( string_length( locale ) != 4 ) )
		locale = config_string( HASH_APPLICATION, HASH_LOCALE );
	if( !locale || ( string_length( locale ) != 4 ) )
		locale = _system_default_locale();
	memcpy( &localeval, locale, 4 );
	return localeval;
}


const char* system_locale_string( void )
{
	static char localestr[5] = {0};
	uint32_t locale = system_locale();
	memcpy( localestr, &locale, 4 );
	return localestr;
}


uint16_t system_language( void )
{
	return (uint16_t)( ( system_locale() >> 16 ) & 0xFFFF );
}


uint16_t system_country( void )
{
	return (uint16_t)( system_locale() & 0xFFFF );
}


event_stream_t* system_event_stream( void )
{
	return _system_event_stream;
}


void system_post_event( foundation_event_id event )
{
	event_post( _system_event_stream, SYSTEM_FOUNDATION, event, 0, 0, 0 );
}


bool system_message_box( const char* title, const char* message, bool cancel_button )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return ( MessageBoxA( 0, message, title, cancel_button ? MB_OKCANCEL : MB_OK ) == IDOK );
#elif FOUNDATION_PLATFORM_MACOSX
	return _objc_show_alert( title, message, cancel_button ? 1 : 0 ) > 0;
#elif 0//FOUNDATION_PLATFORM_LINUX
	char* buf = string_format( "%s\n\n%s\n", title, message );
	pid_t pid = fork();

	switch( pid )
	{
		case -1:
			//error
			string_deallocate( buf );
			break;

		case 0:
			execlp( "xmessage", "xmessage", "-buttons", cancel_button ? "OK:101,Cancel:102" : "OK:101", "-default", "OK", "-center", buf, (char*)0 );
			_exit( -1 );
			break;

		default:
		{
			string_deallocate( buf );
			int status;
			waitpid( pid, &status, 0 );
			if( ( !WIFEXITED( status ) ) || ( WEXITSTATUS( status ) != 101 ) )
				return false;
			return true;
		}
	}

	return false;
#else
	//Not implemented
	return false;
#endif
}
