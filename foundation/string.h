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

/*! \file string.h
\brief String utility functions
\details String handling and utility functions in UTF-8, conversion to/from UTF-16. Conversion
to and from pritimive integral data types. Unless otherwise noted in the function descripion it
is safe to pass null pointers to any arguments.

Strings are declared with a pointer to a memory block and the length of the string. Zero
termination is generally NOT enforced. Functions that allocate or modify strings always return
zero-terminated strings. Functions that find substrings and return const string arguments does
not modify or allocate memory, and returns substrings which are NOT zero terminated. Check the
documentation for each function for further details regarding zero termination state.

All functions in the library taking string arguments as (pointer, length) tuples respect given
lengths and does not require zero termination. This design minimized calls to find string
lengths and minimizes additional memory allocations to store zero terminated substrings by
allowing substrings to be declared as a (pointer, length) tuple into the original string
memory buffer. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! \brief Allocate string
Allocate capacity bytes of memory for a string of length characters. The memory block will be
initialized to zero. The length is capped to [0,capacity-1].
\param length Length of string
\param capacity Capacity of memory block allocated
\return String with given length in zero initialized memory block with given capacity */
FOUNDATION_API string_t
string_allocate(size_t length, size_t capacity);

/*! \brief Clone string
Clone a string of the given length. Allocates a memory block of capacity (length+1) and copies
the content of the given string. Zero terminates the new string. Will allocate a new memory
block even for null/empty string arguments.
\param str String to clone
\param length Length of string to clone
\return Zero-terminated cloned string of given length stored in a new memory block
        of capacity (length+1) */
FOUNDATION_API string_t
string_clone(const char* str, size_t length);

/*! \brief Clone string
Clone a string. Allocates a memory block of capacity (length+1) and copies the content of
the given string. Zero terminates the new string. Will allocate a new memory block even
for a null/empty string argument.
\param str String to clone
\param length Length of string to clone
\return Zero-terminated cloned string of given length stored in a new memory block
        of capacity (length+1) */
FOUNDATION_API string_t
string_clone_string(string_const_t str);

/*! \brief Dellocate string
Deallocate memory used by the given string
\param str String to deallocate */
FOUNDATION_API void
string_deallocate(char* str);

/*! \brief Declare an null string
Declare an null string of zero length pointing to null.
\return String with zero length pointing to null. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_null(void);

/*! \brief Declare an empty string
Declare an empty string of zero length pointing to a zero-terminated empty string. The
string pointer must not be deallocated.
\return String with zero length pointing to a zero-terminated empty string. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_empty(void);

/*! \brief Declare a const string of given length
Declare a const string of the given length pointing to a const memory block with the
given address. String is not guaranteed to be zero terminated.
\param str String memory block
\param length length Length of string
\return String with given length pointing to a const memory block of at least given
        length in size. Not guaranteed to be zero terminated. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_const(const char* str, size_t length);

/*! \brief Cast a string to a const string
Cast a given string to a const string with the same memory pointer and length.
\param str String to cast
\return Const string of same length pointing to same memory block as given string. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_to_const(string_t str);

/*! \brief Format string
Allocate a new string from a format specifier and variable data, printf style.
\param format Format specifier
\param format_length Length of format specifier
\return Formatted string in a newly allocate memory buffer, zero terminated */
FOUNDATION_API string_t
string_allocate_format(const char* format, size_t format_length, ...)
FOUNDATION_ATTRIBUTE4(format, printf, 1, 3);

/*! \brief Format string in buffer
In-memory string formatting from a format specifier and variable data, printf style. Will print
at most (capacity-1) characters into the buffer and always zero terminate.
\param buffer Destination buffer
\param capactiy Capacity of destination buffer
\param format Format specifier
\param format_length Length of format specifier
\return Formatted string in given buffer, zero terminated */
FOUNDATION_API string_t
string_format(char* buffer, size_t capacity, const char* format, size_t format_length, ...)
FOUNDATION_ATTRIBUTE4(format, printf, 3, 5);

/*! \brief Format string
Allocate a new string from a format specifier and variable data given as a va_list, printf style.
\param format Format specifier
\param format_length Length of format specifier
\param list Variable argument list
\return Formatted string in a newly allocate memory buffer, zero terminated */
FOUNDATION_API string_t
string_allocate_vformat(const char* format, size_t format_length, va_list list)
FOUNDATION_ATTRIBUTE4(format, printf, 1, 0);

/*! \brief Format string in buffer
In-memory string formatting from a format specifier and variable data given as a va_list, printf
style. Will print at most (capacity-1) characters into the buffer and always zero terminate.
\param buffer Destination buffer
\param capactiy Capacity of destination buffer
\param format Format specifier
\param format_length Length of format specifier
\param list Variable argument list
\return Formatted string in given buffer, zero terminated */
FOUNDATION_API string_t
string_vformat(char* buffer, size_t capacity, const char* format, size_t format_length, va_list list)
FOUNDATION_ATTRIBUTE4(format, printf, 3, 0);

/*! \brief Get string length
Get length of string in bytes. String must be zero terminated.
\param str String
\return Length of string in bytes */
FOUNDATION_API size_t
string_length(const char* str);

/*! \brief Get number of glyphs
Get number of unicode glyphs stored in utf-8 string
\param str String in utf-8 encoding
\parma length Length of string
\return Number of unicode glyphs in string */
FOUNDATION_API size_t
string_glyphs(const char* str, size_t length);

/*! \brief Get string hash
Calculate hash of string. A null pointer is treated as an empty string, in which
case the length is disregarded.
\param str String
\param length Length of string
\return Hash of string */
FOUNDATION_API hash_t
string_hash(const char* str, size_t length);

/*! \brief Copy string
Copy one string to another. Like strlcpy in that dst will always be zero terminated,
i.e copies at most (capacity-1) characters from source string. Safe to pass null
pointers in both pointer arguments.
\param dst Destination buffer
\param capacity Capacity of destination buffer
\param src Source string
\param length Length of source string */
FOUNDATION_API string_t
string_copy(char* dst, size_t capacity, const char* src, size_t length);

/*! \brief Resize string
Reallocate a string and fill with the given character. If the current buffer capacity
is greater than the requested length the buffer is not reallocated. If the requested
length is equal greater than the capacity, the buffer is reallocated to (new_length+1)
bytes. The resulting string is always zero terminated.
\param str String
\param length Current length of string
\param capacity Current capacity of string buffer
\param new_length Requested new length of string
\param fill Fill character
\return New string of requested length padded with given character */
FOUNDATION_API string_t
string_resize(char* str, size_t length, size_t capacity, size_t new_length, char fill);

/*! \brief Replace substrings
Replace all occurrences of the given key string inside the given string, optionally repeating
the replace after an occurrence have been replaced. If repeat is set to false, the newly
replaced part of the string is not rechecked for replacement. The repeat is recursion safe
and will not endlessly loop, like repeat replacing "foo" with "foobar". The string will not
be reallocated, replacement will continue in a best effort way until the capacity of the
string buffer is reached.
\param str String to replace in
\param length Length of string
\param capacity Capacity of string bufgfer
\param key Key string
\param key_length Length of key string
\param newkey String to replace key with
\param newkey_length Length of string to replace key with
\param repeat Repeat flag
\return Final replaced string */
FOUNDATION_API string_t
string_replace(char* str, size_t length, size_t capacity, const char* key, size_t key_length,
               const char* newkey, size_t newkey_length, bool repeat);

/*! \brief Concatenate strings
Allocate a new string which is the concatenation of the given two strings. The allocated
string will be zero terminated.
\param prefix Prefix string
\param prefix_length Length of prefix string
\param suffix Suffix string
\param suffix_length Length of suffix string
\return Newly allocated string which is concatenation of prefix and suffix */
FOUNDATION_API string_t
string_allocate_concat(const char* prefix, size_t prefix_length, const char* suffix,
                       size_t suffix_length);

/*! \brief Concatenate strings
Allocate a new string which is the concatenation of the given variable number of strings.
The argument list should be pairs of (char*, size_t) arguments specifying strings with
given length to concatenate. The list should be terminated with a null pointer. The
resulting allocated string will be zero terminated.
\param prefix Prefix string
\param prefix_length Length of prefix string
\param suffix Suffix string
\param suffix_length Length of suffix string
\return Newly allocated string which is concatenation of all given strings */
FOUNDATION_API string_t
string_allocate_concat_varg(const char* prefix, size_t prefix_length, const char* suffix,
                            size_t suffix_length, ...)
FOUNDATION_ATTRIBUTE(sentinel);

/*! \brief Concatenate strings
Allocate a new string which is the concatenation of the given variable number of strings.
The argument list should be pairs of (char*, size_t) arguments specifying strings with
given length to concatenate. The list should be terminated with a null pointer. The
resulting allocated string will be zero terminated.
\param list Variable argument list of (char*, size_t) tuples
\return Newly allocated string which is concatenation of all given strings */
FOUNDATION_API string_t
string_allocate_concat_vlist(va_list list);

/*! \brief Concatenate strings
Concatenate two strings into the given buffer. The resulting string will be zero terminated.
Will copy at most (capacity-1) bytes from source strings.
\param str Destination string buffer
\param capacity Capacity of destination string buffer.
\param prefix Prefix string
\param prefix_length Length of prefix string
\param suffix Suffix string
\param suffix_length Length of suffix string
\return String in given buffer which is concatenation of prefix and suffix */
FOUNDATION_API string_t
string_concat(char* str, size_t capacity, const char* prefix, size_t prefix_length,
              const char* suffix, size_t suffix_length);

/*! \brief Concatenate strings
Concatenate variable number of strings into the given buffer. The resulting string will be
zero terminated. Will copy at most (capacity-1) bytes from source strings. The argument list
should be pairs of (char*, size_t) arguments specifying strings with given length to
concatenate. The list should be terminated with a null pointer.
\param str Destination string buffer
\param capacity Capacity of destination string buffer.
\param prefix Prefix string
\param prefix_length Length of prefix string
\param suffix Suffix string
\param suffix_length Length of suffix string
\return String in given buffer which is concatenation of given strings */
FOUNDATION_API string_t
string_concat_varg(char* str, size_t capacity, const char* prefix, size_t prefix_length,
                   const char* suffix, size_t suffix_length, ...)
FOUNDATION_ATTRIBUTE(sentinel);

/*! \brief Concatenate strings
Concatenate variable number of strings into the given buffer. The resulting string will be
zero terminated. Will copy at most (capacity-1) bytes from source strings. The argument list
should be pairs of (char*, size_t) arguments specifying strings with given length to
concatenate. The list should be terminated with a null pointer.
\param str Destination string buffer
\param capacity Capacity of destination string buffer.
\param list Variable argument list of (char*, size_t) tuples
\return String in given buffer which is concatenation of given strings */
FOUNDATION_API string_t
string_concat_vlist(char* str, size_t capacity, va_list list);

/*! \brief Append suffix
Append a suffix string to a base string. The string will not be reallocated, the appending
will continue in a best effort way until the capacity of the destination buffer is reached.
The resulting string will be zero terminated.
\param str Base string
\param length Length of base string
\param capacity Capacity of base string buffer
\param suffix Suffix string
\param suffix_length Length of suffix string
\return String in given buffer which is base string with suffix string appended (base+suffix) */
FOUNDATION_API string_t
string_append(char* str, size_t length, size_t capacity, const char* suffix, size_t suffix_length);

/*! \brief Append suffix
Append a variable number of suffix strings to a base string. The string will not be reallocated,
the appending will continue in a best effort way until the capacity of the destination buffer
is reached. The variable argument list should be (char*, size_t) tuples giving additional
suffix strings to append, terminated by a null pointer argument. The resulting string will be
zero terminated.
\param str Base string
\param length Length of base string
\param capacity Capacity of base string buffer
\param suffix Suffix string
\param suffix_length Length of suffix string
\return String in given buffer which is base string with suffix strings appended (base+suffix) */
FOUNDATION_API string_t
string_append_varg(char* str, size_t length, size_t capacity, const char* suffix,
                   size_t suffix_length, ...)
FOUNDATION_ATTRIBUTE(sentinel);

/*! \brief Append suffix
Append a variable number of suffix strings to a base string. The string will not be reallocated,
the appending will continue in a best effort way until the capacity of the destination buffer
is reached. The variable argument list should be (char*, size_t) tuples giving additional
suffix strings to append, terminated by a null pointer argument. The resulting string will be
zero terminated.
\param str Base string
\param length Length of base string
\param capacity Capacity of base string buffer
\param list Variable argument list of (char*, size_t) tuples
\return String in given buffer which is base string with suffix strings appended (base+suffix) */
FOUNDATION_API string_t
string_append_vlist(char* str, size_t length, size_t capacity, va_list list);

/*! \brief Prepend suffix
Prepend a suffix string to a base string. The string will not be reallocated, the prepending
will continue in a best effort way until the capacity of the destination buffer is reached.
The resulting string will be zero terminated. Given prefix fragments are prepended in order,
meaning the last argument will be the first string fragment in the concatenated final string.
\param str Base string
\param length Length of base string
\param capacity Capacity of base string buffer
\param prefix Prefix string
\param prefix_length Length of prefix string
\return String in given buffer which is base string with prefix string prepended (prefix+base) */
FOUNDATION_API string_t
string_prepend(char* str, size_t length, size_t capacity, const char* prefix, size_t prefix_length);

/*! \brief Prepend suffix
Prepend a variable number of prefix strings to a base string. The string will not be reallocated,
the prepending will continue in a best effort way until the capacity of the destination buffer
is reached. The variable argument list should be (char*, size_t) tuples giving additional
suffix strings to append, terminated by a null pointer argument. The resulting string will be
zero terminated. Given prefix fragments are prepended in order, meaning the last argument will
be the first string fragment in the concatenated final string.
\param str Base string
\param length Length of base string
\param capacity Capacity of base string buffer
\param prefix Prefix string
\param prefix_length Length of prefix string
\return String in given buffer which is base string with prefix strings prepended (prefix+base) */
FOUNDATION_API string_t
string_prepend_varg(char* str, size_t length, size_t capacity, const char* prefix,
                    size_t prefix_length, ...)
FOUNDATION_ATTRIBUTE(sentinel);

/*! \brief Prepend suffix
Prepend a variable number of prefix strings to a base string. The string will not be reallocated,
the prepending will continue in a best effort way until the capacity of the destination buffer
is reached. The variable argument list should be (char*, size_t) tuples giving additional
suffix strings to append, terminated by a null pointer argument. The resulting string will be
zero terminated. Given prefix fragments are prepended in order, meaning the last argument will
be the first string fragment in the concatenated final string.
\param str Base string
\param length Length of base string
\param capacity Capacity of base string buffer
\param list Variable argument list of (char*, size_t) tuples
\return String in given buffer which is base string with prefix strings prepended (prefix+base) */
FOUNDATION_API string_t
string_prepend_vlist(char* str, size_t length, size_t capacity, va_list list);

/*! \brief Substring
Get substring of a string. Substring range will be clamped to source string limits. Returned
string pointer is poiting to a address in the input string buffer, and is NOT zero terminated.
\param str Source string
\param length Length of source string
\param offset Start offset
\param sub_length Length of substring
\return Substring pointing to given subsegment of input string */
FOUNDATION_API string_const_t
string_substr(const char* str, size_t length, size_t offset, size_t sub_length);

/*! \brief Strip string
Strip a string of given characters at start and end by returning a substring with offset
and length calculated to strip the given characters. The returned substring is NOT zero
terminated. If null is passed as string to strip, or if all characters are stripped,
a null string is returned.
\param str String to strip
\param length Length of string
\param delimiters Characters to strip
\param delim_length Length of characters to strip
\return Stipped substring of input string */
FOUNDATION_API string_const_t
string_strip(const char* str, size_t length, const char* delimiters, size_t delim_length);

FOUNDATION_API size_t
string_find(const char* str, size_t length, char c, size_t offset);

FOUNDATION_API size_t
string_find_string(const char* str, size_t length, const char* key, size_t key_length,
                   size_t offset);

FOUNDATION_API size_t
string_rfind(const char* str, size_t length, char c, size_t offset);

FOUNDATION_API size_t
string_rfind_string(const char* str, size_t length, const char* key, size_t key_length,
                    size_t offset);

FOUNDATION_API size_t
string_find_first_of(const char* str, size_t length, const char* key, size_t key_length,
                     size_t offset);

FOUNDATION_API size_t
string_find_last_of(const char* str, size_t length, const char* key, size_t key_length,
                    size_t offset);

FOUNDATION_API size_t
string_find_first_not_of(const char* str, size_t length, const char* key, size_t key_length,
                         size_t offset);

FOUNDATION_API size_t
string_find_last_not_of(const char* str, size_t length, const char* key, size_t key_length,
                        size_t offset);

FOUNDATION_API bool
string_ends_with(const char* str, size_t length, const char* suffix, size_t suffix_length);

FOUNDATION_API bool
string_equal(const char* lhs, size_t lhs_length, const char* rhs, size_t rhs_length);

FOUNDATION_API bool
string_equal_substr(const char* lhs, size_t lhs_length, size_t lhs_offset, const char* rhs,
                    size_t rhs_length, size_t rhs_offset);

FOUNDATION_API bool
string_match_pattern(const char* str, size_t length, const char* pattern, size_t pattern_length);

FOUNDATION_API void
string_split(const char* str, size_t length, const char* separators, size_t sep_length,
             string_const_t* left, string_const_t* right, bool allowempty);

FOUNDATION_API size_t
string_explode(const char* str, size_t length, const char* delimiters, size_t delim_length,
               string_const_t* arr, size_t arrsize, bool allow_empty);

FOUNDATION_API string_t
string_merge(char* dst, size_t capacity, const string_const_t* array, size_t array_size,
             const char* delimiter, size_t delim_length);

FOUNDATION_API string_t
string_merge_varg(char* dst, size_t capacity, const char* delimiter, size_t delim_length,
                  const char* str, size_t length, ...)
FOUNDATION_ATTRIBUTE(sentinel);

FOUNDATION_API string_t
string_merge_vlist(char* dst, size_t capacity, const char* delimiter, size_t delim_length,
                   va_list list);

FOUNDATION_API uint32_t
string_glyph(const char* str, size_t length, size_t offset, size_t* consumed);

FOUNDATION_API ssize_t
string_array_find(const string_const_t* haystack, size_t haystack_size, const char* needle,
                  size_t needle_length);

FOUNDATION_API void
string_array_deallocate_elements(string_t* array);

#define string_array_deallocate( array ) \
  string_array_deallocate_elements( array ); \
  array_deallocate( array ); \
  (array) = 0

FOUNDATION_API wchar_t*
wstring_allocate_from_string(const char* cstr, size_t cstr_length);

FOUNDATION_API void
wstring_from_string(wchar_t* str, size_t length, const char* cstr, size_t cstr_length);

FOUNDATION_API void
wstring_deallocate(wchar_t* str);

FOUNDATION_API size_t
wstring_length(const wchar_t* str);

FOUNDATION_API bool
wstring_equal(const wchar_t* lhs, const wchar_t* rhs);

FOUNDATION_API string_t
string_allocate_from_wstring(const wchar_t* str, size_t length);

FOUNDATION_API string_t
string_allocate_from_utf16(const uint16_t* str, size_t length);

FOUNDATION_API string_t
string_allocate_from_utf32(const uint32_t* str, size_t length);

FOUNDATION_API void
string_convert_utf16(char* dst, size_t dst_length, const uint16_t* src, size_t src_length);

FOUNDATION_API void
string_convert_utf32(char* dst, size_t dst_length, const uint32_t* src, size_t src_length);

FOUNDATION_API string_t
string_from_int(char* str, size_t length, int64_t val, unsigned int width, char padding);

FOUNDATION_API string_t
string_from_uint(char* str, size_t length, uint64_t val, bool hex, unsigned int width,
                 char padding);

FOUNDATION_API string_t
string_from_uint128(char* str, size_t length, const uint128_t val);

FOUNDATION_API string_t
string_from_real(char* str, size_t length, real val, unsigned int precision, unsigned int width,
                 char padding);

FOUNDATION_API string_t
string_from_time(char* str, size_t length, tick_t time);

FOUNDATION_API string_t
string_from_uuid(char* str, size_t length, const uuid_t uuid);

FOUNDATION_API string_t
string_from_version(char* str, size_t length, const version_t version);

FOUNDATION_API string_const_t
string_from_int_static(int64_t val, unsigned int width, char padding);

FOUNDATION_API string_const_t
string_from_uint_static(uint64_t val, bool hex, unsigned int width, char padding);

FOUNDATION_API string_const_t
string_from_uint128_static(const uint128_t val);

FOUNDATION_API string_const_t
string_from_real_static(real val, unsigned int precision, unsigned int width, char padding);

FOUNDATION_API string_const_t
string_from_time_static(tick_t time);

FOUNDATION_API string_const_t
string_from_uuid_static(const uuid_t uuid);

FOUNDATION_API string_const_t
string_from_version_static(const version_t version);

FOUNDATION_API int
string_to_int(const char* str, size_t length);

FOUNDATION_API unsigned int
string_to_uint(const char* str, size_t length, bool hex);

FOUNDATION_API int64_t
string_to_int64(const char* str, size_t length);

FOUNDATION_API uint64_t
string_to_uint64(const char* str, size_t length, bool hex);

FOUNDATION_API uint128_t
string_to_uint128(const char* str, size_t length);

FOUNDATION_API float32_t
string_to_float32(const char* str, size_t length);

FOUNDATION_API float64_t
string_to_float64(const char* str, size_t length);

FOUNDATION_API real
string_to_real(const char* str, size_t length);

FOUNDATION_API uuid_t
string_to_uuid(const char* str, size_t length);

FOUNDATION_API version_t
string_to_version(const char* str, size_t length);

FOUNDATION_API string_t
string_thread_buffer(void);

#define STRING_NPOS            ((size_t)-1)
#define STRING_NEWLINE         "\n"
#define STRING_EMPTY           ""
#define STRING_WHITESPACE      " \n\r\t\v\f"
#define WSTRING_WHITESPACE    L" \n\r\t\v\f"

// Implementation

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_null(void) {
  return (string_const_t) { 0, 0 };
}


static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_empty(void) {
  return (string_const_t) { STRING_EMPTY, 0 };
}


static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_const(const char* str, size_t length) {
  return (string_const_t) { str, length };
}


static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_to_const(string_t str) {
  return (string_const_t) { str.str, str.length };
}

