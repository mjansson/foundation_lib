/* path.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file path.h
\brief Path utilities

Path utilities for unifying, merging, cleaning and extracting path parts.

All function expect the path to be of a well formed format using forward
slashes only, such as [protocol:[//]][drive:][/]path/to/some/file[.ext],
except for #path_clean. Use #path_clean to create well formed paths from unknown formats

Query functions to get subpath or parts of a path does not modify input buffers,
but return const strings specifying given substring of the input buffer. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Get base file name (no path and no extension) from full path name
\param path Full path
\param length Length of path
\return File base name */
FOUNDATION_API string_const_t
path_base_file_name(const char* path, size_t length);

/*! \details Get base file name with directory (i.e no extension) from full path name
\param path Full path
\param length Length of path
\return File base name with path */
FOUNDATION_API string_const_t
path_base_file_name_with_directory(const char* path, size_t length);

/*! Get file extension (string after last dot) from full path name, not including the dot.
\param path Full path
\param length Length of path
\return File extension */
FOUNDATION_API string_const_t
path_file_extension(const char* path, size_t length);

/*! Get file name (no directory but with extension) from full path name
\param path Full path
\param length Length of path
\return File full name */
FOUNDATION_API string_const_t
path_file_name(const char* path, size_t length);

/*! Get directory name from full path name. Any protocol in the path is included in
the returned path.
\param path Full path
\param length Length of path
\return Path name */
FOUNDATION_API string_const_t
path_directory_name(const char* path, size_t length);

/*! Get subpath (relative path) from full path name and root directory. If the full path is an
absolute path, the root directory must also be an absolute path (and vice versa). Protocols
will be compared when matching paths, with the exception that if either path or root
does not contain a protocol, it will implicitly match any protocol in the other argument.
\param path Full path
\param length Length of path
\param root Root full path
\param root_length Length of root path
\return Subpath within root directory, empty string if not in root directory */
FOUNDATION_API string_const_t
path_subpath(const char* path, size_t length, const char* root, size_t root_length);

/*! Get protocol from full URI (for example, "http://foo.com/some.file" will return "http")
\param uri URI
\param length Length of URI
\return Protocol */
FOUNDATION_API string_const_t
path_protocol(const char* uri, size_t length);

/*! Get path without protocol (for example, "file://foo/some.file" will return "/foo/some.file")
\param uri URI
\param length Length of URI
\return Path without protocol */
FOUNDATION_API string_const_t
path_strip_protocol(const char* uri, size_t length);

/*! Allocate a new string which is the concatenation of the given paths. The first
path part determines if path is absolute or relative. Note that an emtpy string
is considered to be a relative path.
\param first First path
\param first_length Length of first path
\param second Second path
\param second_length Length of second path
\return Concatenated path in new allocated memory buffer */
FOUNDATION_API string_t
path_allocate_concat(const char* first, size_t first_length, const char* second, size_t second_length);

/*! Allocate a new string which is the concatenation of the given paths. The first
path part determines if path is absolute or relative. Note that an emtpy string
is considered to be a relative path. The path list must be pairs of (const char*,
size_t) arguments and terminated by a null pointer.
\param first First path
\param first_length Length of first path
\return Concatenated path in new allocated memory buffer */
FOUNDATION_API string_t
path_allocate_concat_varg(const char* first, size_t first_length, ...) FOUNDATION_ATTRIBUTE(sentinel);

/*! Allocate a new string which is the concatenation of the given paths. The first
path part determines if path is absolute or relative. Note that an emtpy string
is considered to be a relative path. The path list must be pairs of (const char*,
size_t) arguments and terminated by a null pointer.
\param first First path
\param first_length Length of first path
\param list Additional paths argument list
\return Concatenated path in new allocated memory buffer */
FOUNDATION_API string_t
path_allocate_concat_vlist(const char* first, size_t first_length, va_list list);

/*! Concatenation of the given paths into the given buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. Returned path
is always zero terminated except if capacity is zero, in which case an empty string
is returned. The first path part determines if path is absolute or relative. Note that
an emtpy string is considered to be a relative path.
\param dest Destination buffer
\param capacity Capacity of destination buffer
\param first First path
\param first_length Length of first path
\param second Second path
\param second_length Length of second path
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_concat(char* dest, size_t capacity, const char* first, size_t first_length, const char* second,
            size_t second_length);

/*! Concatenation of the given paths into the given buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. The first path part
determines if path is absolute or relative. Note that an emtpy string is considered to be
a relative path. The path list must be pairs of (const char*, size_t) arguments and
terminated by a null pointer. Returned path is always zero terminated except if capacity
is zero, in which case an empty string is returned.
\param dest Destination buffer
\param capacity Capacity of destination buffer
\param first First path
\param first_length Length of first path
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_concat_varg(char* dest, size_t capacity, const char* first, size_t first_length, ...)
    FOUNDATION_ATTRIBUTE(sentinel);

/*! Concatenation of the given paths into the given buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. The first path part
determines if path is absolute or relative. Note that an emtpy string is considered to be
a relative path. The path list must be pairs of (const char*, size_t) arguments and
terminated by a null pointer. Returned path is always zero terminated except if capacity
is zero, in which case an empty string is returned.
\param dest Destination buffer
\param capacity Capacity of destination buffer
\param first First path
\param first_length Length of first path
\param list Path argument list
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_concat_vlist(char* dest, size_t capacity, const char* first, size_t first_length, va_list list);

/*! Append given path at the end of the given path buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. Returned path is
always zero terminated except if capacity is zero, in which case an empty string is
returned.
\param base Base path and target buffer
\param base_length Length of base path
\param base_capacity Capacity of buffer
\param tail Tail path to append
\param tail_length Length of tail path
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_append(char* base, size_t base_length, size_t base_capacity, const char* tail, size_t tail_length);

/*! Append given paths at the end of the given path buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. The path argument
list must be pairs of (const char*, size_t) arguments, terminated by a null pointer.
Returned path is always zero terminated except if capacity is zero, in which case an
empty string is returned.
\param base Base path and target buffer
\param base_length Length of base path
\param base_capacity Capacity of buffer
\param tail Tail path to append
\param tail_length Length of tail path
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_append_varg(char* base, size_t base_length, size_t base_capacity, const char* tail, size_t tail_length, ...)
    FOUNDATION_ATTRIBUTE(sentinel);

/*! Append given paths at the end of the given path buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. The path argument
list must be pairs of (const char*, size_t) arguments, terminated by a null pointer.
Returned path is always zero terminated except if capacity is zero, in which case an
empty string is returned.
\param base Base path and target buffer
\param base_length Length of base path
\param base_capacity Capacity of buffer
\param tail Tail path to append
\param tail_length Length of tail path
\param list Tail path argument list
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_append_vlist(char* base, size_t base_length, size_t base_capacity, const char* tail, size_t tail_length,
                  va_list list);

/*! Prepend given path at the start of the given path buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. Returned path is
always zero terminated except if capacity is zero, in which case an empty string is
returned.
\param tail Tail path and target buffer
\param tail_length Length of tail path
\param tail_capacity Capacity of buffer
\param base Base path to append
\param base_length Length of base path
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_prepend(char* tail, size_t tail_length, size_t tail_capacity, const char* base, size_t base_length);

/*! Prepend given paths at the start of the given path buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. The path argument
list must be pairs of (const char*, size_t) arguments, terminated by a null pointer.
Returned path is always zero terminated except if capacity is zero, in which case an empty
string is returned. Given path fragments are prepended in order, meaning the last argument
will be the first path fragment in the concatenated final path.
\param tail Tail path and target buffer
\param tail_length Length of tail path
\param tail_capacity Capacity of buffer
\param base First base path to append
\param base_length Length of first base path
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_prepend_varg(char* tail, size_t tail_length, size_t tail_capacity, const char* base, size_t base_length, ...)
    FOUNDATION_ATTRIBUTE(sentinel);

/*! Prepend given paths at the start of the given path buffer. If the concatenated path does
not fit into the capacity of the buffer, the path will be truncated. The path argument
list must be pairs of (const char*, size_t) arguments, terminated by a null pointer.
Returned path is always zero terminated except if capacity is zero, in which case an empty
string is returned. Given path fragments are prepended in order, meaning the last argument
will be the first path fragment in the concatenated final path.
\param tail Tail path and target buffer
\param tail_length Length of tail path
\param tail_capacity Capacity of buffer
\param base First base path to append
\param base_length Length of first base path
\param list List of base paths
\return Concatenated path in given memory buffer */
FOUNDATION_API string_t
path_prepend_vlist(char* tail, size_t tail_length, size_t tail_capacity, const char* base, size_t base_length,
                   va_list list);

/*! Make the path in real filesystem absolute by prepending the current working directory
if the path is relative, and cleaning out ".." and "." directories. Returns empty string
if error (invalid path). If the final path does not fit in the buffer capacity, the longest
possible part of the cleaned path will be produced. Returned path is always zero terminated
except if capacity is zero.
\param path Path
\param length Length of path
\param capacity Capacity of path buffer
\return Absolute path, empty string if error */
FOUNDATION_API string_t
path_absolute(char* path, size_t length, size_t capacity);

/*! Make the path in real filesystem absolute by prepending the current working directory
if the path is relative, and cleaning out ".." and "." directories. Returns empty string
if error (invalid path). The final path is stored in a newly allocated buffer large enough
to the cleaned path. Returned path is always zero terminated.
\param path Path
\param length Length of path
\return Absolute path, empty string if error */
FOUNDATION_API string_t
path_allocate_absolute(const char* path, size_t length);

/*! Clean up path in a best effort way by replacing windows backslash with slash and cleaning
up multiple concurrent slashes. Any slash characters at end of path will be removed as well
(except if resulting path is "/"). This method recognizes network protocol separator "://"
and windows drive designations. Any colon characters appearing after the first path separator
slash will be removed. Furthermore, any parts of the path of type "/./" will be condensed
to "/". The operations are in-place, which means that the given string will be modified.
If the final path does not fit in the buffer capacity, the longest possible part of the
cleaned path will be produced. Returned path is always zero terminated except if capacity
is zero,
\param path Path string
\param length Length of path
\param capacity Capacity of path buffer
\return Cleaned path string */
FOUNDATION_API string_t
path_clean(char* path, size_t length, size_t capacity);

/*! Check if path is absolute. An absolute path is either an URI or a file path that starts
with a directory separator or a volume identificator
\param path Path
\param length Length of path
\return true if path is absolute, false if not */
FOUNDATION_API bool
path_is_absolute(const char* path, size_t length);

/*! Create a temporary path, without extension mapping to a suitable filesystem
temporary storage
\param buffer Path buffer
\param capacity Capacity of buffer
\return A unique temporary path stored in given buffer */
FOUNDATION_API string_t
path_make_temporary(char* buffer, size_t capacity);
