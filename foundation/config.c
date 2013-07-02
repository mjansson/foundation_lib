/* config.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


#define BUILD_CONFIG_DEBUG        0

#define CONFIG_SECTION_BUCKETS    7
#define CONFIG_KEY_BUCKETS        11

typedef enum _foundation_config_value_type
{
	CONFIGVALUE_BOOL = 0,
	CONFIGVALUE_INT,
	CONFIGVALUE_REAL,
	CONFIGVALUE_STRING,
	CONFIGVALUE_STRING_CONST,
	CONFIGVALUE_STRING_VAR,
	CONFIGVALUE_STRING_CONST_VAR
} config_value_type_t;

typedef struct ALIGN(16) _foundation_config_key
{
	hash_t                  name;
	config_value_type_t     type;
	bool                    bval;
	int64_t                 ival;
	real                    rval;
	char*                   sval;
	char*                   expanded;
} config_key_t;

typedef struct ALIGN(16) _foundation_config_section
{
	hash_t                  name;
	config_key_t*           key[CONFIG_KEY_BUCKETS];
} config_section_t;

FOUNDATION_COMPILETIME_ASSERT( ( sizeof( config_key_t ) % 16 ) == 0, config_key_align );
FOUNDATION_COMPILETIME_ASSERT( ( sizeof( config_section_t ) % 16 ) == 0, config_section_align );

//Global config store
static config_section_t* _config_section[CONFIG_SECTION_BUCKETS] = {0};


static int64_t _config_string_to_int( const char* str )
{
	unsigned int length = string_length( str );
	unsigned int first_nonnumeric = 0;
	unsigned int dot_position = 0;
	if( length < 2 )
		return string_to_int64( str );

	first_nonnumeric = string_find_first_not_of( str, "0123456789.", 0 );
	if( ( first_nonnumeric == ( length - 1 ) ) && ( ( str[ first_nonnumeric ] == 'm' ) || ( str[ first_nonnumeric ] == 'M' ) ) )
	{
		dot_position = string_find( str, '.', 0 );
		if( dot_position != STRING_NPOS )
		{
			if( string_find( str, '.', dot_position + 1 ) == STRING_NPOS )
				return (int64_t)( string_to_real( str ) * ( REAL_C( 1024.0 ) * REAL_C( 1024.0 ) ) );
			return string_to_int64( str ); //More than one dot
		}
		return string_to_int64( str ) * ( 1024LL * 1024LL );
	}
	if( ( first_nonnumeric == ( length - 1 ) ) && ( ( str[ first_nonnumeric ] == 'k' ) || ( str[ first_nonnumeric ] == 'K' ) ) )
	{
		dot_position = string_find( str, '.', 0 );
		if( dot_position != STRING_NPOS )
		{
			 if( string_find( str, '.', dot_position + 1 ) == STRING_NPOS )
				return (int64_t)( string_to_real( str ) * REAL_C( 1024.0 ) );
			 return string_to_int64( str ); //More than one dot
		}
		return string_to_int64( str ) * 1024LL;
	}

	return string_to_int64( str );
}


static real _config_string_to_real( const char* str )
{
	unsigned int length = string_length( str );
	unsigned int first_nonnumeric = 0;
	unsigned int dot_position = 0;
	if( length < 2 )
		return string_to_real( str );

	first_nonnumeric = string_find_first_not_of( str, "0123456789.", 0 );
	if( ( first_nonnumeric == ( length - 1 ) ) && ( ( str[ first_nonnumeric ] == 'm' ) || ( str[ first_nonnumeric ] == 'M' ) ) )
	{
		dot_position = string_find( str, '.', 0 );
		if( dot_position != STRING_NPOS )
		{
			if( string_find( str, '.', dot_position + 1 ) != STRING_NPOS )
				return string_to_real( str ); //More than one dot
		}
		return string_to_real( str ) * ( REAL_C( 1024.0 ) * REAL_C( 1024.0 ) );
	}
	if( ( first_nonnumeric == ( length - 1 ) ) && ( ( str[ first_nonnumeric ] == 'k' ) || ( str[ first_nonnumeric ] == 'K' ) ) )
	{
		dot_position = string_find( str, '.', 0 );
		if( dot_position != STRING_NPOS )
		{
			if( string_find( str, '.', dot_position + 1 ) != STRING_NPOS )
				return string_to_real( str ); //More than one dot
		}
		return string_to_real( str ) * REAL_C( 1024.0 );
	}

	return string_to_real( str );
}


static NOINLINE char* _expand_string( hash_t section_current, char* str )
{
	char* expanded;
	char* variable;
	unsigned int var_pos, var_end_pos, variable_length, separator;
	hash_t section, key;

	expanded = str;
	var_pos = string_find_string( expanded, "$(", 0 );

	while( var_pos != STRING_NPOS )
	{
		var_end_pos = string_find( expanded, ')', var_pos + 2 );
		FOUNDATION_ASSERT_MSG( var_end_pos != STRING_NPOS, "Malformed config variable statement" );
		variable = string_substr( expanded, var_pos, ( var_end_pos != STRING_NPOS ) ? ( 1 + var_end_pos - var_pos ) : STRING_NPOS );

		section = section_current;
		key = 0;
		variable_length = string_length( variable );
		separator = string_find( variable, ':', 0 );
		if( separator != STRING_NPOS )
		{
			if( separator != 2 )
				section = hash( variable + 2, separator - 2 );
			key = hash( variable + separator + 1, variable_length - ( separator + 1 + ( variable[ variable_length - 1 ] == ')' ? 1 : 0 ) ) );
		}
		else
		{
			key = hash( variable + 2, variable_length - ( 2 + ( variable[ variable_length ] == ')' ? 1 : 0 ) ) );
		}

		if( expanded == str )
			expanded = string_clone( str );

		expanded = string_replace( expanded, variable, config_string( section, key ), false );
		string_deallocate( variable );

		var_pos = string_find_string( expanded, "$(", 0 );
	}
#if BUILD_CONFIG_DEBUG
	if( str != expanded )
		debug_logf( "Expanded config value \"%s\" to \"%s\"", str, expanded );
#endif

	return expanded;
}


static NOINLINE void _expand_string_val( hash_t section, config_key_t* key )
{
	bool is_true = false;
	FOUNDATION_ASSERT( key->sval );
	if( key->expanded != key->sval )
		string_deallocate( key->expanded );
	key->expanded = _expand_string( section, key->sval );

	is_true = string_equal( key->expanded, "true" );
	key->bval = ( string_equal( key->expanded, "false" ) || string_equal( key->expanded, "0" ) || !string_length( key->expanded ) ) ? false : true;
	key->ival = is_true ? 1 : _config_string_to_int( key->expanded );
	key->rval = is_true ? REAL_C(1.0) : _config_string_to_real( key->expanded );
}



int _config_initialize( void )
{
	config_load( "foundation", 0, true, false );
	config_load( "application", 0, true, false );

	//Load per-user config
	config_load( "user", HASH_USER, false, true );

	return 0;
}


void _config_shutdown( void )
{
	int isb, is, ikb, ik, ssize, ksize;
	config_section_t* section;
	config_key_t* key;
	for( isb = 0; isb < CONFIG_SECTION_BUCKETS; ++isb )
	{
		section = _config_section[isb];
		for( is = 0, ssize = array_size( section ); is < ssize; ++is )
		{
			for( ikb = 0; ikb < CONFIG_KEY_BUCKETS; ++ikb )
			{
				key = section[is].key[ikb];
				for( ik = 0, ksize = array_size( key ); ik < ksize; ++ik )
				{
					if( key[ik].expanded != key[ik].sval )
						string_deallocate( key[ik].expanded );
					if( ( key[ik].type != CONFIGVALUE_STRING_CONST ) && ( key[ik].type != CONFIGVALUE_STRING_CONST_VAR ) )
						string_deallocate( key[ik].sval );
				}
				array_deallocate( key );
			}
		}
		array_deallocate( section );
	}
}


void config_load( const char* name, hash_t filter_section, bool built_in, bool overwrite )
{
#define NUM_SEARCH_PATHS 10
#define ANDROID_ASSET_PATH_INDEX 5
	char* sub_exe_path = 0;
	char* exe_parent_path = 0;
	char* exe_processed_path = 0;
	char* abs_exe_parent_path = 0;
	char* abs_exe_processed_path = 0;
	char* bundle_path = 0;
	char* home_dir = 0;
	char* cmdline_path = 0;
	char* cwd_config_path = 0;
	const char* paths[NUM_SEARCH_PATHS];
	const char* const* cmd_line;
	int icl, clsize, start_path, i, j;

	const char buildsuffix[4][9] = { "/debug", "/release", "/profile", "/deploy" };
	const char platformsuffix[7][14] = { "/win32", "/win64", "/osx", "/ios", "/android", "/raspberrypi", "/unknown" };
	const char binsuffix[1][5] = { "/bin" };

	FOUNDATION_ASSERT( name );

	//Parse config file
	//
	//Look for config files in the following order in order to allow easy overloading of default values
	//(for each directory, a platform subdirectory is also searched)
	// 1) Executable directory (C:\path\bin\platform\build\exe)
	// 2) Executable directory "config" subdirectory (C:\path\bin\platform\build\config)
	// 3) Executable directory parent "config" subdirectory (C:path\bin\platform\config)
	// 4) Executable directory processed "config" subdirectory (C:\path\config)
	//    This step is performed by identifying if the path contains
	//      1) known directories (bin)
	//      2) a platform identifier (win32,win64,osx,ios,android,raspberrypi)
	//      3) a build identifier (debug,release,profile,deploy)
	//    and if so cleaning away these subdirectories. This allows you to have binaries
	//    sorted on platform/build under a common "bin" directory, while placing configs
	//    in a unified config directory outside the binary directory.
	//
	//For desktop builds (Windows, MacOSX, ...), if build is debug/release (i.e not deploy)
	// 5) Initial working directory
	//
	//For Android development
	// 6) Application asset manager config dir (/config)
	//For iOS & MacOSX development (non-bsdutil apps)
	// 6) Application bundle config dir (/path/to/exe.app/Contents/Resources/config)
	//Other platforms:
	// 6)
	//
	//For desktop platforms (Windows, MacOSX, ...)
	// 7) Current working directory (C:\current\dir)
	// 8) Current working directory "config" subdirectory (C:\current\dir\config)
	// 9) Command line --configdir directive
	//
	//If built_in flag is false, the following directories are also searched
	//For Windows development, the user app directory
	// 10) C:\Users\<username>\AppData\Local\<appname>
	//For Linux development, the user app directory
	// 10) /home/<username>/.<appname>
	//For MacOSX development, the user app directory
	// 10) /Users/<username>/.<appname>

	sub_exe_path = path_merge( environment_executable_directory(), "config" );
	exe_parent_path = path_merge( environment_executable_directory(), "../config" );
	abs_exe_parent_path = path_make_absolute( exe_parent_path );

	exe_processed_path = string_clone( environment_executable_directory() );
	for( i = 0; i < 4; ++i )
	{
		if( string_ends_with( exe_processed_path, buildsuffix[i] ) )
		{
			exe_processed_path[ string_length( exe_processed_path ) - string_length( buildsuffix[i] ) ] = 0;
			break;
		}
	}
	for( i = 0; i < 7; ++i )
	{
		if( string_ends_with( exe_processed_path, platformsuffix[i] ) )
		{
			exe_processed_path[ string_length( exe_processed_path ) - string_length( platformsuffix[i] ) ] = 0;
			break;
		}
	}
	for( i = 0; i < 1; ++i )
	{
		if( string_ends_with( exe_processed_path, binsuffix[i] ) )
		{
			exe_processed_path[ string_length( exe_processed_path ) - string_length( binsuffix[i] ) ] = 0;
			break;
		}
	}
	exe_processed_path = path_append( exe_processed_path, "config" );
	abs_exe_processed_path = path_make_absolute( exe_processed_path );
	
	paths[0] = environment_executable_directory();
	paths[1] = sub_exe_path;
	paths[2] = abs_exe_parent_path;
	paths[3] = abs_exe_processed_path;

#if FOUNDATION_PLATFORM_FAMILY_DESKTOP && !BUILD_DEPLOY
	paths[4] = environment_initial_working_directory();
#else
	paths[4] = 0;
#endif

#if FOUNDATION_PLATFORM_APPLE
	bundle_path = path_merge( environment_executable_directory(), "../Resources/config" );
	paths[5] = bundle_path;
#elif FOUNDATION_PLATFORM_ANDROID
	paths[5] = "/config";
#else
	paths[5] = 0;
#endif

#if FOUNDATION_PLATFORM_FAMILY_DESKTOP
	paths[6] = environment_current_working_directory();
#else
	paths[6] = 0;
#endif

	paths[7] = 0;
	paths[8] = 0;

	string_deallocate( exe_parent_path );
	string_deallocate( exe_processed_path );

#if FOUNDATION_PLATFORM_FAMILY_DESKTOP
	cwd_config_path = path_merge( environment_current_working_directory(), "config" );
	paths[7] = cwd_config_path;

	cmd_line = environment_command_line();
	for( icl = 0, clsize = array_size( cmd_line ); icl < clsize; ++icl )
	{
		if( string_equal_substr( cmd_line[icl], "--configdir", 11 ) )
		{
			if( string_equal_substr( cmd_line[icl], "--configdir=", 12 ) )
			{
				paths[8] = cmdline_path = string_substr( cmd_line[icl], 12, STRING_NPOS );
			}
			else if( icl < ( clsize - 1 ) )
			{
				paths[8] = cmdline_path = string_clone( cmd_line[++icl] );
			}
		}
	}
#endif
	
	start_path = 0;
	if( !built_in )
	{
#if FOUNDATION_PLATFORM_WINDOWS
		home_dir = path_merge( environment_home_directory(), environment_application()->config_dir );
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX
		home_dir = path_prepend( string_concat( ".", environment_application()->config_dir ), environment_home_directory() );
#endif
		if( home_dir )
			paths[9] = home_dir;
		start_path = 9;
	}
	else
	{
		paths[9] = 0;
	}

	for( i = start_path; i < NUM_SEARCH_PATHS; ++i )
	{
		char* filename;
		stream_t* istream;
		bool path_already_searched = false;

		if( !paths[i] )
			continue;

		for( j = start_path; j < i; ++j )
		{
			if( paths[j] && string_equal( paths[j], paths[i] ) )
			{
				path_already_searched = true;
				break;
			}
		}
		if( path_already_searched )
			continue;
		
		//TODO: Support loading configs from virtual file system (i.e in zip/other packages)
		filename = string_append( path_merge( paths[i], name ), ".ini" );
		istream = 0;
#if FOUNDATION_PLATFORM_ANDROID
		if( i == ANDROID_ASSET_PATH_INDEX )
			istream = asset_stream_open( filename, STREAM_IN );
		else
#endif
		istream = stream_open( filename, STREAM_IN );
		if( istream )
		{
			config_parse( istream, filter_section, overwrite );
			stream_deallocate( istream );
		}
		string_deallocate( filename );

		if( built_in )
		{
			const char* FOUNDATION_PLATFORM_name =
#if FOUNDATION_PLATFORM_WINDOWS
				"windows";
#elif FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
				"raspberrypi";
#elif FOUNDATION_PLATFORM_LINUX
				"linux";
#elif FOUNDATION_PLATFORM_MACOSX
				"osx";
#elif FOUNDATION_PLATFORM_IOS
				"ios";
#elif FOUNDATION_PLATFORM_ANDROID
				"android";
#else
#  error Insert platform name
				"unknown";
#endif
			filename = string_append( path_append( path_merge( paths[i], FOUNDATION_PLATFORM_name ), name ), ".ini" );
#if FOUNDATION_PLATFORM_ANDROID
			if( i == ANDROID_ASSET_PATH_INDEX )
				istream = asset_stream_open( filename, STREAM_IN );
			else
#endif
			istream = stream_open( filename, STREAM_IN );
			if( istream )
			{
				config_parse( istream, filter_section, overwrite );
				stream_deallocate( istream );
			}
			string_deallocate( filename );
		}
	}

	string_deallocate( home_dir );
	string_deallocate( cmdline_path );
	string_deallocate( sub_exe_path );
	string_deallocate( abs_exe_processed_path );
	string_deallocate( abs_exe_parent_path );
	string_deallocate( bundle_path );
	string_deallocate( cwd_config_path );
}


static NOINLINE config_section_t* config_section( hash_t section, bool create )
{
	config_section_t* bucket;
	int ib, bsize;

	bucket = _config_section[ section % CONFIG_SECTION_BUCKETS ];
	for( ib = 0, bsize = array_size( bucket ); ib < bsize; ++ib )
	{
		if( bucket[ib].name == section )
			return bucket + ib;
	}

	if( !create )
		return 0;

	//TODO: Thread safeness
	{
		config_section_t new_section = {0};
		new_section.name = section;

		array_push_memcpy( bucket, &new_section );
		_config_section[ section % CONFIG_SECTION_BUCKETS ] = bucket;
	}

	return bucket + bsize;
}


static NOINLINE config_key_t* config_key( hash_t section, hash_t key, bool create )
{
	config_key_t new_key = {0};
	config_section_t* csection;
	config_key_t* bucket;
	int ib, bsize;

	csection = config_section( section, create );
	if( !csection )
	{
		FOUNDATION_ASSERT( !create );
		return 0;
	}
	bucket = csection->key[ key % CONFIG_KEY_BUCKETS ];
	for( ib = 0, bsize = array_size( bucket ); ib < bsize; ++ib )
	{
		if( bucket[ib].name == key )
			return bucket + ib;
	}

	if( !create )
		return 0;

	new_key.name = key;

	//TODO: Thread safeness
	array_push_memcpy( bucket, &new_key );
	csection->key[ key % CONFIG_KEY_BUCKETS ] = bucket;

	return bucket + bsize;
}


bool config_bool( hash_t section, hash_t key )
{
	config_key_t* key_val = config_key( section, key, false );
	if( key_val && ( key_val->type >= CONFIGVALUE_STRING_VAR ) )
		_expand_string_val( section, key_val );
	return key_val ? key_val->bval : false;
}


int64_t config_int( hash_t section, hash_t key )
{
	config_key_t* key_val = config_key( section, key, false );
	if( key_val && ( key_val->type >= CONFIGVALUE_STRING_VAR ) )
		_expand_string_val( section, key_val );
	return key_val ? key_val->ival : 0;
}


real config_real( hash_t section, hash_t key )
{
	config_key_t* key_val = config_key( section, key, false );
	if( key_val && ( key_val->type >= CONFIGVALUE_STRING_VAR ) )
		_expand_string_val( section, key_val );
	return key_val ? key_val->rval : 0;
}


const char* config_string( hash_t section, hash_t key )
{
	config_key_t* key_val = config_key( section, key, false );
	if( !key_val )
		return "";
	//Convert to string
	switch( key_val->type )
	{
		case CONFIGVALUE_BOOL:  return key_val->bval ? "true" : "false";
		case CONFIGVALUE_INT:   if( !key_val->sval ) key_val->sval = string_from_int( key_val->ival, 0, 0 ); return key_val->sval;
		case CONFIGVALUE_REAL:  if( !key_val->sval ) key_val->sval = string_from_real( key_val->rval, 4, 0, '0' ); return key_val->sval;
		default: break;
	}
	//String value of some form
	if( !key_val->sval )
		return "";
	if( key_val->type >= CONFIGVALUE_STRING_VAR )
	{
		_expand_string_val( section, key_val );
		return key_val->expanded;
	}
	return key_val->sval;
}


hash_t config_string_hash( hash_t section, hash_t key )
{
	const char* value = config_string( section, key );
	return value ? hash( value, string_length( value ) ) : HASH_EMPTY_STRING;
}


void config_set_bool( hash_t section, hash_t key, bool value )
{
	config_key_t* key_val = config_key( section, key, true );
	FOUNDATION_ASSERT( key_val );
	key_val->bval = value;
	key_val->ival = ( value ? 1 : 0 );
	key_val->rval = ( value ? REAL_C( 1.0 ) : REAL_C( 0.0 ) );
	if( key_val->expanded != key_val->sval )
		string_deallocate( key_val->expanded );
	if( ( key_val->type != CONFIGVALUE_STRING_CONST ) && ( key_val->type != CONFIGVALUE_STRING_CONST_VAR ) )
		string_deallocate( key_val->sval );
	key_val->sval = 0;
	key_val->expanded = 0;
	key_val->type = CONFIGVALUE_BOOL;
}


void config_set_int( hash_t section, hash_t key, int64_t value )
{
	config_key_t* key_val = config_key( section, key, true );
	FOUNDATION_ASSERT( key_val );
	key_val->bval = value ? true : false;
	key_val->ival = value;
	key_val->rval = (real)value;
	if( key_val->expanded != key_val->sval )
		string_deallocate( key_val->expanded );
	if( ( key_val->type != CONFIGVALUE_STRING_CONST ) && ( key_val->type != CONFIGVALUE_STRING_CONST_VAR ) )
		string_deallocate( key_val->sval );
	key_val->sval = 0;
	key_val->expanded = 0;
	key_val->type = CONFIGVALUE_INT;
}


void config_set_real( hash_t section, hash_t key, real value )
{
	config_key_t* key_val = config_key( section, key, true );
	FOUNDATION_ASSERT( key_val );
	key_val->bval = !math_realzero( value );
	key_val->ival = (int64_t)value;
	key_val->rval = value;
	if( key_val->expanded != key_val->sval )
		string_deallocate( key_val->expanded );
	if( ( key_val->type != CONFIGVALUE_STRING_CONST ) && ( key_val->type != CONFIGVALUE_STRING_CONST_VAR ) )
		string_deallocate( key_val->sval );
	key_val->sval = 0;
	key_val->expanded = 0;
	key_val->type = CONFIGVALUE_REAL;
}


void config_set_string( hash_t section, hash_t key, const char* value )
{
	config_key_t* key_val = config_key( section, key, true );
	FOUNDATION_ASSERT( key_val );
	FOUNDATION_ASSERT( value );
	if( key_val->expanded != key_val->sval )
		string_deallocate( key_val->expanded );
	if( ( key_val->type != CONFIGVALUE_STRING_CONST ) && ( key_val->type != CONFIGVALUE_STRING_CONST_VAR ) )
		string_deallocate( key_val->sval );

	key_val->sval = string_clone( value );
	key_val->expanded = 0;
	key_val->type = ( ( string_find_string( key_val->sval, "$(", 0 ) != STRING_NPOS ) ? CONFIGVALUE_STRING_VAR : CONFIGVALUE_STRING );

	if( key_val->type == CONFIGVALUE_STRING )
	{
		bool is_true = string_equal( key_val->sval, "true" );
		key_val->bval = ( string_equal( key_val->sval, "false" ) || string_equal( key_val->sval, "0" ) || !string_length( key_val->sval ) ) ? false : true;
		key_val->ival = is_true ? 1 : _config_string_to_int( key_val->sval );
		key_val->rval = is_true ? REAL_C(1.0) : _config_string_to_real( key_val->sval );
	}
}


void config_set_string_constant( hash_t section, hash_t key, const char* value )
{
	config_key_t* key_val = config_key( section, key, true );
	FOUNDATION_ASSERT( key_val );
	FOUNDATION_ASSERT( value );
	if( key_val->expanded != key_val->sval )
		string_deallocate( key_val->expanded );
	if( ( key_val->type != CONFIGVALUE_STRING_CONST ) && ( key_val->type != CONFIGVALUE_STRING_CONST_VAR ) )
		string_deallocate( key_val->sval );
	//key_val->sval = (char*)value;
	memcpy( &key_val->sval, &value, sizeof( char* ) ); //Yeah yeah, we're storing a const pointer in a non-const var
	key_val->expanded = 0;
	key_val->type = ( ( string_find_string( key_val->sval, "$(", 0 ) != STRING_NPOS ) ? CONFIGVALUE_STRING_CONST_VAR : CONFIGVALUE_STRING_CONST );

	if( key_val->type == CONFIGVALUE_STRING_CONST )
	{
		bool is_true = string_equal( key_val->sval, "true" );
		key_val->bval = ( string_equal( key_val->sval, "false" ) || string_equal( key_val->sval, "0" ) || !string_length( key_val->sval ) ) ? false : true;
		key_val->ival = is_true ? 1 : _config_string_to_int( key_val->sval );
		key_val->rval = is_true ? REAL_C(1.0) : _config_string_to_real( key_val->sval );
	}
}


void config_parse( stream_t* stream, hash_t filter_section, bool overwrite )
{
	//Format: compatible with "standard" INI files (see http://en.wikipedia.org/wiki/INI_file)
	//[section]
	//name = value
	//; comment (or #comment)

	char* buffer;
	hash_t section = 0;
	hash_t key = 0;
	unsigned int line = 0;

#if BUILD_CONFIG_DEBUG
	debug_logf( "Parsing config stream: %s", stream_path( stream ) );
#endif
	buffer = memory_allocate_zero( 1024, 0, MEMORY_TEMPORARY );
	while( !stream_eos( stream ) )
	{
		++line;
		stream_read_line_buffer( stream, buffer, 1024, '\n' );
		string_strip( buffer, " \t\n\r" );
		if( !string_length( buffer ) || ( buffer[0] == ';' ) || ( buffer[0] == '#' ) )
			continue;
		if( buffer[0] == '[' )
		{
			//Section declaration
			unsigned int endpos = string_rfind( buffer, ']', string_length( buffer ) - 1 );
			if( ( endpos == STRING_NPOS ) || ( endpos < 1 ) )
			{
				log_warnf( WARNING_BAD_DATA, "Invalid section declaration on line %d in config stream '%s'", line, stream_path( stream ) );
				continue;
			}
			buffer[endpos] = 0;
			section = hash( buffer + 1, endpos - 1 );
#if BUILD_CONFIG_DEBUG
			debug_logf( "  config: section set to '%s' (0x%llx)", buffer + 1, section );
#endif
		}
		else if( !filter_section || ( filter_section == section ) )
		{
			//name=value declaration
			char* name;
			char* value;
			unsigned int separator = string_find( buffer, '=', 0 );
			if( separator == STRING_NPOS )
			{
				log_warnf( WARNING_BAD_DATA, "Invalid value declaration on line %d in config stream '%s', missing assignment operator '=': %s", line, stream_path( stream ), buffer );
				continue;
			}
			
			name = string_strip_substr( buffer, " \t", separator );
			value = string_strip( buffer + separator + 1, " \t" );
			if( !string_length( name ) )
			{
				log_warnf( WARNING_BAD_DATA, "Invalid value declaration on line %d in config stream '%s', empty name string", line, stream_path( stream ) );
				continue;
			}

			key = hash( name, string_length( name ) );

			if( overwrite || !config_key( section, key, false ) )
			{
#if BUILD_CONFIG_DEBUG
				debug_logf( "  config: %s (0x%llx) = %s", name, key, value );
#endif

				if( !string_length( value ) )
					config_set_string( section, key, "" );
				else if( string_equal( value, "false" ) )
					config_set_bool( section, key, false );
				else if( string_equal( value, "true" ) )
					config_set_bool( section, key, true );
				else if( ( string_find( value, '.', 0 ) != STRING_NPOS ) && ( string_find_first_not_of( value, "0123456789.", 0 ) == STRING_NPOS ) && ( string_find( value, '.', string_find( value, '.', 0 ) + 1 ) == STRING_NPOS ) ) //Exactly one "."
					config_set_real( section, key, string_to_real( value ) );
				else if( string_find_first_not_of( value, "0123456789", 0 ) == STRING_NPOS )
					config_set_int( section, key, string_to_int64( value ) );
				else
					config_set_string( section, key, value );
			}
		}
	}
	memory_deallocate( buffer );
}


void config_parse_commandline( const char* const* cmdline, unsigned int num )
{
	//TODO: Implement, format --section:key=value
	unsigned int arg;
	for( arg = 0; arg < num; ++arg )
	{
		if( string_match_pattern( cmdline[arg], "--*:*=*" ) )
		{
			unsigned int first_sep = string_find( cmdline[arg], ':', 0 );
			unsigned int second_sep = string_find( cmdline[arg], '=', 0 );
			if( ( first_sep != STRING_NPOS ) && ( second_sep != STRING_NPOS ) && ( first_sep < second_sep ) )
			{
				unsigned int section_length = first_sep - 2;
				unsigned int key_length = second_sep - first_sep - 1;

				const char* section_str = cmdline[arg] + 2;
				const char* key_str = cmdline[arg] + ( first_sep + 1 );
				
				hash_t section = hash( section_str, section_length );
				hash_t key = hash( key_str, key_length );
				
				char* value = string_substr( cmdline[arg], second_sep + 1, STRING_NPOS );
				char* set_value = value;
				
				unsigned int value_length = string_length( value );
				
				if( !value_length )
					config_set_string( section, key, "" );
				else if( string_equal( value, "false" ) )
					config_set_bool( section, key, false );
				else if( string_equal( value, "true" ) )
					config_set_bool( section, key, true );
				else if( ( string_find( value, '.', 0 ) != STRING_NPOS ) && ( string_find_first_not_of( value, "0123456789.", 0 ) == STRING_NPOS ) && ( string_find( value, '.', string_find( value, '.', 0 ) + 1 ) == STRING_NPOS ) ) //Exactly one "."
					config_set_real( section, key, string_to_real( value ) );
				else if( string_find_first_not_of( value, "0123456789", 0 ) == STRING_NPOS )
					config_set_int( section, key, string_to_int64( value ) );
				else
				{
					if( ( value_length > 1 ) && ( value[0] == '"' ) && ( value[ value_length - 1 ] == '"' ) )
					{
						value[ value_length - 1 ] = 0;
						set_value = value + 1;
						config_set_string( section, key, set_value );
					}
					else
					{
						config_set_string( section, key, value );
					}
				}

				log_infof( "Config value from command line: %.*s:%.*s = %s", section_length, section_str, key_length, key_str, set_value );
				
				string_deallocate( value );
			}	
		}
	}
}


void config_write( stream_t* stream, hash_t filter_section )
{
	config_section_t* csection;
	config_key_t* bucket;
	int key, ib, bsize;

	stream_set_binary( stream, false );

	if( true )//stream_is_sequential( stream ) )
	{
		stream_write_format( stream, "[%s]", hash_to_string( filter_section ) );
		stream_write_endl( stream );

		csection = config_section( filter_section, false );
		if( csection ) for( key = 0; key < CONFIG_KEY_BUCKETS; ++key )
		{
			bucket = csection->key[ key ];
			for( ib = 0, bsize = array_size( bucket ); ib < bsize; ++ib )
			{
				stream_write_format( stream, "\t%s\t\t\t\t= ", hash_to_string( bucket[ib].name ) );
				switch( bucket[ib].type )
				{
					case CONFIGVALUE_BOOL:
						stream_write_bool( stream, bucket[ib].bval );
						break;

					case CONFIGVALUE_INT:
						stream_write_int64( stream, bucket[ib].ival );
						break;

					case CONFIGVALUE_REAL:
#if FOUNDATION_PLATFORM_REALSIZE == 64
						stream_write_float64( stream, bucket[ib].rval );
#else
						stream_write_float32( stream, bucket[ib].rval );
#endif
						break;

					case CONFIGVALUE_STRING:
					case CONFIGVALUE_STRING_CONST:
					case CONFIGVALUE_STRING_VAR:
					case CONFIGVALUE_STRING_CONST_VAR:
						stream_write_string( stream, bucket[ib].sval );
						break;

					default:
						break;
				}
				stream_write_endl( stream );
			}
		}
	}
	else
	{
		//TODO: Update section if available, else append at end of stream
	}
}

