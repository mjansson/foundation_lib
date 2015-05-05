/* string.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/array.h>


FOUNDATION_API char*          string_allocate( size_t length );
FOUNDATION_API void           string_deallocate( char* str );
FOUNDATION_API char*          string_clone( const char* str );

FOUNDATION_API char*          string_format( const char* format, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 1, 2 );
FOUNDATION_API char*          string_format_buffer( char* buffer, size_t maxlen, const char* format, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 4 );
FOUNDATION_API char*          string_vformat( const char* format, va_list list ) FOUNDATION_ATTRIBUTE4( format, printf, 1, 0 );
FOUNDATION_API char*          string_vformat_buffer( char* buffer, size_t maxlen, const char* format, va_list list ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 0 );

FOUNDATION_API size_t         string_length( const char* str );
FOUNDATION_API size_t         string_glyphs( const char* str );
FOUNDATION_API hash_t         string_hash( const char* str );

FOUNDATION_API char*          string_resize( char* str, size_t length, char fill );
FOUNDATION_API void           string_copy( char* dst, const char* src, size_t limit );
FOUNDATION_API char*          string_strip( char* str, const char* delimiters );
FOUNDATION_API char*          string_strip_substr( char* str, const char* delimiters, size_t length );
FOUNDATION_API char*          string_replace( char* str, const char* key, const char* newkey, bool repeat );
FOUNDATION_API char*          string_append( char* str, const char* suffix );
FOUNDATION_API char*          string_prepend( char* str, const char* prefix );
FOUNDATION_API char*          string_concat( const char* lhs, const char* rhs );
FOUNDATION_API void           string_split( const char* str, const char* separators, char** left, char** right, bool allowempty );
FOUNDATION_API char*          string_substr( const char* str, size_t offset, size_t length );

FOUNDATION_API size_t         string_find( const char* str, char c, size_t offset );
FOUNDATION_API size_t         string_find_string( const char* str, const char* key, size_t offset );
FOUNDATION_API size_t         string_rfind( const char* str, char c, size_t offset );
FOUNDATION_API size_t         string_rfind_string( const char* str, const char* key, size_t offset );
FOUNDATION_API size_t         string_find_first_of( const char* str, const char* key, size_t offset );
FOUNDATION_API size_t         string_find_last_of( const char* str, const char* key, size_t offset );
FOUNDATION_API size_t         string_find_first_not_of( const char* str, const char* key, size_t offset );
FOUNDATION_API size_t         string_find_last_not_of( const char* str, const char* key, size_t offset );
FOUNDATION_API size_t         string_rfind_first_of( const char* str, const char* key, size_t offset );
FOUNDATION_API size_t         string_rfind_first_not_of( const char* str, const char* key, size_t offset );

FOUNDATION_API bool           string_ends_with( const char* str, const char* suffix );
FOUNDATION_API bool           string_equal( const char* lhs, const char* rhs );
FOUNDATION_API bool           string_equal_substr( const char* lhs, const char* rhs, size_t length );
FOUNDATION_API bool           string_match_pattern( const char* element, const char* pattern );

FOUNDATION_API char**         string_explode( const char* str, const char* delimiters, bool allow_empty );
FOUNDATION_API char*          string_merge( const char* const* array, size_t array_size, const char* delimiter );

FOUNDATION_API uint32_t       string_glyph( const char* str, size_t offset, size_t* consumed );

FOUNDATION_API ssize_t        string_array_find( const char* const* haystack, const char* needle, size_t haystack_size );
FOUNDATION_API void           string_array_deallocate_elements( char** array );
#define                       string_array_deallocate( array ) ( string_array_deallocate_elements(array), array_deallocate(array), array=0 )

FOUNDATION_API wchar_t*       wstring_allocate_from_string( const char* cstr, size_t length );
FOUNDATION_API void           wstring_from_string( wchar_t* str, const char* cstr, size_t max_length, size_t length );
FOUNDATION_API void           wstring_deallocate( wchar_t* str );

FOUNDATION_API size_t         wstring_length( const wchar_t* str );
FOUNDATION_API bool           wstring_equal( const wchar_t* lhs, const wchar_t* rhs );

FOUNDATION_API char*          string_allocate_from_wstring( const wchar_t* str, size_t length );
FOUNDATION_API char*          string_allocate_from_utf16( const uint16_t* str, size_t length );
FOUNDATION_API char*          string_allocate_from_utf32( const uint32_t* str, size_t length );
FOUNDATION_API void           string_convert_utf16( char* dst, const uint16_t* src, size_t dstsize, size_t srclength );
FOUNDATION_API void           string_convert_utf32( char* dst, const uint32_t* src, size_t dstsize, size_t srclength );

FOUNDATION_API char*          string_from_int( int64_t val, unsigned int width, char padding );
FOUNDATION_API char*          string_from_uint( uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API char*          string_from_uint128( const uint128_t val );
FOUNDATION_API char*          string_from_real( real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API char*          string_from_time( tick_t time );
FOUNDATION_API char*          string_from_uuid( const uuid_t uuid );
FOUNDATION_API char*          string_from_version( const version_t version );

FOUNDATION_API char*          string_from_int_buffer( char* buffer, size_t size, int64_t val, unsigned int width, char padding );
FOUNDATION_API char*          string_from_uint_buffer( char* buffer, size_t size, uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API char*          string_from_uint128_buffer( char* buffer, size_t size, const uint128_t val );
FOUNDATION_API char*          string_from_real_buffer( char* buffer, size_t size, real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API char*          string_from_time_buffer( char* buffer, size_t size, tick_t time );
FOUNDATION_API char*          string_from_uuid_buffer( char* buffer, size_t size, const uuid_t uuid );
FOUNDATION_API char*          string_from_version_buffer( char* buffer, size_t size, const version_t version );

FOUNDATION_API const char*    string_from_int_static( int64_t val, unsigned int width, char padding );
FOUNDATION_API const char*    string_from_uint_static( uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API const char*    string_from_uint128_static( const uint128_t val );
FOUNDATION_API const char*    string_from_real_static( real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API const char*    string_from_time_static( tick_t time );
FOUNDATION_API const char*    string_from_uuid_static( const uuid_t uuid );
FOUNDATION_API const char*    string_from_version_static( const version_t version );

FOUNDATION_API int            string_to_int( const char* val );
FOUNDATION_API unsigned int   string_to_uint( const char* val, bool hex );
FOUNDATION_API int64_t        string_to_int64( const char* val );
FOUNDATION_API uint64_t       string_to_uint64( const char* val, bool hex );
FOUNDATION_API uint128_t      string_to_uint128( const char* val );
FOUNDATION_API float32_t      string_to_float32( const char* val );
FOUNDATION_API float64_t      string_to_float64( const char* val );
FOUNDATION_API real           string_to_real( const char* val );
FOUNDATION_API uuid_t         string_to_uuid( const char* val );
FOUNDATION_API version_t      string_to_version( const char* val );

#define STRING_NPOS           ((size_t)-1)
#define STRING_WHITESPACE     " \n\r\t\v\f"
#define WSTRING_WHITESPACE   L" \n\r\t\v\f"
