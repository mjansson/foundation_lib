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


FOUNDATION_API string_t        string_allocate( size_t length );
FOUNDATION_API void            string_deallocate( char* str );
FOUNDATION_API string_t        string_clone( const char* str, size_t length );

FOUNDATION_API string_const_t  string_null( void );
FOUNDATION_API string_const_t  string_const( const char* str, size_t length );
FOUNDATION_API string_const_t  string_to_const( string_t str );

FOUNDATION_API string_t        string_format( const char* format, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 1, 2 );
FOUNDATION_API string_t        string_format_buffer( char* str, size_t length, const char* format, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 4 );
FOUNDATION_API string_t        string_vformat( const char* format, va_list list ) FOUNDATION_ATTRIBUTE4( format, printf, 1, 0 );
FOUNDATION_API string_t        string_vformat_string( char* str, size_t length, const char* format, va_list list ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 0 );

FOUNDATION_API size_t          string_length( const char* str );
FOUNDATION_API size_t          string_glyphs( const char* str, size_t length );
FOUNDATION_API hash_t          string_hash( const char* str, size_t length );

FOUNDATION_API string_t        string_resize( char* str, size_t length, size_t new_length, char fill );
FOUNDATION_API string_t        string_copy( char* dst, size_t dst_length, const char* src, size_t src_length );
FOUNDATION_API string_t        string_replace( char* str, size_t length, const char* key, size_t key_length, const char* newkey, size_t newkey_length, bool repeat );
FOUNDATION_API string_t        string_append( char* str, size_t length, const char* suffix, size_t suffix_length );
FOUNDATION_API string_t        string_prepend( char* str, size_t length, const char* prefix, size_t prefix_length );
FOUNDATION_API string_t        string_concat( const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length );
FOUNDATION_API void            string_split( const char* str, size_t length, const char* separators, size_t sep_length, string_const_t* left, string_const_t* right, bool allowempty );
FOUNDATION_API string_t        string_substr( char* str, size_t length, size_t offset, size_t sub_length );
FOUNDATION_API string_const_t  string_substr_const( const char* str, size_t length, size_t offset, size_t sub_length );
FOUNDATION_API string_t        string_strip( char* str, size_t length, const char* delimiters, size_t delim_length );
FOUNDATION_API string_const_t  string_strip_const( const char* str, size_t length, const char* delimiters, size_t delim_length );

FOUNDATION_API size_t          string_find( const char* str, size_t length, char c, size_t offset );
FOUNDATION_API size_t          string_find_string( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_rfind( const char* str, size_t length, char c, size_t offset );
FOUNDATION_API size_t          string_rfind_string( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_find_first_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_find_last_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_find_first_not_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_find_last_not_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_rfind_first_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_rfind_first_not_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );

FOUNDATION_API bool            string_ends_with( const char* str, size_t length, const char* suffix, size_t suffix_length );
FOUNDATION_API bool            string_equal( const char* lhs, size_t lhs_length, const char* rhs, size_t rhs_length );
FOUNDATION_API bool            string_equal_substr( const char* lhs, size_t lhs_length, size_t lhs_offset, const char* rhs, size_t rhs_length, size_t rhs_offset );
FOUNDATION_API bool            string_match_pattern( const char* str, size_t length, const char* pattern, size_t pattern_length );

FOUNDATION_API string_const_t* string_explode( const char* str, size_t length, const char* delimiters, size_t delim_length, bool allow_empty );
FOUNDATION_API string_t        string_merge( string_const_t* array, size_t array_size, const char* delimiter, size_t delim_length );

FOUNDATION_API uint32_t        string_glyph( const char* str, size_t length, size_t offset, size_t* consumed );

FOUNDATION_API ssize_t         string_array_find( string_const_t* haystack, size_t haystack_size, const char* needle, size_t needle_length );
FOUNDATION_API void            string_array_deallocate_elements( string_t* array );
#define                        string_array_deallocate( array ) ( string_array_deallocate_elements( array ), array_deallocate( array ), (array) = 0 )

FOUNDATION_API wchar_t*       wstring_allocate_from_string( const char* cstr, size_t cstr_length );
FOUNDATION_API void           wstring_from_string( wchar_t* str, size_t length, const char* cstr, size_t cstr_length );
FOUNDATION_API void           wstring_deallocate( wchar_t* str );

FOUNDATION_API size_t         wstring_length( const wchar_t* str );
FOUNDATION_API bool           wstring_equal( const wchar_t* lhs, const wchar_t* rhs );

FOUNDATION_API string_t       string_allocate_from_wstring( const wchar_t* str, size_t length );
FOUNDATION_API string_t       string_allocate_from_utf16( const uint16_t* str, size_t length );
FOUNDATION_API string_t       string_allocate_from_utf32( const uint32_t* str, size_t length );
FOUNDATION_API void           string_convert_utf16( char* dst, size_t dst_length, const uint16_t* src, size_t src_length );
FOUNDATION_API void           string_convert_utf32( char* dst, size_t dst_length, const uint32_t* src, size_t src_length );

FOUNDATION_API string_t       string_from_int( int64_t val, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_uint( uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_uint128( const uint128_t val );
FOUNDATION_API string_t       string_from_real( real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_time( tick_t time );
FOUNDATION_API string_t       string_from_uuid( const uuid_t uuid );
FOUNDATION_API string_t       string_from_version( const version_t version );

FOUNDATION_API string_t       string_from_int_buffer( char* str, size_t length, int64_t val, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_uint_buffer( char* str, size_t length, uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_uint128_buffer( char* str, size_t length, const uint128_t val );
FOUNDATION_API string_t       string_from_real_buffer( char* str, size_t length, real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API string_t       string_from_time_buffer( char* str, size_t length, tick_t time );
FOUNDATION_API string_t       string_from_uuid_buffer( char* str, size_t length, const uuid_t uuid );
FOUNDATION_API string_t       string_from_version_buffer( char* str, size_t length, const version_t version );

FOUNDATION_API string_const_t string_from_int_static( int64_t val, unsigned int width, char padding );
FOUNDATION_API string_const_t string_from_uint_static( uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API string_const_t string_from_uint128_static( const uint128_t val );
FOUNDATION_API string_const_t string_from_real_static( real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API string_const_t string_from_time_static( tick_t time );
FOUNDATION_API string_const_t string_from_uuid_static( const uuid_t uuid );
FOUNDATION_API string_const_t string_from_version_static( const version_t version );

FOUNDATION_API int            string_to_int( const char* str, size_t length );
FOUNDATION_API unsigned int   string_to_uint( const char* str, size_t length, bool hex );
FOUNDATION_API int64_t        string_to_int64( const char* str, size_t length );
FOUNDATION_API uint64_t       string_to_uint64( const char* str, size_t length, bool hex );
FOUNDATION_API uint128_t      string_to_uint128( const char* str, size_t length );
FOUNDATION_API float32_t      string_to_float32( const char* str, size_t length );
FOUNDATION_API float64_t      string_to_float64( const char* str, size_t length );
FOUNDATION_API real           string_to_real( const char* str, size_t length );
FOUNDATION_API uuid_t         string_to_uuid( const char* str, size_t length );
FOUNDATION_API version_t      string_to_version( const char* str, size_t length );

#define STRING_NPOS           ((size_t)-1)
#define STRING_WHITESPACE     " \n\r\t\v\f"
#define WSTRING_WHITESPACE   L" \n\r\t\v\f"
