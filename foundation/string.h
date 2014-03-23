/* utf.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file string.h
    String handling and utility functions in UTF-8, conversion to/from UTF-16. Conversion to and from pritimive integral
	data types.

    Unless otherwise noted in the function descripion it is safe to pass null pointers to any arguments. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate (length+1) bytes of memory for a string of length characters. The string will be empty, i.e first byte
    will be initialized to 0. The remainder of the memory block will be uninitialized
	\param length             Length of string
	\return                   Empty string stored in memory block of length (length+1) */
FOUNDATION_API char*          string_allocate( unsigned int length );

/*! Deallocate memory used by the given string
    \param str                String */
FOUNDATION_API void           string_deallocate( char* str );

/*! Clone a string
    \param str                String to clone
	\return                   Cloned string */
FOUNDATION_API char*          string_clone( const char* str );

/*! Create a string from a format specifier and variable data, printf style.
    \param format             Format specifier
	\return                   Formatted string */
FOUNDATION_API char*          string_format( const char* format, ... );

/*! In-memory string formatting from a format specifier and variable data, printf style. Will print at most
    maxlen-1 characters into the buffer and always zero terminate.
	\param buffer             Destination buffer
	\param maxlen             Maximum number of characters to print into buffer, including zero termination
    \param format             Format specifier
	\return                   Formatted string (buffer) */
FOUNDATION_API char*          string_format_buffer( char* buffer, unsigned int maxlen, const char* format, ... );

/*! Create a string from a format specifier and variable data given as a va_list, printf style.
    \param format             Format specifier
	\param list               Variable argument list
	\return                   Formatted string */
FOUNDATION_API char*          string_vformat( const char* format, va_list list );

/*! In-memory string formatting from a format specifier and variable data given as a va_list, printf style. Will print at most
    maxlen-1 characters into the buffer and always zero terminate.
	\param buffer             Destination buffer
	\param maxlen             Maximum number of characters to print into buffer, including zero termination
    \param format             Format specifier
	\param list               Variable argument list
	\return                   Formatted string (buffer) */
FOUNDATION_API char*          string_vformat_buffer( char* buffer, unsigned int maxlen, const char* format, va_list list );

/*! Get length of string in bytes
    \param str                String
    \return                   Length of string in bytes */
FOUNDATION_API unsigned int   string_length( const char* str );

/*! Get number of unicode glyphs stored in utf-8 string
    \param str                String in utf-8 encoding
    \return                   Number of unicode glyphs in string */
FOUNDATION_API unsigned int   string_glyphs( const char* str );

/*! Calculate hash of string. A null pointer is treated as an empty string
    \param str                String
	\return                   Hash of string */
FOUNDATION_API hash_t         string_hash( const char* str );

/*! Reallocate a string and fill with the given character. If the current string length
    is greater or equal to the requested length the string is zero terminated at the
    requested length but not reallocated.
    \param str                String
    \param length             New requested length
    \param c                  Fill character
    \return                   New string of requested length */
FOUNDATION_API char*          string_resize( char* str, unsigned int length, char c );

/*! Copy one string to another. Like strncpy, except that dst will always be zero
    terminated (i.e copies at most limit-1 characters from src)
    \param dst                Destination string
    \param src                Source string
    \param limit              Maximum number of bytes to store in dst string, including zero terminator */
FOUNDATION_API void           string_copy( char* dst, const char* src, unsigned int limit );

/*! Strip a string (in-place) of given characters at start and end. 
    Modifies the passed string and returns a new pointer to it. If null is passed as string
    to strip, a new empty string is returned.
    \param str                String to strip
    \param delimiters         Characters to strip
    \return                   Stipped string */
FOUNDATION_API char*          string_strip( char* str, const char* delimiters );

/*! Strip a substring (in-place) of given characters at start end end of the given sub-string. 
    Modifies the passed string (starting at str and ending at str+length)
    and returns a new pointer to it. If null is passed as string to strip, a new empty string is returned.
    \param str                String to strip
    \param delimiters         Characters to strip
    \param length             Length of substring to strip
    \return                   Stipped string */
FOUNDATION_API char*          string_strip_substr( char* str, const char* delimiters, unsigned int length );

/*! Replace all occurrences of the given key string inside the given string, optionally repeating the
    replace after an occurrence have been replaced. If repeat is set to false, the newly replaced part of
    the string is not rechecked for replacement. The repeat is recursion safe and will not endlessly loop,
    like repat replacing "foo" with "foobar". The string will be reallocated if the replacement string
    is longer than the key string, so always replace and use of the old string pointer with value returned.
    \param str                String to replace in
    \param key                Key string
    \param newkey             String to replace key with
    \param repeat             Repeat flag
    \return                   New (potentially reallocated) string pointer */
FOUNDATION_API char*          string_replace( char* str, const char* key, const char* newkey, bool repeat );

/*! Append a suffix to a string. Will reallocate the given string so
    use the returned pointer as a replacement for the passed string.
    \param str                Base string
    \param suffix             Suffix string
    \return                   New string which is base string with suffix string appended (base+suffix) */
FOUNDATION_API char*          string_append( char* str, const char* suffix );

/*! Prepend a prefix to a string. Will reallocate the given string so
    use the returned pointer as a replacement for the passed string.
    \param str                Base string
    \param prefix             Prefix string
    \return                   New string which is base string with prefix string prepended (prefix+base) */
FOUNDATION_API char*          string_prepend( char* str, const char* prefix );

/*! Concatenate two strings. Does not modify arguments, will allocate
    a new string to hold the result.
    \param lhs                First string
    \param rhs                Second string
    \return                   New concatenated string (lhs+rhs) */
FOUNDATION_API char*          string_concat( const char* lhs, const char* rhs );

/*! Split a string into two strings along the first occurrence of any of the given separator characters.
    The entire group of separators following the initial occurrence will be removed in the split. For example,
    splitting "this is insidious" with separators "is " will yield resulting strings "th" and "nsidious".
    Unless null the left and right string pointers will be set to newly allocated strings. The allowempty flag
    controls if an empty left part is allowed. If not, any leading separator characters will first be
    discarded before processing split.
    \param str                Source string
    \param separators         Separator characters
    \param left               Pointer to string pointer which will receive new left part of split result.
    \param right              Pointer to string pointer which will receive new right part of split result
    \param allowempty         Empty flag */
FOUNDATION_API void           string_split( const char* str, const char* separators, char** left, char** right, bool allowempty );

/*! Get substring of a string. Will be clamped to source string limits
    \param str                Source string
    \param offset             Start offset
    \param length             Length of substring
    \return                   New substring */
FOUNDATION_API char*          string_substr( const char* str, unsigned int offset, unsigned int length );

FOUNDATION_API unsigned int   string_find( const char* str, char c, unsigned int offset );
FOUNDATION_API unsigned int   string_find_string( const char* str, const char* key, unsigned int offset );
FOUNDATION_API unsigned int   string_rfind( const char* str, char c, unsigned int offset );
FOUNDATION_API unsigned int   string_rfind_string( const char* str, const char* key, unsigned int offset );
FOUNDATION_API unsigned int   string_find_first_of( const char* str, const char* tokens, unsigned int offset );
FOUNDATION_API unsigned int   string_find_last_of( const char* str, const char* tokens, unsigned int offset );
FOUNDATION_API unsigned int   string_find_first_not_of( const char* str, const char* tokens, unsigned int offset );
FOUNDATION_API unsigned int   string_find_last_not_of( const char* str, const char* tokens, unsigned int offset );
FOUNDATION_API unsigned int   string_rfind_first_of( const char* str, const char* tokens, unsigned int offset );
FOUNDATION_API unsigned int   string_rfind_first_not_of( const char* str, const char* tokens, unsigned int offset );

FOUNDATION_API bool           string_ends_with( const char* str, const char* suffix );

FOUNDATION_API bool           string_equal( const char* lhs, const char* rhs );
FOUNDATION_API bool           string_equal_substr( const char* lhs, const char* rhs, unsigned int length );

FOUNDATION_API bool           string_match_pattern( const char* element, const char* pattern );

FOUNDATION_API char**         string_explode( const char* str, const char* delimiters, bool allow_empty );
FOUNDATION_API char*          string_merge( const char* const* array, unsigned int array_size, const char* delimiter );

FOUNDATION_API uint32_t       string_glyph( const char* str, unsigned int offset, unsigned int* consumed );

FOUNDATION_API int            string_array_find( const char* const* haystack, const char* needle, unsigned int haystack_size );

FOUNDATION_API void           string_array_deallocate_elements( char** array );
#define                       string_array_deallocate( array ) ( string_array_deallocate_elements(array), array_deallocate(array), array=0 )

FOUNDATION_API wchar_t*       wstring_allocate_from_string( const char* cstr, unsigned int length );
FOUNDATION_API void           wstring_from_string( wchar_t* str, const char* cstr, unsigned int max_length );
FOUNDATION_API void           wstring_deallocate( wchar_t* str );
FOUNDATION_API unsigned int   wstring_length( const wchar_t* str );
FOUNDATION_API bool           wstring_equal( const wchar_t* lhs, const wchar_t* rhs );

FOUNDATION_API char*          string_allocate_from_wstring( const wchar_t* str, unsigned int length );
FOUNDATION_API char*          string_allocate_from_utf16( const uint16_t* str, unsigned int length );
FOUNDATION_API char*          string_allocate_from_utf32( const uint32_t* str, unsigned int length );

FOUNDATION_API void           string_convert_utf16( char* dst, const uint16_t* src, unsigned int dstsize, unsigned int srclength );
FOUNDATION_API void           string_convert_utf32( char* dst, const uint32_t* src, unsigned int dstsize, unsigned int srclength );


/*! Convert an integer to a string, with optional hexadecimal base and base prefix, field width and fill character
    \param val                Integer value
    \param width              Field width
    \param padding            Fill character */
FOUNDATION_API char*          string_from_int( int64_t val, unsigned int width, char padding );

/*! Convert an unsigned integer to a string, with optional hexadecimal base and base prefix, field width and fill character
    \param val                Integer value
    \param hex                Hexadecimal flag
    \param width              Field width
    \param padding            Fill character */
FOUNDATION_API char*          string_from_uint( uint64_t val, bool hex, unsigned int width, char padding );

/*! Convert an 128-bit unsigned integer to a string
    \param val                Integer value */
FOUNDATION_API char*          string_from_uint128( const uint128_t val );

/*! Convert a float to a string, with optional fixed notation, field width, precision and fill character
    \param val                Float value
    \param precision          Precision
    \param width              Field width
    \param padding            Fill character */
FOUNDATION_API char*          string_from_real( real val, unsigned int precision, unsigned int width, char padding );

/*! Get a formatted string of the given timestamp
    \param time               Timestamp
    \return                   String */
FOUNDATION_API char*          string_from_time( uint64_t time );

/*! Get a formatted string of the given UUID
    \param uuid               UUID
    \return                   String */
FOUNDATION_API char*          string_from_uuid( const uuid_t uuid );

/*! Get a formatted string of the given version
    \param version            Version
    \return                   String */
FOUNDATION_API char*          string_from_version( const version_t version );

FOUNDATION_API char*          string_from_int_buffer( char* buffer, int64_t val, unsigned int width, char padding );
FOUNDATION_API char*          string_from_uint_buffer( char* buffer, uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API char*          string_from_uint128_buffer( char* buffer, const uint128_t val );
FOUNDATION_API char*          string_from_real_buffer( char* buffer, real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API char*          string_from_time_buffer( char* buffer, uint64_t time );
FOUNDATION_API char*          string_from_uuid_buffer( char* buffer, const uuid_t uuid );
FOUNDATION_API char*          string_from_version_buffer( char* buffer, const version_t version );

FOUNDATION_API const char*    string_from_int_static( int64_t val, unsigned int width, char padding );
FOUNDATION_API const char*    string_from_uint_static( uint64_t val, bool hex, unsigned int width, char padding );
FOUNDATION_API const char*    string_from_uint128_static( const uint128_t val );
FOUNDATION_API const char*    string_from_real_static( real val, unsigned int precision, unsigned int width, char padding );
FOUNDATION_API const char*    string_from_time_static( uint64_t time );
FOUNDATION_API const char*    string_from_uuid_static( const uuid_t uuid );
FOUNDATION_API const char*    string_from_version_static( const version_t version );


/*! Convert a string to a signed integer
    \param val                String */
FOUNDATION_API int            string_to_int( const char* val );

/*! Convert a string to an unsigned integer
    \param val                String
    \param hex                Hexadecimal flag */
FOUNDATION_API unsigned int   string_to_uint( const char* val, bool hex );

/*! Convert a string to a signed 64-bit integer
    \param val                String */
FOUNDATION_API int64_t        string_to_int64( const char* val );

/*! Convert a string to an unsigned 64-bit integer
    \param val                String
    \param hex                Hexadecimal flag */
FOUNDATION_API uint64_t       string_to_uint64( const char* val, bool hex );

/*! Convert a string to an unsigned 128-bit integer
    \param val                String */
FOUNDATION_API uint128_t      string_to_uint128( const char* val );

/*! Convert a string to a float
    \param val                String */
FOUNDATION_API real           string_to_real( const char* val );

/*! Convert a string to a uuid
    \param val                String */
FOUNDATION_API uuid_t         string_to_uuid( const char* str );

/*! Convert a string to a version identifier
    \param val                String */
FOUNDATION_API version_t      string_to_version( const char* str );

/*! Validate email address format (basic tests)
    \param address            Email address string */
FOUNDATION_API bool           string_is_valid_email_address( const char* address );


#define STRING_NPOS                     0xFFFFFFFFU

#define STRING_WHITESPACE               " \n\r\t"
#define WSTRING_WHITESPACE             L" \n\r\t"

