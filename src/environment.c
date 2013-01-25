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

#include <foundation.h>


static char**  _environment_argv;
static char    _environment_wd[FOUNDATION_MAX_PATHLEN] = {0};
static char    _environment_executable_dir[FOUNDATION_MAX_PATHLEN] = {0};
static char    _environment_initial_working_dir[FOUNDATION_MAX_PATHLEN] = {0};
static char    _environment_current_working_dir[FOUNDATION_MAX_PATHLEN] = {0};
static char    _environment_home_dir[FOUNDATION_MAX_PATHLEN] = {0};
static char    _environment_temp_dir[FOUNDATION_MAX_PATHLEN] = {0};
#if FOUNDATION_PLATFORM_WINDOWS
static char    _environment_var[FOUNDATION_MAX_PATHLEN] = {0};
#  include <safewindows.h>
#endif

static application_t   _environment_app;


int _environment_initialize( const application_t application )
{
#if FOUNDATION_PLATFORM_WINDOWS
	int ia;
	int num_args = 0;
	LPWSTR* arg_list = CommandLineToArgvW( GetCommandLineW(), &num_args );
	if( !arg_list )
		return -1;

	for( ia = 0; ia < num_args; ++ia )
		array_push( _environment_argv, string_allocate_from_wstring( arg_list[ia], 0 ) );

	LocalFree( arg_list );
#else
#  error Not implemented yet
#endif

   	_environment_app = application;

   	string_copy( _environment_initial_working_dir, environment_current_working_directory(), FOUNDATION_MAX_PATHLEN );

   	if( array_size( _environment_argv ) > 0 )
	{
		char* exe_path = path_clean( string_clone( _environment_argv[0] ), path_is_absolute( _environment_argv[0] ) );
		char* dir_path = path_make_absolute( exe_path );
		unsigned int last_path = string_rfind( dir_path, '/', STRING_NPOS );
		if( last_path != STRING_NPOS )
			dir_path[last_path] = 0;
		else
			dir_path[0] = 0;
		string_copy( _environment_executable_dir, dir_path, FOUNDATION_MAX_PATHLEN );
		string_deallocate( dir_path );
		string_deallocate( exe_path );
	}
   	else
	   	string_copy( _environment_executable_dir, environment_current_working_directory(), FOUNDATION_MAX_PATHLEN );

	return 0;
}


void _environment_shutdown( void )
{
	string_array_deallocate( _environment_argv );
}


const char* const* environment_command_line( void )
{
	return _environment_argv;
}


const char* environment_executable_directory( void )
{
	return _environment_executable_dir;
}


const char* environment_initial_working_directory( void )
{
	return _environment_initial_working_dir;
}


const char* environment_current_working_directory( void )
{
	if( _environment_current_working_dir[0] )
		return _environment_current_working_dir;
#if FOUNDATION_PLATFORM_WINDOWS
	{
		char* path;
		wchar_t* wd = memory_allocate_zero( sizeof( wchar_t ) * FOUNDATION_MAX_PATHLEN, 0, MEMORY_TEMPORARY );
		GetCurrentDirectoryW( FOUNDATION_MAX_PATHLEN-1, wd );
		path = path_clean( string_allocate_from_wstring( wd, 0 ), true );
		string_copy( _environment_current_working_dir, path, FOUNDATION_MAX_PATHLEN );
		string_deallocate( path );
		memory_deallocate( wd );
	}
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
	char* path = memory_allocate_zero( FOUNDATION_MAX_PATHLEN, 0, MEMORY_TEMPORARY );
	if( !getcwd( path, FOUNDATION_MAX_PATHLEN ) )
	{
		error_logf( ERRORLEVEL_ERROR, ERROR_SYSTEM_CALL_FAIL, "Unable to get cwd: %s", system_error_message( 0 ) );
		return "";
	}
	path = path_clean( path, true );
	string_copy( _environment_current_working_dir, path, FOUNDATION_MAX_PATHLEN );
	memory_deallocate( path );
#else
#  error Not implemented
#endif
	return _environment_current_working_dir;
}


void environment_set_current_working_directory( const char* path )
{
	if( !path )
		return;
	log_debugf( "Setting current working directory to: %s", path );
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t* wpath = wstring_allocate_from_string( path, 0 );
		if( !SetCurrentDirectoryW( wpath ) )
			log_warnf( WARNING_SUSPICIOUS, "Unable to set working directory: %ls", wpath );
		wstring_deallocate( wpath );
	}
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
	if( chdir( path ) < 0 )
		warn_logf( WARNING_SUSPICIOUS, "Unable to set working directory: %s", path );
#else
#  error Not implemented
#endif
	_environment_current_working_dir[0] = 0;
}


const char* environment_home_directory( void )
{
	if( _environment_home_dir[0] )
		return _environment_home_dir;
#if FOUNDATION_PLATFORM_WINDOWS
	{
		char* path;
		wchar_t* wpath = memory_allocate_zero( sizeof( wchar_t ) * FOUNDATION_MAX_PATHLEN, 0, MEMORY_TEMPORARY );
		SHGetFolderPathW( 0, CSIDL_LOCAL_APPDATA, 0, 0, wpath );
		path = path_clean( string_allocate_from_wstring( wpath, 0 ), true );
		string_copy( _environment_home_dir, path, FOUNDATION_MAX_PATHLEN );
		string_deallocate( path );
		memory_deallocate( wpath );
	}
#elif FOUNDATION_PLATFORM_LINUX
	string_copy( _environment_home_dir, environment_variable( "HOME" ), FOUNDATION_MAX_PATHLEN );
#elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	ENTER_AUTORELEASE();
	CFStringRef home = NSHomeDirectory();
	CFStringGetCString( home, _environment_home_dir, FOUNDATION_MAX_PATHLEN, kCFStringEncodingUTF8 );
	LEAVE_AUTORELEASE();

	if( !app_config_bool( _HASH_APPLICATION, _HASH_BSDUTILITY ) )
	{
		char bundle_identifier[FOUNDATION_MAX_PATHLEN+1];
		app_bundle_identifier( bundle_identifier );
		
		char* path = path_append( path_merge( _environment_home_dir, "/Library/Application Support" ), bundle_identifier );
		string_copy( _environment_home_dir, path, FOUNDATION_MAX_PATHLEN );
		string_deallocate( path );
	}
#elif FOUNDATION_PLATFORM_ANDROID
	string_copy( _environment_home_dir, _global_app->activity->internalDataPath, FOUNDATION_MAX_PATHLEN );
#else
#  error Not implemented
#endif
	return _environment_home_dir;
}


const char* environment_temporary_directory( void )
{
	if( _environment_temp_dir[0] )
		return _environment_temp_dir;
#if FOUNDATION_PLATFORM_WINDOWS
	{
		char* path;
		wchar_t* wpath = memory_allocate_zero( sizeof( wchar_t ) * FOUNDATION_MAX_PATHLEN, 0, MEMORY_TEMPORARY );
		GetTempPathW( FOUNDATION_MAX_PATHLEN, wpath );
		path = path_clean( string_allocate_from_wstring( wpath, 0 ), true );
		string_copy( _environment_temp_dir, path, FOUNDATION_MAX_PATHLEN );
		string_deallocate( path );
		memory_deallocate( wpath );
	}
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
	string_copy( _environment_temp_dir, P_tmpdir, FOUNDATION_MAX_PATHLEN );
	unsigned int len = string_length( _environment_temp_dir );
	if( ( len > 1 ) && ( _environment_temp_dir[ len - 1 ] == '/' ) )
		_environment_temp_dir[ len - 1 ] = 0;
#else
#  error Not implemented
#endif
	return _environment_temp_dir;
}


const char* environment_environment_variable( const char* var )
{
#if FOUNDATION_PLATFORM_WINDOWS
	char* cval;
	wchar_t* key = wstring_allocate_from_string( var, 0 );
	wchar_t val[512]; val[0] = 0;
	GetEnvironmentVariableW( key, val, 512 );
	wstring_deallocate( key );
	cval = string_allocate_from_wstring( val, 0 );
	string_copy( _environment_var, cval, 512 );
	string_deallocate( cval );
	return _environment_var;
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
	return getenv( var );
#else
#  error Not implemented
#endif
}


const application_t* environment_application( void )
{
	return &_environment_app;
}
