/* path.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <string.h>


char* path_clean( char* path, bool absolute )
{
	char* replace;
	char* inpath;
	char* next;
	unsigned int inlength, length, remain, protocollen, up, last_up, prev_up;

	if( !path )
		return 0;

	inpath = path;
	inlength = string_length( path );
	protocollen = string_find_string( path, "://", 0 );
	if( protocollen != STRING_NPOS )
	{
		protocollen += 3; //Also skip the "://" separator
		inlength -= protocollen;
		path += protocollen;
	}
	else
	{
		protocollen = 0;
	}
	length = inlength;

	replace = path;
	while( ( replace = strchr( replace, '\\' ) ) != 0 )
		*replace++ = '/';

	remain = length;
	replace = path;
	while( ( next = strstr( replace, "/./" ) ) != 0 )
	{
		remain -= (unsigned int)( next - replace ) + 2;
		length -= 2;
		memmove( next + 1, next + 3, remain ); //Include terminating zero to avoid looping when string ends in "/./"
		replace = next;
	}

	remain = length;
	replace = path;
	while( ( next = strstr( replace, "//" ) ) != 0 )
	{
		remain -= (unsigned int)( next - replace ) + 1;
		--length;
		memmove( next + 1, next + 2, remain ); //Include terminating zero to avoid looping when string ends in "//"
		replace = next;
	}

	path[length] = 0;

	if( string_equal( path, "." ) )
	{
		length = 0;
		path[0] = 0;
	}
	else if( length > 1 )
	{
		if( ( path[ length - 2 ] == '/' ) && ( path[ length - 1 ] == '.' ) )
		{
			path[ length - 2 ] = 0;
			length -= 2;
		}
		if( string_equal( path, "." ) )
		{
			length = 0;
			path[0] = 0;
		}
		else if( string_equal( path, "./" ) )
		{
			length = 1;
			path[0] = '/';
			path[1] = 0;
		}
		else if( ( length > 1 ) && ( path[0] == '.' ) && ( path[1] == '/' ) )
		{
			--length;
			memmove( path, path + 1, length );
			path[length] = 0;
		}
	}

	if( absolute )
	{
		if( !length )
		{
			if( !inlength )
			{
				inlength += 2;
				inpath = memory_reallocate( inpath, inlength + protocollen + 1, 0 );
				path = inpath + protocollen;
			}
			path[0] = '/';
			path[1] = 0;
			++length;
		}
#if FOUNDATION_PLATFORM_WINDOWS
		else if( ( length >= 2 ) && ( path[1] == ':' ) )
		{
			//Make sure first character is upper case
			if( ( path[0] >= 'a' ) && ( path[0] <= 'z' ) )
				path[0] = ( path[0] - (char)( (int)'a' - (int)'A' ) );

			if( length == 2 )
			{
				if( inlength <= 2 )
				{
					inlength += 2;
					inpath = memory_reallocate( inpath, inlength + protocollen + 1, 0 );
					path = inpath + protocollen;
				}
				path[2] = '/';
				++length;
			}
			else if( path[2] != '/' )
			{
				//splice in slash in weird-format paths (C:foo/bar/...)
				if( inlength < ( length + 1 ) )
				{
					//Need more space
					inlength = length + 1;
					inpath = memory_reallocate( inpath, length + protocollen + 2, 0 );
					path = inpath + protocollen;
				}

				memmove( path + 3, path + 2, length + 1 - 2 );
				path[2] = '/';
				++length;
			}
		}
#endif
		else if( path[0] != '/' )
		{
			//make sure capacity is enough to hold additional character
			if( inlength < ( length + 1 ) )
			{
				//Need more space
				inlength = length + 1;
				inpath = memory_reallocate( inpath, length + protocollen + 2, 0 );
				path = inpath + protocollen;
			}
			
			memmove( path + 1, path, length + 1 );
			path[0] = '/';
			++length;
		}
	}
	else //relative
	{
		if( length && ( path[0] == '/' ) )
		{
			memmove( path, path + 1, length - 1 );
			--length;
		}
	}

	//Deal with .. references
	last_up = 0;
	while( ( up = string_find_string( path, "/../", last_up ) ) != STRING_NPOS )
	{
		if( up >= length )
			break;
		if( up == 0 )
		{
			last_up = 3;
			continue;
		}
		prev_up = string_rfind( path, '/', up - 1 );
		if( ( prev_up != STRING_NPOS ) && ( prev_up > last_up ) )
		{
			memmove( path + prev_up, path + up + 3, length - up - 3 );
			length -= ( up - prev_up + 3 );
		}
		else
		{
			last_up = up + 1;
		}
	}

	if( length > 1 )
	{
		if( path[ length - 1 ] == '/' )
		{
			path[ length - 1 ] = 0;
			--length;
		}
	}

	if( protocollen )
	{
		if( ( length == 1 ) && ( path[0] == '/' ) )
			length = 0;
		length += protocollen;
		path = inpath;
	}

	path[length] = 0;

	return path;
}


char* path_base_file_name( const char* path )
{
	unsigned int start, end;
	if( !path )
		return 0;
	start = string_find_last_of( path, "/\\", STRING_NPOS );
	end = string_find( path, '.', ( start != STRING_NPOS ) ? start : 0 );
	//For "dot" files, i.e files with names like "/path/to/.file", return the dot name ".file"
	if( !end || ( end == ( start + 1 ) ) )
		end = STRING_NPOS;
	if( start != STRING_NPOS )
		return string_substr( path, start + 1, ( end != STRING_NPOS ) ? ( end - start - 1 ) : STRING_NPOS );
	return string_substr( path, 0, end );
}


char* path_base_file_name_with_path( const char* path )
{
	unsigned int end;
	char* base;
	if( !path )
		return 0;
	end = string_rfind( path, '.', STRING_NPOS );
	base = string_substr( path, 0, ( end != STRING_NPOS ) ? end : STRING_NPOS );
	base = path_clean( base, path_is_absolute( base ) );
	return base;
}


char* path_file_extension( const char* path )
{
	unsigned int end = string_rfind( path, '.', STRING_NPOS );
	if( end != STRING_NPOS )
		return string_substr( path, end + 1, STRING_NPOS );
	return 0;
}


char* path_file_name( const char* path )
{
	unsigned int end = string_find_last_of( path, "/\\", STRING_NPOS );
	if( end == STRING_NPOS )
		return string_clone( path );
	return string_substr( path, end + 1, STRING_NPOS );
}


char* path_path_name( const char* path )
{
	char* pathname;
	unsigned int end = string_find_last_of( path , "/\\", STRING_NPOS );
	if( end == 0 )
		return string_clone( "/" );
	if( end == STRING_NPOS )
		return 0;
	pathname = string_substr( path, 0, end );
	pathname = path_clean( pathname, path_is_absolute( pathname ) );
	return pathname;
}


char* path_subpath_name( const char* path, const char* root )
{
	char* subpath;
	char* cpath = string_clone( path );
	char* croot = string_clone( root );

	cpath = path_clean( cpath, path_is_absolute( cpath ) );
	croot = path_clean( croot, path_is_absolute( croot ) );
	
	if( strncmp( cpath, croot, strlen( croot ) ) != 0 )
		subpath = string_allocate( 0 );
	else
		subpath = string_substr( cpath, string_length( croot ), STRING_NPOS );
	subpath = path_clean( subpath, false );
	string_deallocate( cpath );
	string_deallocate( croot );

	return subpath;
}


char* path_protocol( const char* uri )
{
	unsigned int end = string_find_string( uri, "://", 0 );
	if( end == STRING_NPOS )
		return 0;
	return string_substr( uri, 0, end );
}


char* path_merge( const char* first, const char* second )
{
	char* merged;
	if( !first )
		merged = string_clone( second );
	else if( !second )
		merged = string_clone( first );
	else
	{
		merged = string_format( "%s/", first );
		merged = string_append( merged, second );
	}
	merged = path_clean( merged, path_is_absolute( first ) );
	return merged;
}


char* path_append( char* first, const char* second )
{
	if( !first )
		return 0;
	first = string_append( first, "/" );
	first = string_append( first, second );
	first = path_clean( first, path_is_absolute( first ) );
	return first;
}


bool path_is_absolute( const char* path )
{
	if( !path || !strlen( path ) )
		return false;
	if( strstr( path, "://" ) )
		return true;
	if( ( path[0] == '/' ) || ( path[0] == '\\' ) )
		return true;
#if FOUNDATION_PLATFORM_WINDOWS
	if( path[1] == ':' ) //Weird formats like "C:path/to/something"
		return true;
#endif
	return false;
}


char* path_make_absolute( const char* path )
{
	unsigned int up, last, length;
	char* abspath = string_clone( path );
	if( !path_is_absolute( abspath ) )
	{
		abspath = string_prepend( abspath, "/" );
		abspath = string_prepend( abspath, environment_current_working_directory() );
		abspath = path_clean( abspath, true );
	}
	else
	{
		abspath = path_clean( abspath, true );
	}
	
	//Deal with .. references
	while( ( up = string_find_string( abspath, "/../", 0 ) ) != STRING_NPOS )
	{
		char* subpath;
		if( up == 0 )
		{
			memmove( abspath, abspath + 4, strlen( abspath ) + 1 - 4 );
			continue;
		}
		last = string_rfind( abspath, '/', up - 1 );
		FOUNDATION_ASSERT( last != STRING_NPOS ); //Gotta be at least one since it's a cleaned absolute path
		subpath = string_substr( abspath, 0, last );
		subpath = string_append( subpath, abspath + up + 3 ); // +3 will include the / of the later part of the path
		string_deallocate( abspath );
		abspath = subpath;
	}

	length = string_length( abspath );
	if( length >= 3 )
	{
		while( ( length >= 3 ) && ( abspath[length-3] == '/' ) && ( abspath[length-2] == '.' ) && ( abspath[length-1] == L'.' ) )
		{
			//Step up
			if( length == 3 )
			{
				abspath[0] = 0;
				length = 0;
			}
			else
			{
				length = string_rfind( abspath, '/', length - 4 );
				abspath[length] = 0;
			}
		}
	}

	return abspath;
}


char* path_make_temporary( void )
{
	char uintbuffer[18];
	return path_append( path_merge( environment_temporary_directory(), "tmp" ), string_from_uint_buffer( uintbuffer, random64(), true, 0, '0' ) );
}

