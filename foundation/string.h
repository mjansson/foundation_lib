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


FOUNDATION_API string_t       string_allocate( size_t length );
FOUNDATION_API void           string_deallocate( string_t str );
FOUNDATION_API string_t       string_clone( string_const_t str );

FOUNDATION_API string_const_t string_null( void );
FOUNDATION_API string_const_t string_const( const char* str, size_t length );
FOUNDATION_API string_const_t string_to_const( string_t str );

FOUNDATION_API string_t       string_format( const char* format, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 1, 2 );
FOUNDATION_API string_t       string_format_string( string_t str, const char* format, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 2, 3 );
FOUNDATION_API string_t       string_vformat( const char* format, va_list list ) FOUNDATION_ATTRIBUTE4( format, printf, 1, 0 );
FOUNDATION_API string_t       string_vformat_string( string_t str, const char* format, va_list list ) FOUNDATION_ATTRIBUTE4( format, printf, 2, 0 );

FOUNDATION_API size_t         string_length( const char* str );
FOUNDATION_API size_t         string_glyphs( string_const_t str );
FOUNDATION_API hash_t         string_hash( string_const_t str );

FOUNDATION_API string_t       string_resize( string_t str, size_t length, char fill );
FOUNDATION_API string_t       string_copy( string_t dst, string_const_t src );
FOUNDATION_API string_t       string_replace( string_t str, string_const_t key, string_const_t newkey, bool repeat );
FOUNDATION_API string_t       string_append( string_t str, string_const_t suffix );
FOUNDATION_API string_t       string_prepend( string_t str, string_const_t prefix );
FOUNDATION_API string_t       string_concat( string_const_t lhs, string_const_t rhs );
FOUNDATION_API void           string_split( string_const_t str, string_const_t separators, string_const_t* left, string_const_t* right, bool allowempty );
FOUNDATION_API string_t       string_substr( string_t str, size_t offset, size_t length );
FOUNDATION_API string_const_t string_substr_const( string_const_t str, size_t offset, size_t length );
FOUNDATION_API string_t       string_strip( string_t str, string_const_t delimiters );
FOUNDATION_API string_const_t string_strip_const( string_const_t str, string_const_t delimiters );

FOUNDATION_API size_t         string_find( string_const_t str, char c, size_t offset );
FOUNDATION_API size_t         string_find_string( string_const_t str, string_const_t key, size_t offset );
FOUNDATION_API size_t         string_rfind( string_const_t str, char c, size_t offset );
FOUNDATION_API size_t         string_rfind_string( string_const_t str, string_const_t key, size_t offset );
FOUNDATION_API size_t         string_find_first_of( string_const_t str, string_const_t key, size_t offset );
FOUNDATION_API size_t         string_find_last_of( string_const_t str, string_const_t key, size_t offset );
FOUNDATION_API size_t         string_find_first_not_of( string_const_t str, string_const_t key, size_t offset );
FOUNDATION_API size_t         string_find_last_not_of( string_const_t str, string_const_t key, size_t offset );
FOUNDATION_API size_t         string_rfind_first_of( string_const_t str, string_const_t key, size_t offset );
FOUNDATION_API size_t         string_rfind_first_not_of( string_const_t str, string_const_t key, size_t offset );

FOUNDATION_API bool           string_ends_with( string_const_t str, string_const_t suffix );
FOUNDATION_API bool           string_equal( string_const_t lhs, string_const_t rhs );
FOUNDATION_API bool           string_equal_substr( string_const_t lhs, string_const_t rhs, size_t length );
FOUNDATION_API bool           string_match_pattern( string_const_t element, string_const_t pattern );

FOUNDATION_API string_t*      string_explode( string_const_t str, string_const_t delimiters, bool allow_empty );
FOUNDATION_API string_t       string_merge( string_const_t* array, size_t array_size, string_const_t delimiter );

FOUNDATION_API uint32_t       string_glyph( string_const_t str, size_t offset, size_t* consumed );

FOUNDATION_API ssize_t        string_array_find( string_const_t* haystack, size_t haystack_size, string_const_t needle );
FOUNDATION_API void           string_array_deallocate_elements( string_t* array );
#define                       string_array_deallocate( array ) ( string_array_deallocate_elements( array ), array_deallocate( array ), (array) = 0 )

FOUNDATION_API wchar_t*       wstring_allocate_from_string( string_const_t cstr, size_t length );
FOUNDATION_API void           wstring_from_string( wchar_t* str, size_t max_length, string_const_t cstr );
FOUNDATION_API void           wstring_deallocate( wchar_t* str );

FOUNDATION_API size_t         wstring_length( const wchar_t* str );
FOUNDATION_API bool           wstring_equal( const wchar_t* lhs, const wchar_t* rhs );

FOUNDATION_API string_t       string_allocate_from_wstring( const wchar_t* str, size_t length );
FOUNDATION_API string_t       string_allocate_from_utf16( const uint16_t* str, size_t length );
FOUNDATION_API string_t       string_allocate_from_utf32( const uint32_t* str, size_t length );
FOUNDATION_API void           string_convert_utf16( string_t dst, const uint16_t* src, size_t srclength );
FOUNDATION_API void           string_convert_utf32( string_t dst, const uint32_t* src, size_t srclength );

FOUNDATION_API string_t       string_from_int( int64_t val, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_uint( uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_uint128( const uint128_t val );
FOUNDATION_API string_t       string_from_real( real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_time( tick_t time );
FOUNDATION_API string_t       string_from_uuid( const uuid_t uuid );
FOUNDATION_API string_t       string_from_version( const version_t version );

FOUNDATION_API string_t       string_from_int_string( string_t str, int64_t val, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_uint_string( string_t str, uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_uint128_string( string_t str, const uint128_t val );
FOUNDATION_API string_t       string_from_real_string( string_t stre, real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_time_string( string_t str, tick_t time );
FOUNDATION_API string_t       string_from_uuid_string( string_t str, const uuid_t uuid );
FOUNDATION_API string_t       string_from_version_string( string_t stre, const version_t version );

FOUNDATION_API string_const_t string_from_int_static( int64_t val, unsigned int width, char padding );
FOUNDATION_API string_const_t string_from_uint_static( uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API string_const_t string_from_uint128_static( const uint128_t val );
FOUNDATION_API string_const_t string_from_real_static( real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API string_const_t string_from_time_static( tick_t time );
FOUNDATION_API string_const_t string_from_uuid_static( const uuid_t uuid );
FOUNDATION_API string_const_t string_from_version_static( const version_t version );

FOUNDATION_API int            string_to_int( string_const_t str );
FOUNDATION_API unsigned int   string_to_uint( string_const_t str, bool hex );
FOUNDATION_API int64_t        string_to_int64( string_const_t str );
FOUNDATION_API uint64_t       string_to_uint64( string_const_t str, bool hex );
FOUNDATION_API uint128_t      string_to_uint128( string_const_t str );
FOUNDATION_API float32_t      string_to_float32( string_const_t str );
FOUNDATION_API float64_t      string_to_float64( string_const_t str );
FOUNDATION_API real           string_to_real( string_const_t str );
FOUNDATION_API uuid_t         string_to_uuid( string_const_t str );
FOUNDATION_API version_t      string_to_version( string_const_t str );

#define STRING_NPOS           ((size_t)-1)
#define STRING_WHITESPACE     " \n\r\t\v\f"
#define WSTRING_WHITESPACE   L" \n\r\t\v\f"
