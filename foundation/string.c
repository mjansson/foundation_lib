/* string.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if FOUNDATION_PLATFORM_WINDOWS
FOUNDATION_EXTERN errno_t _ctime64_s( char*, size_t, const __time64_t* );
#  if FOUNDATION_COMPILER_MSVC
#    define snprintf( p, s, ... ) _snprintf_s( p, s, _TRUNCATE, __VA_ARGS__ )
#    define vsnprintf( s, n, format, arg ) _vsnprintf_s( s, n, _TRUNCATE, format, arg )
#  endif
#elif FOUNDATION_PLATFORM_APPLE
FOUNDATION_EXTERN char* ctime_r( const time_t*, char* );
#elif FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL
#include <time.h>
#endif


char* string_allocate( size_t length )
{
	char* str = memory_allocate( HASH_STRING, length + 1, 0, MEMORY_PERSISTENT );
	str[0] = 0;
	return str;
}


void string_deallocate( char* str )
{
	memory_deallocate( str );
}


char* string_clone( const char* str )
{
	if( str )
	{
		size_t len = string_length( str ) + 1;
		char* clone = memory_allocate( HASH_STRING, len, 0, MEMORY_PERSISTENT );
		memcpy( clone, str, len );
		return clone;
	}
	return 0;
}


char* string_format( const char* format, ... )
{
	ssize_t n;
	size_t capacity;
	char* buffer;
	va_list list;

	if( !format )
		return 0;

	capacity = string_length( format ) + 32;
	buffer = memory_allocate( HASH_STRING, capacity + 1, 0, MEMORY_PERSISTENT );

	while( 1 )
	{
		va_start( list, format );
		n = vsnprintf( buffer, capacity, format, list );
		va_end( list );

		if( ( n > -1 ) && ( (size_t)n < capacity ) )
			break;

		if( n > -1 )
			capacity = (size_t)n + 1;
		else
			capacity *= 2;

		buffer = memory_reallocate( buffer, capacity + 1, 0, 0 );
	}

	return buffer;
}


char* string_format_buffer( char* buffer, size_t maxlen, const char* format, ... )
{
	va_list list;

	if( !buffer )
		return 0;
	if( !format )
	{
		buffer[0] = 0;
		return buffer;
	}

	va_start( list, format );
	vsnprintf( buffer, maxlen, format, list );
	va_end( list );

	return buffer;
}


char* string_vformat( const char* format, va_list list )
{
	ssize_t n;
	size_t capacity;
	char* buffer;
	va_list copy_list;

	if( !format )
		return 0;

	capacity = string_length( format ) + 32;
	buffer = memory_allocate( HASH_STRING, capacity + 1, 0, MEMORY_PERSISTENT );

	while( 1 )
	{
		va_copy( copy_list, list );
		n = vsnprintf( buffer, capacity, format, copy_list );
		va_end( copy_list );

		if( ( n > -1 ) && ( (size_t)n < capacity ) )
			break;

		if( n > -1 )
			capacity = (size_t)n + 1;
		else
			capacity *= 2;

		buffer = memory_reallocate( buffer, capacity + 1, 0, 0 );
	}

	return buffer;
}


char* string_vformat_buffer( char* buffer, size_t maxlen, const char* format, va_list list )
{
	va_list copy_list;

	if( !buffer )
		return 0;
	if( !format )
	{
		buffer[0] = 0;
		return buffer;
	}

	va_copy( copy_list, list );
	vsnprintf( buffer, maxlen, format, copy_list );
	va_end( copy_list );

	return buffer;
}


size_t string_length( const char* str )
{
	return str ? strlen( str ) : 0;
}


hash_t string_hash( const char* str )
{
	return str ? hash( str, string_length( str ) ) : HASH_EMPTY_STRING;
}


char* string_resize( char* str, size_t length, char c )
{
	size_t curlen = string_length( str );

	if( curlen < length )
	{
		str = memory_reallocate( str, length + 1, 0, curlen );
		memset( str + curlen, c, length - curlen );
	}
	if( str )
		str[length] = 0;
	return str;
}


void string_copy( char* dst, const char* src, size_t limit )
{
	size_t length = string_length( src );
	if( length >= limit )
		length = math_max( limit, 1 ) - 1;
	if( dst )
	{
		memcpy( dst, src, length );
		dst[length] = 0;
	}
}


char* string_strip( char* str, const char* delimiters )
{
	size_t start, end, length, newlength;

	if( !str )
		return 0;

	length = string_length( str );
	start = string_find_first_not_of( str, delimiters, 0 );
	end   = string_rfind_first_not_of( str, delimiters, length - 1 );

	if( start != STRING_NPOS )
	{
		if( end == STRING_NPOS )
			end = length - 1;
		newlength = end - start + 1;
		if( start != 0 )
			memmove( str, str + start, newlength );
		str[newlength] = 0;
	}
	else
	{
		str[0] = 0;
	}

	return str;
}


char* string_strip_substr( char* str, const char* delimiters, size_t length )
{
	size_t start, end, newlength;

	if( !str || ( length <= 0 ) )
		return 0;

	start = string_find_first_not_of( str, delimiters, 0 );
	end   = string_rfind_first_not_of( str, delimiters, length - 1 );

	if( ( start != STRING_NPOS ) && ( start < length ) )
	{
		if( end == STRING_NPOS )
			end = length - 1;
		newlength = end - start + 1;
		if( start != 0 )
			memmove( str, str + start, newlength );
		str[newlength] = 0;
	}
	else
	{
		str[0] = 0;
	}

	return str;
}


char* string_replace( char* str, const char* key, const char* newkey, bool repeat )
{
	size_t pos, lastpos, keylen, newkeylen, slen, replaced;
	ssize_t lendiff;

	slen = string_length( str );
	keylen = string_length( key );
	if( !slen || !keylen || string_equal( key, newkey ) )
		return str;

	lastpos = STRING_NPOS;
	newkeylen = newkey ? string_length( newkey ) : 0;
	lendiff = (ssize_t)newkeylen - (ssize_t)keylen;
	pos = 0;
	replaced = 0;

	while( ( pos = string_find_string( str, key, pos ) ) != STRING_NPOS )
	{
		if( repeat && ( lastpos != STRING_NPOS ) && ( lendiff > 0 ) && ( pos <= ( lastpos + (size_t)lendiff ) ) )
		{
			//Avoid infinite loop - found position did not move ahead more than
			//newly introduced characters in the remaining part of the string
			pos = lastpos + newkeylen;
			continue;
		}

		if( lendiff <= 0 )
		{
			//String is reducing or keeping length, just overwrite
			memcpy( str + pos, newkey, newkeylen );
			if( lendiff < 0 )
			{
				memmove( str + pos + newkeylen, str + pos + keylen, slen - pos - keylen + 1 );
				FOUNDATION_ASSERT( slen >= (size_t)(-lendiff) );
				slen -= (size_t)(-lendiff);
			}
		}
		else
		{
			str = memory_reallocate( str, slen + (size_t)lendiff + 1, 0, slen + 1 );
			memmove( str + pos + newkeylen, str + pos + keylen, slen - pos - keylen + 1 );
			memcpy( str + pos, newkey, newkeylen );
			slen += (size_t)lendiff;
		}

		++replaced;

		lastpos = pos;
		if( !repeat )
			pos += newkeylen;
	}

	if( replaced )
		str[ slen ] = 0;

	return str;
}


char* string_append( char* str, const char* suffix )
{
	size_t slen = str ? string_length( str ) : 0;
	size_t suffixlen = suffix ? string_length( suffix ) : 0;
	size_t totallen = slen + suffixlen;
	if( !suffixlen )
		return str;

	str = str ? memory_reallocate( str, totallen + 1, 0, slen ) : memory_allocate( HASH_STRING, totallen + 1, 0, MEMORY_PERSISTENT );
	memcpy( str + slen, suffix, suffixlen + 1 ); //Include terminating zero

	return str;
}


char* string_prepend( char* str, const char* prefix )
{
	size_t slen = str ? string_length( str ) : 0;
	size_t prefixlen = prefix ? string_length( prefix ) : 0;
	size_t totallen = slen + prefixlen;
	if( !prefixlen )
		return str;

	str = str ? memory_reallocate( str, totallen + 1, 0, slen + 1 ) : memory_allocate( HASH_STRING, totallen + 1, 0, MEMORY_PERSISTENT );
	if( slen )
		memmove( str + prefixlen, str, slen + 1 ); //Include terminating zero
	memcpy( str, prefix, prefixlen );
	if( !slen )
		str[prefixlen] = 0;

	return str;
}


char* string_concat( const char* lhs, const char* rhs )
{
	size_t llen = string_length( lhs );
	size_t rlen = string_length( rhs );
	char* buf = memory_allocate( HASH_STRING, llen + rlen + 1, 0, MEMORY_PERSISTENT );
	if( llen )
		memcpy( buf, lhs, llen );
	if( rlen )
		memcpy( buf + llen, rhs, rlen );
	buf[ llen + rlen ] = 0;
	return buf;
}


void string_split( const char* str, const char* separators, char** left, char** right, bool allowempty )
{
	size_t start, delim;

	start = ( allowempty ? 0 : string_find_first_not_of( str, separators, 0 ) );
	if( start == STRING_NPOS )
	{
		if( left )
			*left = 0;
		if( right )
			*right = 0;
		return;
	}

	delim = string_find_first_of( str, separators, start );
	if( delim != STRING_NPOS )
	{
		if( left )
			*left = string_substr( str, start, delim - start );
		if( right )
		{
			delim = ( allowempty ? ( delim + 1 ) : string_find_first_not_of( str, separators, delim ) );
			if( delim != STRING_NPOS )
				*right = string_substr( str, delim, STRING_NPOS );
			else
				*right = 0;
		}
	}
	else
	{
		if( left )
			*left = string_substr( str, start, STRING_NPOS );
		if( right )
			*right = 0;
	}
}


char* string_substr( const char* str, size_t offset, size_t length )
{
	char* buffer;
	size_t newlen;
	size_t slen = string_length( str );
	if( !str || ( offset >= slen ) || !slen )
		return string_allocate( 0 );
	newlen = slen - offset;
	if( length < newlen )
		newlen = length;
	buffer = memory_allocate( HASH_STRING, newlen + 1, 0, MEMORY_PERSISTENT );
	memcpy( buffer, str + offset, newlen );
	buffer[newlen] = 0;
	return buffer;
}


size_t string_find( const char* str, char c, size_t offset )
{
	const char* found;
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		return STRING_NPOS;
	found = strchr( str + offset, c );
	if( found )
		return (size_t)pointer_diff( found, str );
	return STRING_NPOS;
}


size_t string_find_string( const char* str, const char* key, size_t offset )
{
	const char* found;
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		return STRING_NPOS;
	found = strstr( str + offset, key );
	if( found )
		return (size_t)pointer_diff( found, str );
	return STRING_NPOS;
}


size_t string_rfind( const char* str, char c, size_t offset )
{
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		offset = string_length( str ) - 1;

	if( offset != STRING_NPOS ) do
	{
		if( c == str[ offset ] )
			return offset;
		--offset;
	} while( offset != STRING_NPOS ); //Wrap-around terminates

	return STRING_NPOS;
}


size_t string_rfind_string( const char* str, const char* key, size_t offset )
{
	size_t keylen, slen;

	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );

	slen = string_length( str );
	keylen = string_length( key );
	if( ( slen > 0 ) && ( keylen <= slen ) )
	{
		if( offset > slen )
			offset = slen;
		if( !keylen )
			return ( offset <= slen ? offset : slen );

		do
		{
			if( !strncmp( str + offset, key, keylen ) )
				return offset;
			--offset;
		} while( offset != STRING_NPOS );
	}
	return STRING_NPOS;
}


size_t string_find_first_of( const char* str, const char* tokens, size_t offset )
{
	const char* found;
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		return STRING_NPOS;
	found = strpbrk( str + offset, tokens );
	if( found )
		return (size_t)pointer_diff( found, str );
	return STRING_NPOS;
}


size_t string_find_last_of( const char* str, const char* tokens, size_t offset )
{
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		offset = string_length( str ) - 1;

	if( offset != STRING_NPOS ) do
	{
		if( strchr( tokens, str[ offset ] ) && str[ offset ] )
			return offset;
		--offset;
	} while( offset != STRING_NPOS ); //Wrap-around terminates

	return STRING_NPOS;
}


size_t string_find_first_not_of( const char* str, const char* tokens, size_t offset )
{
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		return STRING_NPOS;
	while( str[ offset ] )
	{
		if( !strchr( tokens, str[ offset ] ) )
			return offset;
		++offset;
	}

	return STRING_NPOS;
}


size_t string_find_last_not_of( const char* str, const char* tokens, size_t offset )
{
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		offset = string_length( str ) - 1;

	if( offset != STRING_NPOS ) do
	{
		if( !strchr( tokens, str[ offset ] ) )
			return offset;
		--offset;
	} while( offset != STRING_NPOS ); //Wrap-around terminates

	return STRING_NPOS;
}


size_t string_rfind_first_of( const char* str, const char* tokens, size_t offset )
{
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		offset = string_length( str ) - 1;

	if( offset != STRING_NPOS ) do
	{
		if( strchr( tokens, str[offset] ) )
			return offset;
		--offset;
	} while( offset != STRING_NPOS ); //Wrap-around terminates

	return STRING_NPOS;
}


size_t string_rfind_first_not_of( const char* str, const char* tokens, size_t offset )
{
	FOUNDATION_ASSERT( ( offset == STRING_NPOS ) || ( offset <= string_length( str ) ) );
	if( offset == STRING_NPOS )
		offset = string_length( str ) - 1; //Wrap-around caught by if clause below

	if( offset != STRING_NPOS ) do
	{
		if( !strchr( tokens, str[offset] ) )
			return offset;
		--offset;
	} while( offset != STRING_NPOS ); //Wrap-around terminates

	return STRING_NPOS;
}


bool string_ends_with( const char* str, const char* suffix )
{
	size_t len = string_length( str );
	size_t suffix_len = string_length( suffix );
	if( len < suffix_len )
		return false;
	return string_equal( str + ( len - suffix_len ), suffix );
}


bool string_equal( const char* rhs, const char* lhs )
{
	return ( rhs == lhs ) || ( rhs && lhs && ( strcmp( rhs, lhs ) == 0 ) ) || ( !rhs && lhs && lhs[0] == 0 ) || ( rhs && !lhs && rhs[0] == 0 );
}


bool string_equal_substr( const char* rhs, const char* lhs, size_t len )
{
	return ( rhs == lhs ) || ( rhs && lhs && ( strncmp( rhs, lhs, len ) == 0 ) ) || ( !rhs && lhs && ( !len || lhs[0] == 0 ) ) || ( rhs && !lhs && ( !len || rhs[0] == 0 ) );
}


bool string_match_pattern( const char* element, const char* pattern )
{
	if( ( *pattern == '*') && !pattern[1] )
		return true;
	else if( !*element && *pattern )
		return false;
	else if( !*element )
		return true;

	if( *pattern == '*' )
	{
		if( string_match_pattern( element, pattern + 1) )
			return true;
		return string_match_pattern( element + 1, pattern );
	}
	else if( *pattern == '?' )
		return string_match_pattern( element + 1, pattern + 1 );
	else if( *element == *pattern )
		return string_match_pattern( element + 1, pattern + 1 );

	return false;
}


char** string_explode( const char* str, const char* delimiters, bool allow_empty )
{
	char** array;
	size_t slen;
	size_t delimlen;
	size_t token;
	size_t end;

	slen = string_length( str );
	if( !slen )
		return 0;

	array = 0;
	delimlen = string_length( delimiters );
	if( !delimlen )
	{
		array_push( array, string_clone( str ) );
		return array;
	}

	token = 0;
	end   = 0;

	while( end < slen )
	{
		if( !allow_empty )
			token = string_find_first_not_of( str, delimiters, end );
		end = string_find_first_of( str, delimiters, token );

		if( token != STRING_NPOS )
			array_push( array, string_substr( str, token, ( end != STRING_NPOS ) ? ( end - token ) : STRING_NPOS ) );
		if( allow_empty )
			token = end + 1;
	}

	return array;
}


char* string_merge( const char* const* array, size_t num, const char* delimiter )
{
	char* result;
	size_t i;

	if( num <= 0 )
		return string_allocate( 0 );

	result = string_clone( array[0] );
	for( i = 1; i < num; ++i )
	{
		result = string_append( result, delimiter );
		result = string_append( result, array[i] );
	}

	return result;
}


void string_array_deallocate_elements( char** array )
{
	size_t i, size = array_size( array );
	for( i = 0; i < size; ++i )
		string_deallocate( array[i] );
}


ssize_t string_array_find( const char* const* array, const char* needle, size_t haystack_size )
{
	size_t i;
	for( i = 0; i < haystack_size; ++i, ++array )
	{
		if( string_equal( *array, needle ) )
			return (ssize_t)i;
	}
	return -1;
}



#define get_bit_mask( numbits ) ( ( 1U << (numbits) ) - 1 )


static size_t get_num_bytes_utf8( uint8_t lead )
{
	if(      ( lead & 0xFC ) == 0xF8 ) return 5;
	else if( ( lead & 0xF8 ) == 0xF0 ) return 4;
	else if( ( lead & 0xF0 ) == 0xE0 ) return 3;
	else if( ( lead & 0xE0 ) == 0xC0 ) return 2;
	else return 1;
}


static size_t get_num_bytes_as_utf8( uint32_t val )
{
	if(      val >= 0x04000000 ) return 6;
	else if( val >= 0x00200000 ) return 5;
	else if( val >= 0x00010000 ) return 4;
	else if( val >= 0x00000800 ) return 3;
	else if( val >= 0x00000080 ) return 2;
	return 1;
}


static size_t encode_utf8( char* str, uint32_t val )
{
	size_t num, j;

	if( val < 0x80 )
	{
		*str = (char)val;
		return 1;
	}

	//Get number of _extra_ bytes
	num = get_num_bytes_as_utf8( val ) - 1;

	*str++ = (char)( ( 0x80U | ( get_bit_mask( num ) << ( 7 - num ) ) ) | ( ( val >> ( 6 * num ) ) & get_bit_mask( 6U - num ) ) );
	for( j = 1; j <= num; ++j )
		*str++ = (char)( 0x80U | ( ( val >> ( 6 * ( num - j ) ) ) & 0x3F ) );

	return num + 1;
}


uint32_t string_glyph( const char* str, size_t offset, size_t* consumed )
{
	uint32_t glyph = 0;
	size_t num, j;
	const char* cur = str + offset;

	if( !( *cur & 0x80 ) )
	{
		glyph = (uint32_t)*cur++;
		if( consumed )
			*consumed = 1;
	}
	else
	{
		//Convert through UTF-32
		num = get_num_bytes_utf8( (uint8_t)(*cur) ) - 1; //Subtract one to get number of _extra_ bytes
		glyph = ( (uint32_t)(*cur) & get_bit_mask( 6 - num ) ) << ( 6 * num );
		++cur;
		for( j = 1; j <= num; ++j, ++cur )
			glyph |= ( (uint32_t)(*cur) & 0x3F ) << ( 6 * ( num - j ) );
		if( consumed )
			*consumed = num + 1;
	}
	return glyph;
}


size_t string_glyphs( const char* str )
{
	size_t num = 0;
	while( *str )
	{
		++num;
		str += get_num_bytes_utf8( (uint8_t)(*str) );
	}
	return num;
}


wchar_t* wstring_allocate_from_string( const char* cstr, size_t length )
{
	wchar_t* buffer;
	wchar_t* dest;
	size_t maxlen, num_chars, num_bytes, i, j;
	uint32_t glyph;
	const char* cur;

	if( !cstr )
	{
		buffer = memory_allocate( HASH_STRING, sizeof( wchar_t ), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED );
		return buffer;
	}

	maxlen = string_length( cstr );
	length = ( length && length < maxlen ) ? length : maxlen;

	//Count number of wchar_t needed to represent string
	num_chars = 0;
	cur = cstr;
	for( i = 0; i < length; )
	{
		num_bytes = get_num_bytes_utf8( (uint8_t)(*cur) );
#if FOUNDATION_SIZE_WCHAR == 2
		if( num_bytes >= 4 )
			num_chars += 2; // final glyph > 0xFFFF
		else
			++num_chars;
#else
		++num_chars; //wchar_t == UTF-32
#endif
		cur += num_bytes;
		i += num_bytes;
	}

	buffer = memory_allocate( HASH_STRING, sizeof( wchar_t ) * ( num_chars + 1 ), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED );

	dest = buffer;
	cur = cstr;
	for( i = 0; i < length; ++i )
	{
		if( !( *cur & 0x80 ) )
			*dest++ = *cur++;
		else
		{
			//Convert through UTF-32
			num_bytes = get_num_bytes_utf8( (uint8_t)(*cur) ) - 1; //Subtract one to get number of _extra_ bytes
			glyph = ( (uint32_t)(*cur) & get_bit_mask( 6 - num_bytes ) ) << ( 6 * num_bytes );
			++cur;
			for( j = 1; j <= num_bytes; ++j, ++cur )
				glyph |= ( (uint32_t)(*cur) & 0x3F ) << ( 6 * ( num_bytes - j ) );
			if( sizeof( wchar_t ) == 2 )
			{
				FOUNDATION_ASSERT( ( glyph < 0xD800 ) || ( glyph > 0xDFFF ) );
				FOUNDATION_ASSERT( glyph <= 0x10FFFF );
				if( ( glyph < 0xD800 ) || ( glyph > 0xDFFF ) )
				{
					if( glyph <= 0xFFFF )
						*dest++ = (uint16_t)glyph;
					else if( glyph <= 0x10FFFF )
					{
						uint32_t val = glyph - 0x10000;
						*dest++ = 0xD800 | ( ( val >> 10 ) & 0x3FF );
						*dest++ = 0xDC00 | (   val         & 0x3FF );
					}
				}
			}
			else
			{
				*dest++ = (wchar_t)glyph;
			}
			i += num_bytes;
		}
	}

	*dest = 0;

	return buffer;
}


void wstring_from_string( wchar_t* dest, const char* source, size_t max_length, size_t length )
{
	size_t i, j, num, max_srclength;
	uint32_t glyph;
	wchar_t* last = dest + ( max_length - 2 );
	const char* cur = source;

	max_srclength = string_length( source );
	length = ( length < max_srclength ) ? length : max_srclength;

	for( i = 0; ( i < length ) && ( dest < last ); ++i )
	{
		if( !( *cur & 0x80 ) )
			*dest++ = *cur++;
		else
		{
			//Convert through UTF-32
			num = get_num_bytes_utf8( (uint8_t)(*cur) ) - 1; //Subtract one to get number of _extra_ bytes
			glyph = ( (uint32_t)(*cur) & get_bit_mask( 6 - num ) ) << ( 6 * num );
			++cur;
			for( j = 1; j <= num; ++j, ++cur )
				glyph |= ( (uint32_t)(*cur) & 0x3F ) << ( 6 * ( num - j ) );
#if FOUNDATION_SIZE_WCHAR == 2
			FOUNDATION_ASSERT( ( glyph < 0xD800 ) || ( glyph > 0xDFFF ) );
			FOUNDATION_ASSERT( glyph <= 0x10FFFF );
			if( ( glyph < 0xD800 ) || ( glyph > 0xDFFF ) )
			{
				if( glyph <= 0xFFFF )
					*dest++ = (uint16_t)glyph;
				else if( glyph <= 0x10FFFF )
				{
					uint32_t val = glyph - 0x10000;
					*dest++ = 0xD800 | ( ( val >> 10 ) & 0x3FF );
					*dest++ = 0xDC00 | (   val         & 0x3FF );
				}
			}
#else
			*dest++ = (wchar_t)glyph;
#endif
			i += num;
		}
	}

	if( max_length > 0 )
		*dest = 0;
}


void wstring_deallocate( wchar_t* str )
{
	memory_deallocate( str );
}


size_t wstring_length( const wchar_t* str )
{
	return wcslen( str );
}


bool wstring_equal( const wchar_t* lhs, const wchar_t* rhs )
{
	return wcscmp( lhs, rhs ) == 0;
}


static size_t _string_length_utf16( const uint16_t* p_str )
{
	size_t len = 0;
	if( !p_str )
		return 0;
	while( *p_str )
	{
		++len;
		++p_str;
	}
	return len;
}


static size_t _string_length_utf32( const uint32_t* p_str )
{
	size_t len = 0;
	if( !p_str )
		return 0;
	while( *p_str )
	{
		++len;
		++p_str;
	}
	return len;
}


char* string_allocate_from_wstring( const wchar_t* str, size_t length )
{
#if FOUNDATION_SIZE_WCHAR == 2
	return string_allocate_from_utf16( (const uint16_t*)str, length );
#else
	return string_allocate_from_utf32( (const uint32_t*)str, length );
#endif
}


char* string_allocate_from_utf16( const uint16_t* str, size_t length )
{
	bool swap;
	char* buf;
	size_t i, curlen, inlength, maxlen;
	uint32_t glyph, lval;

	maxlen = _string_length_utf16( str );
	length = ( length && length < maxlen ) ? length : maxlen;

	inlength = length;
	curlen = 0;

	swap = false;
	for( i = 0; i < inlength; ++i )
	{
		glyph = str[i];
		if( ( glyph == 0xFFFE ) || ( glyph == 0xFEFF ) )
		{
			swap = ( glyph != 0xFEFF );
			continue; //BOM
		}
		if( swap )
			glyph = byteorder_swap16( (uint16_t)glyph );
		if( ( glyph >= 0xD800 ) && ( glyph <= 0xDFFF ) )
		{
			++i;
			lval = str[i];
			if( swap )
				lval = byteorder_swap16( (uint16_t)lval );
			glyph = ( ( ( ( glyph & 0x3FF ) << 10 ) | ( lval & 0x3FF ) ) + 0x10000 );
		}

		curlen += get_num_bytes_as_utf8( glyph );
	}

	buf = memory_allocate( HASH_STRING, ( curlen + 1 ), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED );

	string_convert_utf16( buf, str, curlen + 1, inlength );

	return buf;
}


char* string_allocate_from_utf32( const uint32_t* str, size_t length )
{
	bool swap;
	char* buf;
	size_t i, curlen, inlength, maxlen;
	uint32_t glyph;

	maxlen = _string_length_utf32( str );
	length = ( length && length < maxlen ) ? length : maxlen;

	inlength = length;
	curlen = 0;

	swap = false;
	for( i = 0; i < inlength; ++i )
	{
		glyph = str[i];
		if( ( glyph == 0x0000FEFF ) || ( glyph == 0xFFFE0000 ) )
		{
			swap = ( glyph != 0x0000FEFF );
			continue; //BOM
		}
		if( swap )
			glyph = byteorder_swap32( glyph );
		curlen += get_num_bytes_as_utf8( glyph );
	}

	buf = memory_allocate( HASH_STRING, ( curlen + 1 ), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED );

	string_convert_utf32( buf, str, curlen + 1, inlength );

	return buf;
}


void string_convert_utf16( char* dst, const uint16_t* src, size_t dstsize, size_t srclength )
{
	bool swap = false;
	uint32_t glyph, lval;
	size_t curlen = 0, numbytes = 0;
	size_t i;

	for( i = 0; ( i < srclength ) && ( curlen < dstsize ); ++i )
	{
		//Convert through full UTF-32
		glyph = src[i];
		if( ( glyph == 0xFFFE ) || ( glyph == 0xFEFF ) )
		{
			swap = ( glyph != 0xFEFF );
			continue; //BOM
		}
		if( swap )
			glyph = byteorder_swap16( (uint16_t)glyph );
		if( ( glyph >= 0xD800 ) && ( glyph <= 0xDFFF ) )
		{
			++i;
			lval = src[i];
			if( swap )
				lval = byteorder_swap16( (uint16_t)lval );
			glyph = ( ( ( ( glyph & 0x3FF ) << 10 ) | ( lval & 0x3FF ) ) + 0x10000 );
		}

		numbytes = get_num_bytes_as_utf8( glyph );
		if( ( curlen + numbytes ) < dstsize )
			curlen += encode_utf8( dst + curlen, glyph );
	}

	dst[curlen] = 0;
}


void string_convert_utf32( char* dst, const uint32_t* src, size_t dstsize, size_t srclength )
{
	bool swap = false;
	uint32_t glyph;
	size_t curlen = 0, numbytes = 0;
	size_t i;

	swap = false;
	for( i = 0; ( i < srclength ) && ( curlen < dstsize ); ++i )
	{
		glyph = src[i];
		if( ( glyph == 0x0000FEFF ) || ( glyph == 0xFFFE0000 ) )
		{
			swap = ( glyph != 0x0000FEFF );
			continue; //BOM
		}
		if( swap )
			glyph = byteorder_swap32( glyph );

		numbytes = get_num_bytes_as_utf8( glyph );
		if( ( curlen + numbytes ) < dstsize )
			curlen += encode_utf8( dst + curlen, glyph );
	}

	dst[curlen] = 0;
}

#define THREAD_BUFFER_SIZE 128
FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( char, convert_buffer, THREAD_BUFFER_SIZE )


char* string_from_int( int64_t val, unsigned int width, char fill )
{
	char buf[32];
	return string_clone( string_from_int_buffer( buf, 32, val, width, fill ) );
}


char* string_from_int_buffer( char* buffer, size_t size, int64_t val, unsigned int width, char fill )
{
	int len = snprintf( buffer, size, "%" PRId64, val );
	if( len < 0 )
	{
		if( size )
			buffer[0] = 0;
		return buffer;
	}
	if( (size_t)len > size )
	{
		if( size )
			buffer[size-1] = 0;
		return buffer;
	}
	if( (unsigned int)len < width )
	{
		size_t diff = (size_t)width - (size_t)len;
		memmove( buffer + diff, buffer, (size_t)len + 1 );
		memset( buffer, fill, diff );
	}
	return buffer;
}


const char* string_from_int_static( int64_t val, unsigned int width, char fill )
{
	return string_from_int_buffer( get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val, width, fill );
}


char* string_from_uint( uint64_t val, bool hex, unsigned int width, char fill )
{
	char buf[32];
	return string_clone( string_from_uint_buffer( buf, 32, val, hex, width, fill ) );
}


char* string_from_uint_buffer( char* buffer, size_t size, uint64_t val, bool hex, unsigned int width, char fill )
{
	int len = snprintf( buffer, size, hex ? "%" PRIx64 : "%" PRIu64, val );
	if( len < 0 )
	{
		if( size )
			buffer[0] = 0;
		return buffer;
	}
	if( (size_t)len > size )
	{
		if( size )
			buffer[size-1] = 0;
		return buffer;
	}
	if( (unsigned int)len < width )
	{
		size_t diff = (size_t)width - (size_t)len;
		memmove( buffer + diff, buffer, (size_t)len + 1 );
		memset( buffer, fill, diff );
	}
	return buffer;
}


const char* string_from_uint_static( uint64_t val, bool hex, unsigned int width, char fill )
{
	return string_from_uint_buffer( get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val, hex, width, fill );
}


char* string_from_uint128( const uint128_t val )
{
	char buf[34];
	return string_clone( string_from_uint128_buffer( buf, 34, val ) );
}


char* string_from_uint128_buffer( char* buffer, size_t size, const uint128_t val )
{
	int len = snprintf( buffer, size, "%016" PRIx64 "%016" PRIx64, val.word[0], val.word[1] );
	if( len < 0 )
	{
		if( size )
			buffer[0] = 0;
	}
	if( (size_t)len > size )
	{
		if( size )
			buffer[size-1] = 0;
	}
	return buffer;
}


const char* string_from_uint128_static( const uint128_t val )
{
	return string_from_uint128_buffer( get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val );
}


char* string_from_real( real val, unsigned int precision, unsigned int width, char fill )
{
	char str[64];
	string_from_real_buffer( str, 64, val, precision, width, fill );
	return string_clone( str );
}


char* string_from_real_buffer( char* buffer, size_t size, real val, unsigned int precision, unsigned int width, char fill )
{
	int len;
	unsigned int ulen;
	size_t end;
#if FOUNDATION_SIZE_REAL == 64
	if( precision )
		len = snprintf( buffer, size, "%.*lf", precision, val );
	else
		len = snprintf( buffer, size, "%.16lf", val );
#else
	if( precision )
		len = snprintf( buffer, size, "%.*f", precision, val );
	else
		len = snprintf( buffer, size, "%.7f", val );
#endif
	if( len < 0 )
	{
		if( size )
			buffer[0] = 0;
		return buffer;
	}

	ulen = (unsigned int)len;
	if( ulen > size )
	{
		if( size )
			buffer[size-1] = 0;
		return buffer;
	}

	end = string_find_last_not_of( buffer, "0", STRING_NPOS );
	if( end != STRING_NPOS )
	{
		if( buffer[ end ] == '.' )
			--end;
		if( end != ( ulen - 1 ) )
		{
			++end;
			ulen = (unsigned int)end;
			buffer[ end ] = 0;
		}
	}

	if( ulen < width )
	{
		memmove( buffer + ( width - ulen ), buffer, ulen + 1 );
		memset( buffer, fill, width - ulen );
	}

	//Some cleanups
	if( string_equal( buffer, "-0" ) )
	{
		buffer[0] = '0';
		buffer[1] = 0;
	}

	return buffer;
}


const char* string_from_real_static( real val, unsigned int precision, unsigned int width, char fill )
{
	return string_from_real_buffer( get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val, precision, width, fill );
}


char* string_from_time( tick_t t )
{
	char buf[64];
	return string_clone( string_from_time_buffer( buf, 64, t ) );
}


char* string_from_time_buffer( char* buffer, size_t size, tick_t t )
{
	if( !size )
		return buffer;
#if FOUNDATION_PLATFORM_WINDOWS
	{
		time_t timet = t / 1000ULL;
		_ctime64_s( buffer, size, &timet );
		buffer[size-1] = 0;
	}
	return string_strip( buffer, STRING_WHITESPACE );
#elif FOUNDATION_PLATFORM_ANDROID
	time_t ts = t / 1000ULL;
	string_copy( buffer, ctime( &ts ), size );
	return string_strip( buffer, STRING_WHITESPACE );
#elif FOUNDATION_PLATFORM_POSIX
	if( size >= 26 )
	{
		time_t ts = (time_t)( t / 1000LL );
		ctime_r( &ts, buffer );
	}
	else
	{
		buffer[0] = 0;
	}
	return string_strip( buffer, STRING_WHITESPACE );
#else
# error Not implemented
#endif
}


const char* string_from_time_static( tick_t t )
{
	return string_from_time_buffer( get_thread_convert_buffer(), THREAD_BUFFER_SIZE, t );
}


char* string_from_uuid( const uuid_t val )
{
	char buf[38];
	return string_clone( string_from_uuid_buffer( buf, 38, val ) );
}


const char* string_from_uuid_static( const uuid_t val )
{
	return string_from_uuid_buffer( get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val );
}


char* string_from_version( const version_t version )
{
	char buf[128];
	return string_clone( string_from_version_buffer( buf, 128, version ) );
}


char* string_from_version_buffer( char* buffer, size_t size, const version_t version )
{
	int len;
	if( version.sub.control )
		len = snprintf( buffer, size, "%u.%u.%u-%u-%x", (uint32_t)version.sub.major, (uint32_t)version.sub.minor, version.sub.revision, version.sub.build, version.sub.control );
	else if( version.sub.build )
		len = snprintf( buffer, size, "%u.%u.%u-%u", (uint32_t)version.sub.major, (uint32_t)version.sub.minor, version.sub.revision, version.sub.build );
	else
		len = snprintf( buffer, size, "%u.%u.%u", (uint32_t)version.sub.major, (uint32_t)version.sub.minor, version.sub.revision );
	if( len < 0 )
	{
		if( size )
			buffer[0] = 0;
		return buffer;
	}
	if( (size_t)len > size )
	{
		if( size )
			buffer[size-1] = 0;
		return buffer;
	}
	return buffer;
}


const char* string_from_version_static( const version_t version )
{
	return string_from_version_buffer( get_thread_convert_buffer(), THREAD_BUFFER_SIZE, version );
}



int string_to_int( const char* val )
{
	int ret = 0;
	if( val )
		sscanf( val, "%d", &ret );
	return ret;
}


unsigned int string_to_uint( const char* val, bool hex )
{
	unsigned int ret = 0;
	if( val )
		sscanf( val, hex ? "%x" : "%u", &ret );
	return ret;
}


int64_t string_to_int64( const char* val )
{
	int64_t ret = 0;
	if( val )
		sscanf( val, "%" PRId64, &ret );
	return ret;
}


uint64_t string_to_uint64( const char* val, bool hex )
{
	uint64_t ret = 0;
	if( val )
		sscanf( val, hex ? "%" PRIx64 : "%" PRIu64, &ret );
	return ret;
}


uint128_t string_to_uint128( const char* val )
{
	uint128_t ret = uint128_null();
	if( val )
		sscanf( val, "%016" PRIx64 "%016" PRIx64, &ret.word[0], &ret.word[1] );
	return ret;
}


float32_t string_to_float32( const char* val )
{
	float32_t ret = 0.0f;
	if( val )
		sscanf( val, "%f", &ret );
	return ret;
}


float64_t string_to_float64( const char* val )
{
	float64_t ret = 0.0;
	if( val )
		sscanf( val, "%lf", &ret );
	return ret;
}


real string_to_real( const char* val )
{
#if ( FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_APPLE ) && ( FOUNDATION_SIZE_REAL == 64 )
	long double ret = 0.0f;
#else
	real ret = 0.0f;
#endif
	if( val )
		sscanf( val, "%" PRIREAL, &ret );
	return ret;
}


version_t string_to_version( const char* val )
{
	//%u.%u.%u-%u.%u
	uint32_t num[5];
	size_t i;
	for( i = 0; i < 5; ++i )
	{
		num[i] = 0;
		if( val && *val )
		{
			sscanf( val, i < 4 ? "%u" : "%x", num + i );
			while( *val && ( ( *val >= '0' ) && ( *val < '9' ) ) ) val++;
			while( *val && ( ( *val  < '0' ) || ( *val > '9' ) ) ) val++;
		}
	}
	return version_make( num[0], num[1], num[2], num[3], num[4] );
}

