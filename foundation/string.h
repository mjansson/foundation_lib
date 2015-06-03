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


FOUNDATION_API string_t        string_allocate( size_t length, size_t capacity );
FOUNDATION_API string_t        string_clone( const char* str, size_t length );
FOUNDATION_API string_t        string_clone_string( string_const_t str );
FOUNDATION_API void            string_deallocate( char* str );

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t string_null( void );
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t string_empty( void );
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t string_const( const char* str, size_t length );
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t string_to_const( string_t str );

FOUNDATION_API string_t        string_allocate_format( const char* format, size_t format_length, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 1, 3 );
FOUNDATION_API string_t        string_format( char* buffer, size_t size, const char* format, size_t format_length, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 5 );
FOUNDATION_API string_t        string_allocate_vformat( const char* format, size_t format_length, va_list list ) FOUNDATION_ATTRIBUTE4( format, printf, 1, 0 );
FOUNDATION_API string_t        string_vformat( char* buffer, size_t size, const char* format, size_t format_length, va_list list ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 0 );

FOUNDATION_API size_t          string_length( const char* str );
FOUNDATION_API size_t          string_glyphs( const char* str, size_t length );
FOUNDATION_API hash_t          string_hash( const char* str, size_t length );

FOUNDATION_API string_t        string_copy( char* dst, size_t size, const char* src, size_t src_length );
FOUNDATION_API string_t        string_resize( char* str, size_t length, size_t capacity, size_t new_length, char fill );
FOUNDATION_API string_t        string_replace( char* str, size_t length, size_t capacity, const char* key, size_t key_length, const char* newkey, size_t newkey_length, bool repeat );

FOUNDATION_API string_t        string_allocate_concat( const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length );
FOUNDATION_API string_t        string_allocate_concat_varg( const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length, ... ) FOUNDATION_ATTRIBUTE( sentinel );
FOUNDATION_API string_t        string_allocate_concat_vlist( va_list list );
FOUNDATION_API string_t        string_concat( char* str, size_t capacity, const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length );
FOUNDATION_API string_t        string_concat_varg( char* str, size_t capacity, const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length, ... ) FOUNDATION_ATTRIBUTE( sentinel );
FOUNDATION_API string_t        string_concat_vlist( char* str, size_t capacity, va_list list );
FOUNDATION_API string_t        string_append( char* str, size_t length, size_t capacity, const char* suffix, size_t suffix_length );
FOUNDATION_API string_t        string_append_varg( char* str, size_t length, size_t capacity, const char* suffix, size_t suffix_length, ... ) FOUNDATION_ATTRIBUTE( sentinel );
FOUNDATION_API string_t        string_append_vlist( char* str, size_t length, size_t capacity, va_list list );
FOUNDATION_API string_t        string_prepend( char* str, size_t length, size_t capacity, const char* prefix, size_t prefix_length );
FOUNDATION_API string_t        string_prepend_varg( char* str, size_t length, size_t capacity, const char* prefix, size_t prefix_length, ... ) FOUNDATION_ATTRIBUTE( sentinel );
FOUNDATION_API string_t        string_prepend_vlist( char* str, size_t length, size_t capacity, va_list list );

FOUNDATION_API string_const_t  string_substr( const char* str, size_t length, size_t offset, size_t sub_length );
FOUNDATION_API string_const_t  string_strip( const char* str, size_t length, const char* delimiters, size_t delim_length );

FOUNDATION_API size_t          string_find( const char* str, size_t length, char c, size_t offset );
FOUNDATION_API size_t          string_find_string( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_rfind( const char* str, size_t length, char c, size_t offset );
FOUNDATION_API size_t          string_rfind_string( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_find_first_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_find_last_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_find_first_not_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );
FOUNDATION_API size_t          string_find_last_not_of( const char* str, size_t length, const char* key, size_t key_length, size_t offset );

FOUNDATION_API bool            string_ends_with( const char* str, size_t length, const char* suffix, size_t suffix_length );
FOUNDATION_API bool            string_equal( const char* lhs, size_t lhs_length, const char* rhs, size_t rhs_length );
FOUNDATION_API bool            string_equal_substr( const char* lhs, size_t lhs_length, size_t lhs_offset, const char* rhs, size_t rhs_length, size_t rhs_offset );
FOUNDATION_API bool            string_match_pattern( const char* str, size_t length, const char* pattern, size_t pattern_length );

FOUNDATION_API void            string_split( const char* str, size_t length, const char* separators, size_t sep_length, string_const_t* left, string_const_t* right, bool allowempty );
FOUNDATION_API size_t          string_explode( const char* str, size_t length, const char* delimiters, size_t delim_length, string_const_t* arr, size_t arrsize, bool allow_empty );
FOUNDATION_API string_t        string_merge( char* dst, size_t capacity, const string_const_t* array, size_t array_size, const char* delimiter, size_t delim_length );
FOUNDATION_API string_t        string_merge_varg( char* dst, size_t capacity, const char* delimiter, size_t delim_length, const char* str, size_t length, ... ) FOUNDATION_ATTRIBUTE( sentinel );
FOUNDATION_API string_t        string_merge_vlist( char* dst, size_t capacity, const char* delimiter, size_t delim_length, va_list list );

FOUNDATION_API uint32_t        string_glyph( const char* str, size_t length, size_t offset, size_t* consumed );

FOUNDATION_API ssize_t         string_array_find( const string_const_t* haystack, size_t haystack_size, const char* needle, size_t needle_length );
FOUNDATION_API void            string_array_deallocate_elements( string_t* array );
#define                        string_array_deallocate( array ) ( string_array_deallocate_elements( array ), array_deallocate( array ), (array) = 0 )

FOUNDATION_API wchar_t*        wstring_allocate_from_string( const char* cstr, size_t cstr_length );
FOUNDATION_API void            wstring_from_string( wchar_t* str, size_t length, const char* cstr, size_t cstr_length );
FOUNDATION_API void            wstring_deallocate( wchar_t* str );

FOUNDATION_API size_t          wstring_length( const wchar_t* str );
FOUNDATION_API bool            wstring_equal( const wchar_t* lhs, const wchar_t* rhs );

FOUNDATION_API string_t        string_allocate_from_wstring( const wchar_t* str, size_t length );
FOUNDATION_API string_t        string_allocate_from_utf16( const uint16_t* str, size_t length );
FOUNDATION_API string_t        string_allocate_from_utf32( const uint32_t* str, size_t length );
FOUNDATION_API void            string_convert_utf16( char* dst, size_t dst_length, const uint16_t* src, size_t src_length );
FOUNDATION_API void            string_convert_utf32( char* dst, size_t dst_length, const uint32_t* src, size_t src_length );

FOUNDATION_API string_t        string_from_int( char* str, size_t length, int64_t val, unsigned int width, char padding );
FOUNDATION_API string_t        string_from_uint( char* str, size_t length, uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API string_t        string_from_uint128( char* str, size_t length, const uint128_t val );
FOUNDATION_API string_t        string_from_real( char* str, size_t length, real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API string_t        string_from_time( char* str, size_t length, tick_t time );
FOUNDATION_API string_t        string_from_uuid( char* str, size_t length, const uuid_t uuid );
FOUNDATION_API string_t        string_from_version( char* str, size_t length, const version_t version );

FOUNDATION_API string_const_t  string_from_int_static( int64_t val, unsigned int width, char padding );
FOUNDATION_API string_const_t  string_from_uint_static( uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API string_const_t  string_from_uint128_static( const uint128_t val );
FOUNDATION_API string_const_t  string_from_real_static( real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API string_const_t  string_from_time_static( tick_t time );
FOUNDATION_API string_const_t  string_from_uuid_static( const uuid_t uuid );
FOUNDATION_API string_const_t  string_from_version_static( const version_t version );

FOUNDATION_API int             string_to_int( const char* str, size_t length );
FOUNDATION_API unsigned int    string_to_uint( const char* str, size_t length, bool hex );
FOUNDATION_API int64_t         string_to_int64( const char* str, size_t length );
FOUNDATION_API uint64_t        string_to_uint64( const char* str, size_t length, bool hex );
FOUNDATION_API uint128_t       string_to_uint128( const char* str, size_t length );
FOUNDATION_API float32_t       string_to_float32( const char* str, size_t length );
FOUNDATION_API float64_t       string_to_float64( const char* str, size_t length );
FOUNDATION_API real            string_to_real( const char* str, size_t length );
FOUNDATION_API uuid_t          string_to_uuid( const char* str, size_t length );
FOUNDATION_API version_t       string_to_version( const char* str, size_t length );

FOUNDATION_API string_t        string_thread_buffer( void );

#define STRING_NPOS            ((size_t)-1)
#define STRING_NEWLINE         "\n"
#define STRING_EMPTY           ""
#define STRING_WHITESPACE      " \n\r\t\v\f"
#define WSTRING_WHITESPACE    L" \n\r\t\v\f"


static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t string_null( void )
{
    return (string_const_t){ 0, 0 };
}


static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t string_empty( void )
{
    return (string_const_t){ STRING_EMPTY, 0 };
}


static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t string_const( const char* str, size_t length )
{
    return (string_const_t){ str, length };
}


static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t string_to_const( string_t str )
{
    return (string_const_t){ str.str, str.length };
}

