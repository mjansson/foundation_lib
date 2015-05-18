/* environment.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static string_t  _environment_executable_name;
static string_t  _environment_executable_dir;
static string_t  _environment_executable_path;
static string_t  _environment_initial_working_dir;
static string_t  _environment_current_working_dir;
static string_t  _environment_home_dir;
static string_t  _environment_temp_dir;
static bool      _environment_temp_dir_local;
#if FOUNDATION_PLATFORM_WINDOWS
static string_t  _environment_var;
#  include <foundation/windows.h>
#elif FOUNDATION_PLATFORM_POSIX
#  include <foundation/posix.h>
#endif

#if FOUNDATION_PLATFORM_ANDROID
#  include <foundation/android.h>
#endif

#if FOUNDATION_PLATFORM_APPLE
#  include <foundation/apple.h>
extern void _environment_ns_command_line( string_t** argv );
extern void _environment_ns_home_directory( string_t* );
extern void _environment_ns_temporary_directory( string_t* );
#endif

#if FOUNDATION_PLATFORM_BSD
#  include <sys/types.h>
#  include <sys/sysctl.h>
#endif

static application_t       _environment_app;

static string_t*           _environment_argv;
static int                 _environment_main_argc;
static const char* const*  _environment_main_argv;

static void _environment_clean_temporary_directory( bool recreate );


void _environment_main_args( int argc, const char* const* argv )
{
	_environment_main_argc = argc;
	_environment_main_argv = argv;
}

#if !FOUNDATION_PLATFORM_PNACL

static void _environment_set_executable_paths( char* executable_path, size_t length )
{
	size_t last_path = string_rfind( executable_path, length, '/', STRING_NPOS );
	if( last_path != STRING_NPOS )
	{
		_environment_executable_dir = string_clone( executable_path, last_path );
		_environment_executable_name = string_clone( executable_path + last_path + 1, length - ( last_path + 1 ) );
	}
	else
	{
		_environment_executable_dir = (string_t){ 0, 0 };
		_environment_executable_name = string_clone( executable_path, length );
	}
#if FOUNDATION_PLATFORM_WINDOWS
	if( ( _environment_executable_name.length > 4 ) &&
	    ( string_ends_with( _environment_executable_name.str, _environment_executable_name.length, STRING_CONST( ".exe" ) ) ||
	      string_ends_with( _environment_executable_name.str, _environment_executable_name.length, STRING_CONST( ".EXE" ) ) ) )
	{
		_environment_executable_name.str[ _environment_executable_name.length - 4 ] = 0;
		_environment_executable_name.length -= 4;
	}
#endif
	_environment_executable_path.str = executable_path;
	_environment_executable_path.length = length;
}

#endif

int _environment_initialize( const application_t application )
{
	string_const_t working_dir;

#if FOUNDATION_PLATFORM_WINDOWS
	int ia;
	int num_args = 0;
	DWORD ret = 0;
	wchar_t module_filename[FOUNDATION_MAX_PATHLEN];
	LPWSTR* arg_list = CommandLineToArgvW( GetCommandLineW(), &num_args );
	if( !arg_list )
		return -1;

	for( ia = 0; ia < num_args; ++ia )
		array_push( _environment_argv, string_allocate_from_wstring( arg_list[ia], 0 ) );

	LocalFree( arg_list );

	if( GetModuleFileNameW( 0, module_filename, FOUNDATION_MAX_PATHLEN ) )
	{
		string_t exe_path = string_allocate_from_wstring( module_filename, wstring_length( module_filename ) );
		exe_path = path_absolute( exe_path.str, exe_path.length );

		_environment_set_executable_paths( exe_path.str, exe_path.length ); //Adopts string
	}
	else
	{
		log_errorf( 0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST( "Unable to get module filename" ) );
		return -1;
	}

#elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS

	_environment_ns_command_line( &_environment_argv );

	//TODO: Read executable name from system, not command line (might be set to anything)
	string_t exe_path = string_clone( _environment_argv[0].str, _environment_argv[0].length );
	exe_path = path_absolute( exe_path.str, exe_path.length, exe_path.length, true );
	_environment_set_executable_paths( exe_path.str, exe_path.length ); //Adopts string

#elif FOUNDATION_PLATFORM_ANDROID

	stream_t* cmdline = fs_open_file( "/proc/self/cmdline", STREAM_IN | STREAM_BINARY );
	if( !cmdline )
	{
		log_errorf( 0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST( "Unable to read /proc/self/cmdline" ) );
		return -1;
	}

	while( true )
	{
		string_t arg = stream_read_string( cmdline );
		if( !arg.length )
		{
			string_deallocate( arg.str );
			break;
		}

		array_push( _environment_argv, arg );
	}
	stream_deallocate( cmdline );

	string_const_t dir_name = path_directory_name( android_app()->activity->internalDataPath, string_length( android_app()->activity->internalDataPath ) );
	string_t exe_path = path_merge( dir_name.str, dir_name.length, STRING_CONST( "lib" ) );

	// This will return something like "app_process" since we're just a dynamic
	// library that gets invoked by a launcher process
	char exelink[FOUNDATION_MAX_PATHLEN] = {0};
	ssize_t exelength = readlink( "/proc/self/exe", exelink, FOUNDATION_MAX_PATHLEN );
	if( exelength < 0 )
	{
		log_errorf( 0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST( "Unable to read /proc/self/exe link" ) );
		return -1;
	}

	string_const_t exe_name = path_file_name( exelink, (size_t)exelength );
	exe_path = path_append( exe_path.str, exe_path.length, exe_name.str, exe_name.length );

	_environment_set_executable_paths( exe_path.str, exe_path.length ); //Adopts string

#elif FOUNDATION_PLATFORM_BSD

	for( int ia = 0; ia < _environment_main_argc; ++ia )
		array_push( _environment_argv, string_clone( _environment_main_argv[ia], string_length( _environment_main_argv[ia] ) ) );

	int callarg[4];
	char buf[1024];
	size_t size = sizeof( buf );
	callarg[0] = CTL_KERN;
	callarg[1] = KERN_PROC;
	callarg[2] = KERN_PROC_PATHNAME;
	callarg[3] = -1;
	sysctl(callarg, 4, buf, &size, 0, 0);

	string_t exe_path = string_clone( buf, size );
	exe_path = path_clean( exe_path.str, exe_path.length, path_is_absolute( exe_path.str, exe_path.length ) );
	exe_path = path_absolute( exe_path.str, exe_path.length );

	_environment_set_executable_paths( exe_path.str, exe_path.length ); //Adopts string

#elif FOUNDATION_PLATFORM_POSIX

	stream_t* cmdline = fs_open_file( "/proc/self/cmdline", STREAM_IN | STREAM_BINARY );
	if( !cmdline )
	{
		log_error( 0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST( "Unable to read /proc/self/cmdline" ) );
		return -1;
	}

	while( true )
	{
		string_t arg = stream_read_string( cmdline );
		if( !arg.length )
		{
			string_deallocate( arg.str );
			break;
		}

		array_push( _environment_argv, arg );
	}
	stream_deallocate( cmdline );

	char exelink[FOUNDATION_MAX_PATHLEN] = {0};
	ssize_t exelength = readlink( "/proc/self/exe", exelink, FOUNDATION_MAX_PATHLEN );
	if( exelength < 0 )
	{
		log_errorf( 0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST( "Unable to read /proc/self/exe link" ) );
		return -1;
	}

	string_t exe_path = path_clean( exelink, exelength, FOUNDATION_MAX_PATHLEN, path_is_absolute( exelink, exelength ), false );
	exe_path = string_clone( exe_path.str, exe_path.length );
	exe_path = path_absolute( exe_path.str, exe_path.length );

	_environment_set_executable_paths( exe_path.str, exe_path.length ); //Adopts string

#elif FOUNDATION_PLATFORM_PNACL

	for( int ia = 0; ia < _environment_main_argc; ++ia )
		array_push( _environment_argv, string_clone( _environment_main_argv[ia], string_length( _environment_main_argv[ia] ) ) );

	_environment_executable_dir = string_clone( STRING_CONST( "/cache" ) );
	_environment_current_working_dir = string_clone( STRING_CONST( "/cache" ) );
	_environment_home_dir = string_clone( STRING_CONST( "/persistent" ) );
	_environment_temp_dir = string_clone( STRING_CONST( "/tmp" ) );
	_environment_executable_path = string_clone( application.short_name.str, application.short_name.length );

#else
#  error Not implemented
	/*if( array_size( _environment_argv ) > 0 )
	{
		char* exe_path = path_clean( string_clone( _environment_argv[0] ), path_is_absolute( _environment_argv[0] ) );
		char* dir_path = path_make_absolute( exe_path );

		_environment_set_executable_paths( dir_path );

		string_deallocate( dir_path );
		string_deallocate( exe_path );
	}
	else if( !string_length( _environment_executable_dir ) )
	   	string_copy( _environment_executable_dir, environment_current_working_directory(), FOUNDATION_MAX_PATHLEN ); */
#endif

   	_environment_app = application;

	if( uuid_is_null( _environment_app.instance ) )
		_environment_app.instance = uuid_generate_random();

	working_dir = environment_current_working_directory();
   	_environment_initial_working_dir = string_clone( working_dir.str, working_dir.length );

   	_environment_clean_temporary_directory( true );

	return 0;
}


void _environment_shutdown( void )
{
	_environment_clean_temporary_directory( false );

	string_array_deallocate( _environment_argv );

#if FOUNDATION_PLATFORM_WINDOWS
	string_deallocate( _environment_var );
#endif
}


const string_const_t* environment_command_line( void )
{
	return (const string_const_t*)_environment_argv;
}


string_const_t environment_executable_name( void )
{
	return string_to_const( _environment_executable_name );
}


string_const_t environment_executable_directory( void )
{
	return string_to_const( _environment_executable_dir );
}


string_const_t environment_executable_path( void )
{
	return string_to_const( _environment_executable_path );
}


string_const_t environment_initial_working_directory( void )
{
	return string_to_const( _environment_initial_working_dir );
}


string_const_t environment_current_working_directory( void )
{
	if( _environment_current_working_dir.str )
		return string_to_const( _environment_current_working_dir );
#if FOUNDATION_PLATFORM_WINDOWS
	{
		string_t path;
		wchar_t* wd = memory_allocate( 0, sizeof( wchar_t ) * FOUNDATION_MAX_PATHLEN, 0, MEMORY_TEMPORARY | MEMORY_ZERO_INITIALIZED );
		GetCurrentDirectoryW( FOUNDATION_MAX_PATHLEN-1, wd );
		path = string_allocate_from_wstring( wd, 0 );
		_environment_current_working_dir = path_clean( path.str, path.length, path.length, true, true );
		memory_deallocate( wd );
	}
#elif FOUNDATION_PLATFORM_POSIX
	char path[FOUNDATION_MAX_PATHLEN];
	if( !getcwd( path, FOUNDATION_MAX_PATHLEN ) )
	{
		int err = errno;
		string_const_t errmsg = system_error_message( err );
		log_errorf( 0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST( "Unable to get cwd: %.*s (%d)" ), (int)errmsg.length, errmsg.str, err );
		return string_const( 0, 0 );
	}
	_environment_current_working_dir = path_clean( path, string_length( path ), FOUNDATION_MAX_PATHLEN, true, false );
	_environment_current_working_dir = string_clone( _environment_current_working_dir.str, _environment_current_working_dir.length );
#elif FOUNDATION_PLATFORM_PNACL
	_environment_current_working_dir = string_clone( STRING_CONST( "/persistent" ) );
#else
#  error Not implemented
#endif
	return string_to_const( _environment_current_working_dir );
}


void environment_set_current_working_directory( const char* path, size_t length )
{
	if( !path )
		return;
	log_debugf( 0, STRING_CONST( "Setting current working directory to: %.*s" ), (int)length, path );
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t* wpath = wstring_allocate_from_string( path, length );
		if( !SetCurrentDirectoryW( wpath ) )
			log_warnf( 0, WARNING_SUSPICIOUS, STRING_CONST( "Unable to set working directory: %ls" ), wpath );
		wstring_deallocate( wpath );
	}
	string_deallocate( _environment_current_working_dir.str );
	_environment_current_working_dir = (string_t){ 0, 0 };
#elif FOUNDATION_PLATFORM_POSIX
	if( chdir( path ) < 0 )
	{
		int err = errno;
		string_const_t errmsg = system_error_message( err );
		log_warnf( 0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST( "Unable to set working directory to %.*s: %.*s (%d)" ), (int)length, path, (int)errmsg.length, errmsg.str, err );
	}
	string_deallocate( _environment_current_working_dir.str );
	_environment_current_working_dir = (string_t){ 0, 0 };
#elif FOUNDATION_PLATFORM_PNACL
	//Allow anything
	string_deallocate( _environment_current_working_dir.str );
	_environment_current_working_dir = string_clone( path, length );
	_environment_current_working_dir = path_absolute( _environment_current_working_dir.str, _environment_current_working_dir.length );
#else
#  error Not implemented
#endif
}


string_const_t environment_home_directory( void )
{
	if( _environment_home_dir.str )
		return string_to_const( _environment_home_dir );
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t wpath[FOUNDATION_MAX_PATHLEN];
		SHGetFolderPathW( 0, CSIDL_LOCAL_APPDATA, 0, 0, wpath );
		_environment_home_dir = string_allocate_from_wstring( wpath, 0 );
		_environment_home_dir = path_clean( _environment_home_dir.str, _environment_home_dir.length, _environment_home_dir.length, true, true );
	}
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_BSD || FOUNDATION_PLATFORM_TIZEN
	string_const_t env_home = environment_variable( "HOME" );
	if( !env_home.length )
	{
		struct passwd* pw = getpwuid( getuid() );
		string_deallocate( env_home.str );
		env_home.str = pw->pw_dir;
		env_home.length = string_length( env_home.str );
	}
	_environment_home_dir = string_clone( env_home.str, env_home.length );
#elif FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_MACOSX
	char path[FOUNDATION_MAX_PATHLEN];
	string_t pathstr = (string_t){ path, FOUNDATION_MAX_PATHLEN };
	_environment_ns_home_directory( &pathstr );
#  if FOUNDATION_PLATFORM_MACOSX
	if( !( environment_application()->flags & APPLICATION_UTILITY ) )
	{
		char bundle_identifier[256];
		string_t bundle = environment_bundle_identifier( bundle_identifier, 256 );

		pathstr = path_append( pathstr.str, pathstr.length, FOUNDATION_MAX_PATHLEN, STRING_CONST( "Library/Application Support" ), false );
		pathstr = path_append( pathstr.str, pathstr.length, FOUNDATION_MAX_PATHLEN, bundle.str, bundle.length, false );
	}
#  endif
	_environment_home_dir = string_clone( pathstr.str, pathstr.length );
#elif FOUNDATION_PLATFORM_ANDROID
	_environment_home_dir = string_clone( android_app()->activity->internalDataPath, string_length( android_app()->activity->internalDataPath ) );
#elif FOUNDATION_PLATFORM_PNACL
#else
#  error Not implemented
#endif
	return string_to_const( _environment_home_dir );
}


string_const_t environment_temporary_directory( void )
{
	if( _environment_temp_dir.str )
		return string_to_const( _environment_temp_dir );
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t wpath[FOUNDATION_MAX_PATHLEN];
		GetTempPathW( FOUNDATION_MAX_PATHLEN, wpath );
		_environment_temp_dir = string_allocate_from_wstring( wpath, 0 );
		_environment_temp_dir = path_clean( _environment_temp_dir.str, _environment_temp_dir.length, _environment_temp_dir.length, true, true );
	}
#endif
#if FOUNDATION_PLATFORM_ANDROID
	//Use application internal data path, or if that fails, external data path
	struct android_app* app = android_app();
	const char* test_path[] = { app && app->activity ? app->activity->internalDataPath : 0, app && app->activity ? app->activity->externalDataPath : 0 };
	char path[FOUNDATION_MAX_PATHLEN];
	for( int itest = 0; !_environment_temp_dir.str && ( itest < 2 ); ++itest )
	{
		if( test_path[itest] && test_path[itest][0] )
		{
			string_t pathstr = string_copy( path, FOUNDATION_MAX_PATHLEN, test_path[itest], string_length( test_path[itest] ) );
			pathstr = path_clean( pathstr.str, pathstr.length, FOUNDATION_MAX_PATHLEN, true, false );

			fs_make_directory( pathstr.str, pathstr.length );

			string_t temp_path = path_append( pathstr.str, pathstr.length, FOUNDATION_MAX_PATHLEN, STRING_CONST( ".tmp-" ) );
			string_const_t uuidstr = string_from_uuid_static( uuid_generate_random() );
			temp_path = string_append( temp_path.str, temp_path.length, FOUNDATION_MAX_PATHLEN, uuidstr.str, uuidstr.length, false );

			stream_t* temp_stream = fs_open_file( temp_path.str, temp_path.length, STREAM_CREATE | STREAM_OUT | STREAM_BINARY );
			if( temp_stream )
			{
				stream_deallocate( temp_stream );

				_environment_temp_dir = path_append( pathstr.str, pathstr.length, FOUNDATION_MAX_PATHLEN, STRING_CONST( ".tmp" ) );
				_environment_temp_dir = string_clone( _environment_temp_dir.str, _environment_temp_dir.length );

				_environment_temp_dir_local = true;
			}
		}
	}
#endif
#if FOUNDATION_PLATFORM_APPLE
	_environment_ns_temporary_directory( &_environment_temp_dir );
#endif
#if FOUNDATION_PLATFORM_IOS
	_environment_temp_dir_local = true;
#endif
#if FOUNDATION_PLATFORM_POSIX
	_environment_temp_dir = string_clone( P_tmpdir, string_length( P_tmpdir ) );
#endif
#if !FOUNDATION_PLATFORM_ANDROID && !FOUNDATION_PLATFORM_IOS
	if( _environment_app.config_dir.length )
	{
		size_t curlen = _environment_temp_dir.length;
		size_t cfglen = _environment_app.config_dir.length;
		size_t totallen = curlen + cfglen + 40;
		if( totallen < FOUNDATION_MAX_PATHLEN )
		{
			string_t modpath = string_allocate( totallen );
			string_const_t uuidstr = string_from_uuid_static( _environment_app.instance );
			string_copy( modpath.str, totallen, _environment_temp_dir.str, _environment_temp_dir.length );
			modpath = path_append( modpath.str, modpath.length, totallen, _environment_app.config_dir.str, _environment_app.config_dir.length, false );
			modpath = path_append( modpath.str, modpath.length, totallen, uuidstr.str, uuidstr.length, false );
			string_deallocate( _environment_temp_dir.str );
			_environment_temp_dir = modpath;
			_environment_temp_dir_local = true;
		}
	}
#endif
	if( ( _environment_temp_dir.length > 1 ) && ( _environment_temp_dir.str[ _environment_temp_dir.length - 1 ] == '/' ) )
	{
		--_environment_temp_dir.length;
		_environment_temp_dir.str[ _environment_temp_dir.length ] = 0;
	}
	return string_to_const( _environment_temp_dir );
}


static void _environment_clean_temporary_directory( bool recreate )
{
	string_const_t path = environment_temporary_directory();

	if( _environment_temp_dir_local && fs_is_directory( path.str, path.length ) )
	{
		fs_remove_directory( path.str, path.length );
		if( recreate )
			fs_make_directory( path.str, path.length );
	}
}


string_const_t environment_variable( const char* var, size_t length )
{
#if FOUNDATION_PLATFORM_WINDOWS
	unsigned int required;
	wchar_t* key = wstring_allocate_from_string( var, length );
	wchar_t val[FOUNDATION_MAX_PATHLEN]; val[0] = 0;
	if( ( required = GetEnvironmentVariableW( key, val, FOUNDATION_MAX_PATHLEN ) ) > FOUNDATION_MAX_PATHLEN )
	{
		wchar_t* val_local = memory_allocate( 0, sizeof( wchar_t ) * ( required + 2 ), 0, MEMORY_TEMPORARY );
		val_local[0] = 0;
		required = GetEnvironmentVariableW( key, val_local, required + 1 );
		if( _environment_var.str )
			string_deallocate( _environment_var.str );
		_environment_var = string_allocate_from_wstring( val_local, required );
		memory_deallocate( val_local );
	}
	else
	{
		if( _environment_var.str )
			string_deallocate( _environment_var.str );
		_environment_var = string_allocate_from_wstring( val, required );
	}
	wstring_deallocate( key );
	return _environment_var;
#elif FOUNDATION_PLATFORM_POSIX
	FOUNDATION_UNUSED( length );
	const char* value = getenv( var );
	return string_const( value, value ? string_length( value ) : 0 );
#elif FOUNDATION_PLATFORM_PNACL
	FOUNDATION_UNUSED( var );
	FOUNDATION_UNUSED( length );
	return string_const( 0, 0 ); //No env vars on PNaCl
#else
#  error Not implemented
#endif
}


const application_t* environment_application( void )
{
	return &_environment_app;
}
