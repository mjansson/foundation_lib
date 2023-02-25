/* string.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#pragma once

/*! \file string.h
\brief String utility functions

String handling and utility functions in UTF-8, conversion to/from UTF-16. Conversion
to and from pritimive integral data types. Unless otherwise noted in the function descripion it
is only safe to pass null pointers to any arguments if the corresponding length and/or capacity
argument is also zero.

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

/*! Allocate capacity bytes of memory for a string of length characters. The memory block will be
initialized to zero. The length is capped to [0,capacity-1]. A null string is returned if
a zero capacity is given.
\param length Length of string
\param capacity Capacity of memory block allocated
\return String with given length in zero initialized memory block with given capacity */
FOUNDATION_API string_t
string_allocate(size_t length, size_t capacity);

/*! Clone a string of the given length. Allocates a memory block of capacity (length+1) and copies
the content of the given string. Zero terminates the new string. Will allocate a new memory
block even for null/empty string arguments.
\param str String to clone
\param length Length of string to clone
\return Zero-terminated cloned string of given length stored in a new memory block
        of capacity (length+1) */
FOUNDATION_API string_t
string_clone(const char* str, size_t length);

/*! Clone a string. Allocates a memory block of capacity (length+1) and copies the content of
the given string. Zero terminates the new string. Will allocate a new memory block even
for a null/empty string argument.
\param str String to clone
\return Zero-terminated cloned string of given length stored in a new memory block
        of capacity (length+1) */
FOUNDATION_API string_t
string_clone_string(string_const_t str);

/*! Deallocate memory used by the given string
\param str String to deallocate */
FOUNDATION_API void
string_deallocate(char* str);

/*! Declare an null string of zero length pointing to null.
\return String with zero length pointing to null. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_null(void);

/*! Declare an empty string of zero length pointing to a zero-terminated empty string. The
string pointer must not be deallocated.
\return String with zero length pointing to a zero-terminated empty string. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_empty(void);

/*! Declare a string of the given length pointing to a memory block with the
given address. String is not guaranteed to be zero terminated.
\param str String memory block
\param length length Length of string
\return String with given length pointing to a const memory block of at least given
        length in size. Not guaranteed to be zero terminated. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_t
string(char* str, size_t length);

/*! Declare a const string of the given length pointing to a const memory block with the
given address. String is not guaranteed to be zero terminated.
\param str String memory block
\param length length Length of string
\return String with given length pointing to a const memory block of at least given
        length in size. Not guaranteed to be zero terminated. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_const(const char* str, size_t length);

/*! Cast a given string to a const string with the same memory pointer and length.
\param str String to cast
\return Const string of same length pointing to same memory block as given string. */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_to_const(string_t str);

/*! \fn string_t string_allocate_format(const char* format, size_t length, ...)
Allocate a new string from a format specifier and variable data, printf style.
The format specifier must be a string literal of specified length, zero terminated.
\param format Format specifier
\param length Length of format specifier
\return Formatted string in a newly allocate memory buffer, zero terminated */
FOUNDATION_API string_t
string_allocate_format(const char* format, size_t length, ...) FOUNDATION_PRINTFCALL(1, 3);

/*! \fn string_t string_format(char* buffer, size_t capacity, const char* format, size_t length,
...) In-memory string formatting from a format specifier and variable data, printf style. The format
specifier must be a string literal of specified length, zero terminated. Will print at most
(capacity-1) characters into the buffer and always zero terminate. If buffer or capacity is null the
returned string is null, otherwise the string is always returned with buffer pointer and length [0,
capacity-1]. \param buffer Destination buffer \param capacity Capacity of destination buffer \param
format Format specifier \param length Length of format specifier \return Formatted string in given
buffer, zero terminated */
FOUNDATION_API string_t
string_format(char* buffer, size_t capacity, const char* format, size_t length, ...) FOUNDATION_PRINTFCALL(3, 5);

/*! \fn string_t string_allocate_vformat(const char* format, size_t length, va_list list)
Allocate a new string from a format specifier and variable data given as a va_list,
printf style. The format specifier must be a string literal of specified length,
zero terminated. If format or length is null the returned string is null, otherwise
a string buffer is always allocated and returned.
\param format Format specifier
\param length Length of format specifier
\param list Variable argument list
\return Formatted string in a newly allocate memory buffer, zero terminated */
FOUNDATION_API string_t
string_allocate_vformat(const char* format, size_t length, va_list list) FOUNDATION_PRINTFCALL(1, 0);

/*! \fn string_t string_vformat(char* buffer, size_t capacity, const char* format, size_t length,
va_list list) In-memory string formatting from a format specifier and variable data given as a
va_list, printf style. The format specifier must be a string literal of specified length, zero
terminated. Will print at most (capacity-1) characters into the buffer and always zero terminate the
string. If buffer or capacity is null the returned string is null, otherwise the string is always
returned with buffer pointer and length [0, capacity-1] \param buffer Destination buffer \param
capacity Capacity of destination buffer \param format Format specifier \param length Length of
format specifier \param list Variable argument list \return Formatted string in given buffer, zero
terminated */
FOUNDATION_API string_t
string_vformat(char* buffer, size_t capacity, const char* format, size_t length, va_list list)
    FOUNDATION_PRINTFCALL(3, 0);

/*! \fn size_t string_length(const char* str)
Get length of string in bytes. String must be zero terminated. Safe to pass a null
pointer, in which case the string length returned is zero.
\param str String
\return Length of string in bytes */
FOUNDATION_API size_t
string_length(const char* str);

/*! Get number of unicode glyphs stored in utf-8 string. This method is safe to call with
invalid utf-8 sequences, even with incomplete sequences at end of string.
\param str String in utf-8 encoding
\param length Length of string
\return Number of unicode glyphs in string */
FOUNDATION_API size_t
string_glyphs(const char* str, size_t length);

/*! Get number of bytes used by the ut-8 encoded character. This method is safe to call with
invalid utf-8 sequences, even with incomplete sequences at end of string. Returns -1 if invalid
utf-8 sequence.
\param str String in utf-8 encoding
\param length Length of string
\return Number of bytes used by the first unicode glyph in string */
FOUNDATION_API int
string_glyph_length(const char* str, size_t length);

/*! Calculate hash of string.
\param str String
\param length Length of string
\return Hash of string */
FOUNDATION_API hash_t
string_hash(const char* str, size_t length);

/*! Copy one string to another. Like strlcpy in that dst will always be zero terminated,
i.e copies at most (capacity-1) characters from source string. Safe to pass null
pointers in both pointer arguments.
\param dst Destination buffer
\param capacity Capacity of destination buffer
\param src Source string
\param length Length of source string
\return Destination string in given buffer */
FOUNDATION_API string_t
string_copy(char* FOUNDATION_RESTRICT dst, size_t capacity, const char* FOUNDATION_RESTRICT src, size_t length);

/*! Reallocate a string and fill with the given character. If the current buffer capacity
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

/*! Replace all occurrences of the given key string inside the given string, optionally repeating
the replace after an occurrence have been replaced.

If repeat is set to false, the newly replaced part of the string is not rechecked for
replacement. The repeat is recursion safe and will not endlessly loop, like repeat replacing
"foo" with "foobar".

The string will not be reallocated, replacement will continue in a best effort way until the
capacity of the string buffer is reached.

If string length is modified it will be zero terminated. If the length is unmodified the zero
termination state will be unmodified as well.
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
string_replace(char* str, size_t length, size_t capacity, const char* key, size_t key_length, const char* newkey,
               size_t newkey_length, bool repeat);

/*! Allocate a new string which is the concatenation of the given two strings.. Returned
string is never null, a string is always allocated
and is always zero terminated.
\param prefix Prefix string
\param prefix_length Length of prefix string
\param suffix Suffix string
\param suffix_length Length of suffix string
\return Newly allocated string which is concatenation of prefix and suffix */
FOUNDATION_API string_t
string_allocate_concat(const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length);

/*! Allocate a new string which is the concatenation of the given variable number of strings.
The argument list must be pairs of (char*, size_t) arguments specifying strings with
given length to concatenate. The list must be terminated with a null pointer. Returned
string is never null, a string is always allocated and is always zero terminated.
\param prefix Prefix string
\param prefix_length Length of prefix string
\param suffix Suffix string
\param suffix_length Length of suffix string
\return Newly allocated string which is concatenation of all given strings */
FOUNDATION_API string_t
string_allocate_concat_varg(const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length, ...)
    FOUNDATION_ATTRIBUTE(sentinel);

/*! Allocate a new string which is the concatenation of the given variable number of strings.
The argument list must be pairs of (char*, size_t) arguments specifying strings with
given length to concatenate. The list must be terminated with a null pointer. The
resulting allocated string will be zero terminated.
\param list Variable argument list of (char*, size_t) tuples
\return Newly allocated string which is concatenation of all given strings */
FOUNDATION_API string_t
string_allocate_concat_vlist(va_list list);

/*! Concatenate two strings into the given buffer. The resulting string will be zero terminated.
Will copy at most (capacity-1) bytes from source strings.
\param str Destination string buffer
\param capacity Capacity of destination string buffer.
\param prefix Prefix string
\param prefix_length Length of prefix string
\param suffix Suffix string
\param suffix_length Length of suffix string
\return String in given buffer which is concatenation of prefix and suffix */
FOUNDATION_API string_t
string_concat(char* str, size_t capacity, const char* prefix, size_t prefix_length, const char* suffix,
              size_t suffix_length);

/*! Concatenate variable number of strings into the given buffer. The resulting string will be
zero terminated. Will copy at most (capacity-1) bytes from source strings. The argument list
must be pairs of (char*, size_t) arguments specifying strings with given length to
concatenate. The list must be terminated with a null pointer.
\param str Destination string buffer
\param capacity Capacity of destination string buffer.
\param prefix Prefix string
\param prefix_length Length of prefix string
\param suffix Suffix string
\param suffix_length Length of suffix string
\return String in given buffer which is concatenation of given strings */
FOUNDATION_API string_t
string_concat_varg(char* str, size_t capacity, const char* prefix, size_t prefix_length, const char* suffix,
                   size_t suffix_length, ...) FOUNDATION_ATTRIBUTE(sentinel);

/*! Concatenate variable number of strings into the given buffer. The resulting string will be
zero terminated. Will copy at most (capacity-1) bytes from source strings. The argument list
must be pairs of (char*, size_t) arguments specifying strings with given length to
concatenate. The list must be terminated with a null pointer.
\param str Destination string buffer
\param capacity Capacity of destination string buffer.
\param list Variable argument list of (char*, size_t) tuples
\return String in given buffer which is concatenation of given strings */
FOUNDATION_API string_t
string_concat_vlist(char* str, size_t capacity, va_list list);

/*! Append a suffix string to a base string. The string will not be reallocated, the appending
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

/*! Append a variable number of suffix strings to a base string. The string will not be reallocated,
the appending will continue in a best effort way until the capacity of the destination buffer
is reached. The variable argument list must be (char*, size_t) tuples giving additional
suffix strings to append, terminated by a null pointer argument. The resulting string will be
zero terminated.
\param str Base string
\param length Length of base string
\param capacity Capacity of base string buffer
\param suffix Suffix string
\param suffix_length Length of suffix string
\return String in given buffer which is base string with suffix strings appended (base+suffix) */
FOUNDATION_API string_t
string_append_varg(char* str, size_t length, size_t capacity, const char* suffix, size_t suffix_length, ...)
    FOUNDATION_ATTRIBUTE(sentinel);

/*! Append a variable number of suffix strings to a base string. The string will not be reallocated,
the appending will continue in a best effort way until the capacity of the destination buffer
is reached. The variable argument list must be (char*, size_t) tuples giving additional
suffix strings to append, terminated by a null pointer argument. The resulting string will be
zero terminated.
\param str Base string
\param length Length of base string
\param capacity Capacity of base string buffer
\param list Variable argument list of (char*, size_t) tuples
\return String in given buffer which is base string with suffix strings appended (base+suffix) */
FOUNDATION_API string_t
string_append_vlist(char* str, size_t length, size_t capacity, va_list list);

/*! Prepend a suffix string to a base string. The string will not be reallocated, the prepending
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

/*! Prepend a variable number of prefix strings to a base string. The string will not be
reallocated, the prepending will continue in a best effort way until the capacity of the destination
buffer is reached. The variable argument list must be (char*, size_t) tuples giving additional
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
string_prepend_varg(char* str, size_t length, size_t capacity, const char* prefix, size_t prefix_length, ...)
    FOUNDATION_ATTRIBUTE(sentinel);

/*! Prepend a variable number of prefix strings to a base string. The string will not be
reallocated, the prepending will continue in a best effort way until the capacity of the destination
buffer is reached. The variable argument list must be (char*, size_t) tuples giving additional
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

/*! Get substring of a string. Substring range will be clamped to source string limits. Returned
string pointer is poiting to a address in the input string buffer, and is NOT zero terminated.
\param str Source string
\param length Length of source string
\param offset Start offset
\param sub_length Length of substring
\return Substring pointing to given subsegment of input string */
FOUNDATION_API string_const_t
string_substr(const char* str, size_t length, size_t offset, size_t sub_length);

/*! Strip a string of given characters at start and end by returning a substring with offset
and length calculated to strip the given characters. The returned substring is NOT zero
terminated. If null is passed as string to strip, or if all characters are stripped,
an empty string is returned.
\param str String to strip
\param length Length of string
\param delimiters Characters to strip
\param delim_length Length of characters to strip
\return Stipped substring of input string */
FOUNDATION_API string_const_t
string_strip(const char* str, size_t length, const char* delimiters, size_t delim_length);

/*! Search for a character in a string from the given offset
\param str Source string
\param length Length of source string
\param c Character to search for
\param offset Start offset of search within source string
\return Offset of first occurrence of c within str (greater or equal to offset).
        STRING_NPOS if not found */
FOUNDATION_API size_t
string_find(const char* str, size_t length, char c, size_t offset);

/*! Search for a key string in a string from the given offset
\param str Source string
\param length Length of source string
\param key Key string to search for
\param key_length Length of key string
\param offset Start offset of search within source string
\return Offset of first occurrence of key string within str (greater or equal to offset).
        STRING_NPOS if not found */
FOUNDATION_API size_t
string_find_string(const char* str, size_t length, const char* key, size_t key_length, size_t offset);

/*! Reverse search for a character in a string from the given offset
\param str Source string
\param length Length of source string
\param c Character to search for
\param offset Start offset of search within source string. Pass STRING_NPOS to
              search from end.
\return Offset of first occurrence of c within str (less or equal to offset).
        STRING_NPOS if not found */
FOUNDATION_API size_t
string_rfind(const char* str, size_t length, char c, size_t offset);

/*! Reverse search for a key string in a string from the given offset
\param str Source string
\param length Length of source string
\param key Key string to search for
\param key_length Length of key string
\param offset Start offset of search within source string. Pass STRING_NPOS to search
              from end.
\return Offset of first occurrence of key string within str (less or equal to offset).
        STRING_NPOS if not found */
FOUNDATION_API size_t
string_rfind_string(const char* str, size_t length, const char* key, size_t key_length, size_t offset);

/*! Search for the first occurrence of any character in key string in a string from the
given offset
\param str Source string
\param length Length of source string
\param key Key characters to search for
\param key_length Length of key characters string
\param offset Start offset of search within source string
\return Offset of first occurrence of any of the characters in the key string within
        str (greater or equal to offset). STRING_NPOS if none found */
FOUNDATION_API size_t
string_find_first_of(const char* str, size_t length, const char* key, size_t key_length, size_t offset);

/*! Search for the last occurrence of any character in key string in a string from the
given offset
\param str Source string
\param length Length of source string
\param key Key characters to search for
\param key_length Length of key characters string
\param offset Start offset of search within source string
\return Offset of last occurrence of any of the characters in the key string within
        str (greater or equal to offset). STRING_NPOS if none found */
FOUNDATION_API size_t
string_find_last_of(const char* str, size_t length, const char* key, size_t key_length, size_t offset);

/*! Search for the first occurrence of any character not present in key string in a string
from the given offset
\param str Source string
\param length Length of source string
\param key Key characters to search for
\param key_length Length of key characters string
\param offset Start offset of search within source string
\return Offset of first occurrence of any character which is not part of the key string
        within str (greater or equal to offset). STRING_NPOS if none found */
FOUNDATION_API size_t
string_find_first_not_of(const char* str, size_t length, const char* key, size_t key_length, size_t offset);

/*! Search for the last occurrence of any character not present in key string in a string
from the given offset
\param str Source string
\param length Length of source string
\param key Key characters to search for
\param key_length Length of key characters string
\param offset Start offset of search within source string
\return Offset of last occurrence of any character which is not part of the key string
        within str (greater or equal to offset). STRING_NPOS if none found */
FOUNDATION_API size_t
string_find_last_not_of(const char* str, size_t length, const char* key, size_t key_length, size_t offset);

/*! Query if string ends with the given suffix
\param str String
\param length Length of string
\param suffix Suffix
\param suffix_length Length of suffix
\return True if string ends with the given suffix, false if not */
FOUNDATION_API bool
string_ends_with(const char* str, size_t length, const char* suffix, size_t suffix_length);

/*! Query if strings are equal (case sensitive)
\param lhs First string
\param lhs_length Length of first string
\param rhs Second string
\param rhs_length Length of second string
\return True if equal, false if not */
FOUNDATION_API bool
string_equal(const char* lhs, size_t lhs_length, const char* rhs, size_t rhs_length);

/*! Query if strings are equal (case insensitive)
\param lhs First string
\param lhs_length Length of first string
\param rhs Second string
\param rhs_length Length of second string
\return True if equal, false if not */
FOUNDATION_API bool
string_equal_nocase(const char* lhs, size_t lhs_length, const char* rhs, size_t rhs_length);

/*! Query if substrings are equal (case sensitive)
\param lhs First string
\param lhs_length Length of first string
\param lhs_offset Offset in first string
\param rhs Second string
\param rhs_length Length of second string
\param rhs_offset Offset in second string
\return True if substrings are equal in [offset,length) range, false if not */
FOUNDATION_API bool
string_equal_substr(const char* lhs, size_t lhs_length, size_t lhs_offset, const char* rhs, size_t rhs_length,
                    size_t rhs_offset);

/*! Query if substrings are equal (case insensitive)
\param lhs First string
\param lhs_length Length of first string
\param lhs_offset Offset in first string
\param rhs Second string
\param rhs_length Length of second string
\param rhs_offset Offset in second string
\return True if substrings are equal in [offset,length) range, false if not */
FOUNDATION_API bool
string_equal_substr_nocase(const char* lhs, size_t lhs_length, size_t lhs_offset, const char* rhs, size_t rhs_length,
                           size_t rhs_offset);

/*! Check if a string matches a given pattern using ? and * wildcards. For regular expression
matching see the regex.h module.
\param str String
\param length Length of string
\param pattern Pattern with ? and * wildcards
\param pattern_length Length of pattern
\return true if string matches given pattern, false if not */
FOUNDATION_API bool
string_match_pattern(const char* str, size_t length, const char* pattern, size_t pattern_length);

/*! Split a string into two strings along the first occurrence of any of the given separator
characters. The entire group of separators following the initial occurrence will be
removed in the split. For example, splitting "this is insidious" with separators "is "
will yield resulting strings "th" and "nsidious". Unless null the left and right string
pointers will be set to the found substrings. The allowempty flag controls if an
empty left part is allowed. If not, any leading separator characters will first be
discarded before processing split.
\param str Source string
\param length Length of source string
\param separators Separator characters
\param sep_length Length of separator characters
\param left Pointer to string which will receive new left part of split result
\param right Pointer to string which will receive new right part of split result
\param allowempty Empty flag */
FOUNDATION_API void
string_split(const char* str, size_t length, const char* separators, size_t sep_length, string_const_t* left,
             string_const_t* right, bool allowempty);

/*! Explode a string into substrings along given separator characters, optinally ignoring or
including empty substrings (consecutive separator characters).
\param str Source string
\param length Length of source string
\param delimiters Separator characters
\param delim_length Length of separator characters
\param arr Array of substrings
\param arrsize Size of substring array
\param allow_empty Flag to include empty substrings if true, ignore if false
\return Number of substrings stored in array */
FOUNDATION_API size_t
string_explode(const char* str, size_t length, const char* delimiters, size_t delim_length, string_const_t* arr,
               size_t arrsize, bool allow_empty);

/*! Merge a string array using the given separator string
\param dst Destination string buffer
\param capacity Capacity of the destination buffer
\param array Source string array
\param array_size Number of string in array to use in merge
\param delimiter Separator string to add between all source strings
\param delim_length Length of separator string
\return Merged string in destination buffer */
FOUNDATION_API string_t
string_merge(char* dst, size_t capacity, const string_const_t* array, size_t array_size, const char* delimiter,
             size_t delim_length);

/*! Merge a string list using the given separator string. The variable argument list shoud
be (const char*, size_t) pairs of strings to merge, terminated by a null pointer.
\param dst Destination string buffer
\param capacity Capacity of the destination buffer
\param delimiter Separator string to add between all source strings
\param delim_length Length of separator string
\param str First string
\param length Length of first string
\return Merged string in destination buffer */
FOUNDATION_API string_t
string_merge_varg(char* dst, size_t capacity, const char* delimiter, size_t delim_length, const char* str,
                  size_t length, ...) FOUNDATION_ATTRIBUTE(sentinel);

/*! Merge a string list using the given separator string. The variable argument list shoud
be (const char*, size_t) pairs of strings to merge, terminated by a null pointer.
\param dst Destination string buffer
\param capacity Capacity of the destination buffer
\param delimiter Separator string to add between all source strings
\param delim_length Length of separator string
\param list Variable argument list
\return Merged string in destination buffer */
FOUNDATION_API string_t
string_merge_vlist(char* dst, size_t capacity, const char* delimiter, size_t delim_length, va_list list);

/*! Get unicode glyph represented by the utf-8 enconding in the source string at the given
offset, optionally also reporting the number of consumed 8-bit characters in the process.
\param str utf-8 encoded string
\param length Length of string
\param offset Offset in source string
\param consumed Optional pointer to an unsigned int receiving the number of consumed
                8-bit characters used in constructing the unicode glyph
\return Unicode glyph constructed */
FOUNDATION_API uint32_t
string_glyph(const char* str, size_t length, size_t offset, size_t* consumed);

/*! Find a specific string in an array of strings
\param haystack Array of strings to check
\param haystack_size Size of array
\param needle String to find
\param needle_length Length of string to find
\return Index of string found, or <0 if no string matching */
FOUNDATION_API ssize_t
string_array_find(const string_const_t* haystack, size_t haystack_size, const char* needle, size_t needle_length);

/*! Deallocate all strings in an array of strings. Does not deallocate array itself.
\param array Array of strings */
FOUNDATION_API void
string_array_deallocate_elements(string_t* array);

/*! Deallocate all strings in an array and deallocate array itself. Array
will be reset to null.
\param array Array of strings */
#define string_array_deallocate(array)       \
	string_array_deallocate_elements(array); \
	array_deallocate(array);                 \
	(array) = 0

/*! Allocate a wide char string from utf-8 encoded string
\param cstr Source utf-8 encoded string
\param length Length of source string in bytes (NOT unicode glyphs)
\return Wide char string */
FOUNDATION_API wchar_t*
wstring_allocate_from_string(const char* cstr, size_t length);

/*! Convert a utf-8 encoded string into a preallocated wide char string
\param str Destination wide-char string
\param capacity Maximum number of wide characters to store in destination string,
                including null terminator
\param cstr Source utf-8 encoded string
\param length Length of source string in bytes (NOT unicode glyphs) */
FOUNDATION_API void
wstring_from_string(wchar_t* str, size_t capacity, const char* cstr, size_t length);

/*! Deallocate a wide character string
\param str String */
FOUNDATION_API void
wstring_deallocate(wchar_t* str);

/*! Get length of a wide character string
\param str Wide character string
\return Length in number of wide characters (not bytes) */
FOUNDATION_API size_t
wstring_length(const wchar_t* str);

/*! Compare wide character strings, case sensitive
\param lhs First string
\param rhs Second string
\return true if strings are equal, false if not */
FOUNDATION_API bool
wstring_equal(const wchar_t* lhs, const wchar_t* rhs);

/*! Allocate a new utf-8 string representing the given wide character string
\param str Wide character string
\param length Length of wide character string in number of wide characters (not bytes)
\return Newly allocated utf-8 encoded string */
FOUNDATION_API string_t
string_allocate_from_wstring(const wchar_t* str, size_t length);

/*! Allocate a new utf-8 string representing the given utf-16 encoded string
\param str utf-16 encoded string
\param length Length of utf-16 string in number of 16-bit characters (not bytes)
\return Newly allocated utf-8 encoded string */
FOUNDATION_API string_t
string_allocate_from_utf16(const uint16_t* str, size_t length);

/*! Allocate a new utf-8 string representing the given utf-32 encoded string
\param str utf-32 encoded string
\param length Length of utf-32 string in number of 32-bit characters (not bytes)
\return Newly allocated utf-8 encoded string */
FOUNDATION_API string_t
string_allocate_from_utf32(const uint32_t* str, size_t length);

/*! Convert an utf-16 string into a preallocated utf-8 string. Returned string will be
zero terminated (included in capacity).
\param dst Destination utf-8 string
\param capacity Capacity of destination buffer
\param src Source utf-16 string
\param length Length of source string in utf-16 characters (not bytes),
              not including terminating zero
\return Converted string in given buffer */
FOUNDATION_API string_t
string_convert_utf16(char* dst, size_t capacity, const uint16_t* src, size_t length);

/*! Convert an utf-32 string into a preallocated utf-8 string. Returned string will be
zero terminated (included in capacity).
\param dst Destination utf-8 string
\param capacity Capacity of destination buffer
\param src Source utf-32 string
\param length Length of source string in utf-32 characters (not bytes),
              not including terminating zero
\return Converted string in given buffer */
FOUNDATION_API string_t
string_convert_utf32(char* dst, size_t capacity, const uint32_t* src, size_t length);

/*! Convert an integer to a string, with optional field width and fill character. String buffer
should be at least 12 bytes (11 characters + terminating zero). String will be zero
terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param val Integer value
\param width Field width
\param padding Fill character
\return String in given buffer */
FOUNDATION_API string_t
string_from_int(char* str, size_t capacity, int64_t val, unsigned int width, char padding);

/*! Convert an unsigned integer to a string, with optional hexadecimal base and base prefix,
field width and fill character. String buffer should be at least 12 bytes (11 characters
+ terminating zero). String will be zero terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param val Integer value
\param hex Hexadecimal flag
\param width Field width
\param padding Fill character
\return String in given buffer */
FOUNDATION_API string_t
string_from_uint(char* str, size_t capacity, uint64_t val, bool hex, unsigned int width, char padding);

/*! Convert an 128-bit unsigned integer to a string (represented in hex format). String buffer
should be at least 33 bytes (32 characters + terminating zero). String will be zero
terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param val Integer value
\return String in given buffer */
FOUNDATION_API string_t
string_from_uint128(char* str, size_t capacity, const uint128_t val);

/*! Convert an 256-bit unsigned integer to a string (represented in hex format). String buffer
should be at least 65 bytes (64 characters + terminating zero). String will be zero
terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param val Integer value
\return String in given buffer */
FOUNDATION_API string_t
string_from_uint256(char* str, size_t capacity, const uint256_t val);

/*! Convert an 512-bit unsigned integer to a string (represented in hex format). String buffer
should be at least 129 bytes (128 characters + terminating zero). String will be zero
terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param val Integer value
\return String in given buffer */
FOUNDATION_API string_t
string_from_uint512(char* str, size_t capacity, const uint512_t val);

/*! Convert a float to a string, with optional fixed notation, field width, precision and
fill character. String will be zero terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param val Float value
\param precision Precision
\param width Field width
\param padding Fill character
\return String in given buffer */
FOUNDATION_API string_t
string_from_real(char* str, size_t capacity, real val, unsigned int precision, unsigned int width, char padding);

/*! Convert a float to a string, with optional fixed notation, field width, precision and
fill character. String will be zero terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param val Float value
\param precision Precision
\param width Field width
\param padding Fill character
\return String in given buffer */
FOUNDATION_API string_t
string_from_float32(char* str, size_t capacity, float32_t val, unsigned int precision, unsigned int width,
                    char padding);

/*! Convert a float to a string, with optional fixed notation, field width, precision and
fill character. String will be zero terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param val Float value
\param precision Precision
\param width Field width
\param padding Fill character
\return String in given buffer */
FOUNDATION_API string_t
string_from_float64(char* str, size_t capacity, float64_t val, unsigned int precision, unsigned int width,
                    char padding);

/*! Get a formatted string of the given timestamp. String buffer should be at least
25 bytes (24 characters + terminating zero). A capacity of less than 25 characters
will result in an empty string result. The string will be formetted like
"Thu Jan 01 00:00:00 1970" and the timestamp is treated as either local time or as
UTC (no local timezone is taken into consideration). String will be zero terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param time Timestamp
\param local Local time if true, UTC if false
\return String in given buffer */
FOUNDATION_API string_t
string_from_time(char* str, size_t capacity, tick_t time, bool local);

/*! Get a formatted string of the given UUID. String buffer should be at least
37 bytes (36 characters + terminating zero). String will be zero terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param uuid UUID
\return String in given buffer */
FOUNDATION_API string_t
string_from_uuid(char* str, size_t capacity, const uuid_t uuid);

/*! Get a formatted string of the given version. String will be zero terminated.
\param str String buffer
\param capacity Capacity of string buffer.
\param version Version
\return String in given buffer */
FOUNDATION_API string_t
string_from_version(char* str, size_t capacity, const version_t version);

/*! Convert an integer into a thread-local conversion buffer, with optional field width and
fill character. The buffer is shared between all string_from_*_static functions and
only valid until next call to one of these functions.
\param val Integer value
\param width Field width
\param padding Fill character
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_int_static(int64_t val, unsigned int width, char padding);

/*! Convert an unsigned integer into a thread-local conversion buffer, with optional
hexadecimal base and base prefix, field width and fill character. The buffer is
shared between all string_from_*_static functions and only valid until next call to one
of these functions.
\param val Integer value
\param hex Hexadecimal flag
\param width Field width
\param padding Fill character
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_uint_static(uint64_t val, bool hex, unsigned int width, char padding);

/*! Convert an 128-bit unsigned integer into a thread-local conversion buffer. The
buffer is shared between all string_from_*_static functions and only valid until next
call to one of these functions.
\param val Integer value
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_uint128_static(const uint128_t val);

/*! Convert an 256-bit unsigned integer into a thread-local conversion buffer. The
buffer is shared between all string_from_*_static functions and only valid until next
call to one of these functions.
\param val Integer value
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_uint256_static(const uint256_t val);

/*! Convert an 512-bit unsigned integer into a thread-local conversion buffer. The
buffer is shared between all string_from_*_static functions and only valid until next
call to one of these functions.
\param val Integer value
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_uint512_static(const uint512_t val);

/*! Convert a float into a thread-local conversion buffer, with optional fixed notation,
field width, precision and fill character. The buffer is shared between all
string_from_*_static functions and only valid until next call to one of these functions.
\param val Float value
\param precision Precision
\param width Field width
\param padding Fill character
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_real_static(real val, unsigned int precision, unsigned int width, char padding);

/*! Convert a float into a thread-local conversion buffer, with optional fixed notation,
field width, precision and fill character. The buffer is shared between all
string_from_*_static functions and only valid until next call to one of these functions.
\param val Float value
\param precision Precision
\param width Field width
\param padding Fill character
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_float32_static(float32_t val, unsigned int precision, unsigned int width, char padding);

/*! Convert a float into a thread-local conversion buffer, with optional fixed notation,
field width, precision and fill character. The buffer is shared between all
string_from_*_static functions and only valid until next call to one of these functions.
\param val Float value
\param precision Precision
\param width Field width
\param padding Fill character
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_float64_static(float64_t val, unsigned int precision, unsigned int width, char padding);

/*! Convert a timestamp into a thread-local conversion buffer. The buffer is shared between all
string_from_*_static functions and only valid until next call to one of these functions.
The string will be formetted like "Thu Jan 01 00:00:00 1970" and the timestamp is treated
as either local time or as UTC (no local timezone is taken into consideration).
String will be zero terminated.
\param time Timestamp
\param local Local time if true, UTC if false
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_time_static(tick_t time, bool local);

/*! Convert an UUID into a thread-local conversion buffer. The buffer is shared between all
string_from_*_static functions and only valid until next call to one of these functions.
\param uuid UUID
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_uuid_static(const uuid_t uuid);

/*! Convert a version identifier into a thread-local conversion buffer. The buffer is shared
between all string_from_*_static functions and only valid until next call to one of
these functions.
\param version Version
\return String in thread-local buffer */
FOUNDATION_API string_const_t
string_from_version_static(const version_t version);

/*! Convert a string to a signed integer
\param str String
\param length Length of string
\return Integer value, 0 if conversion failed */
FOUNDATION_API int
string_to_int(const char* str, size_t length);

/*! Convert a string to an unsigned integer
\param str String
\param length Length of string
\param hex Hexadecimal flag
\return Unsigned integer value, 0 if conversion failed */
FOUNDATION_API unsigned int
string_to_uint(const char* str, size_t length, bool hex);

/*! Convert a string to a signed 64-bit integer
\param str String
\param length Length of string
\return 64-bit integer value, 0 if conversion failed */
FOUNDATION_API int64_t
string_to_int64(const char* str, size_t length);

/*! Convert a string to an unsigned 64-bit integer
\param str String
\param length Length of string
\param hex Hexadecimal flag
\return 64-bit unsigned integer value, 0 if conversion failed */
FOUNDATION_API uint64_t
string_to_uint64(const char* str, size_t length, bool hex);

/*! Convert a string in hex format to an unsigned 128-bit integer
\param str String
\param length Length of string
\return 128-bit integer value, 0 if conversion failed */
FOUNDATION_API uint128_t
string_to_uint128(const char* str, size_t length);

/*! Convert a string in hex format to an unsigned 256-bit integer
\param str String
\param length Length of string
\return 256-bit integer value, 0 if conversion failed */
FOUNDATION_API uint256_t
string_to_uint256(const char* str, size_t length);

/*! Convert a string in hex format to an unsigned 512-bit integer
\param str String
\param length Length of string
\return 512-bit integer value, 0 if conversion failed */
FOUNDATION_API uint512_t
string_to_uint512(const char* str, size_t length);

/*! Convert a string to a signed size integer
\param str String
\param length Length of string
\return Size integer value, 0 if conversion failed */
FOUNDATION_API ssize_t
string_to_ssize(const char* str, size_t length);

/*! Convert a string to an unsigned size integer
\param str String
\param length Length of string
\param hex Hexadecimal flag
\return Unsigned size integer value, 0 if conversion failed */
FOUNDATION_API size_t
string_to_size(const char* str, size_t length, bool hex);

/*! Convert a string to a 32-bit float
\param str String
\param length Length of string
\return Floating point value, 0 if conversion failed */
FOUNDATION_API float32_t
string_to_float32(const char* str, size_t length);

/*! Convert a string to a 64-bit float
\param str String
\param length Length of string
\return Floating point value, 0 if conversion failed */
FOUNDATION_API float64_t
string_to_float64(const char* str, size_t length);

/*! Convert a string to a real
\param str String
\param length Length of string
\return Floating point value, 0 if conversion failed */
FOUNDATION_API real
string_to_real(const char* str, size_t length);

/*! Convert a string to a UUID
\param str String
\param length Length of string
\return UUID, null UUID if conversion failed */
FOUNDATION_API uuid_t
string_to_uuid(const char* str, size_t length);

/*! Convert a string to a version identifier
\param str String
\param length Length of string
\return Version identifier, each field zero if conversion failed or data missing */
FOUNDATION_API version_t
string_to_version(const char* str, size_t length);

/*! Thread local buffer for string operations and conversions.
\return String thread local buffer with size indicating capacity */
FOUNDATION_API string_t
string_thread_buffer(void);

/*! Denotes an invalid string position (-1) */
#define STRING_NPOS ((size_t)-1)

/*! Newline character */
#define STRING_NEWLINE "\n"

/*! Empty string */
#define STRING_EMPTY ""

/*! Default whitespace characters */
#define STRING_WHITESPACE " \n\r\t\v\f"

/*! Default whitespace wide characters */
#define WSTRING_WHITESPACE L" \n\r\t\v\f"

/*! Hexadecimal characters */
#define STRING_HEX "0123456789abcdefABCDEF"

/*! Hexadecimal characters */
#define WSTRING_HEX L"0123456789abcdefABCDEF"

// Implementation

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_null(void) {
#ifdef __cplusplus
	const string_const_t s = {0, 0};
	return s;
#else
	return (string_const_t){0, 0};
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_empty(void) {
#ifdef __cplusplus
	const string_const_t s = {STRING_EMPTY, 0};
	return s;
#else
	return (string_const_t){STRING_EMPTY, 0};
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_t
string(char* str, size_t length) {
#ifdef __cplusplus
	const string_t s = {str, length};
	return s;
#else
	return (string_t){str, length};
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_const(const char* str, size_t length) {
#ifdef __cplusplus
	const string_const_t s = {str, length};
	return s;
#else
	return (string_const_t){str, length};
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL string_const_t
string_to_const(string_t str) {
#ifdef __cplusplus
	const string_const_t s = {str.str, str.length};
	return s;
#else
	return (string_const_t){str.str, str.length};
#endif
}
