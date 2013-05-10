/* path.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file path.h
    Path utilities for unifying, merging, cleaning and extracting path parts.

    All function expect the path to be of a wellformed format using forward
    slashes only, such as [protocol://][drive:][/]path/to/some/file[.ext],
    except for path_clean and path_make_absolute

    Use path_clean and path_make_absolute to create well-formed paths from
    unknown formats */

#include <foundation/platform.h>
#include <foundation/types.h>


//! Get base file name
/*! Get base file name (no path and no extension) from full path name
    \param path           Full path
    \return               File base name */
FOUNDATION_API char*      path_base_file_name( const char* path );

//! Get base file name with path
/*! Get base file name with path (no extension) from full path name
    \param path           Full path
    \return               File base name with path */
FOUNDATION_API char*      path_base_file_name_with_path( const char* path );

//! Get file extension
/*! Get file extension (string after last dot) from full path name, not including the dot.
    \param path           Full path
    \return               File extension */
FOUNDATION_API char*      path_file_extension( const char* path );

//! Get file name
/*! Get file name (no path but with extension) from full path name
    \param path           Full path
    \return               File full name */
FOUNDATION_API char*      path_file_name( const char* path );

//! Get path
/*! Get path name from full path name
    \param path           Full path
    \return               Path name */
FOUNDATION_API char*      path_path_name( const char* path );

//! Get subpath
/*! Get subpath name from full path name and root path. If the full path is an absolute path,
    the root path must also be an absolute path (and vice versa). Protocols will be stripped
    before matching paths (which will then be treated as absolute paths).
    \param path           Full path
    \param root           Root full path
    \return               Subpath name within root directory, empty string if not in root directory */
FOUNDATION_API char*      path_subpath_name( const char* path, const char* root );

//! Get network protocol
/*! Get network protocol from full URI (for example, "http://foo.com/some.file" will return "http")
    \param uri            URI
    \return               Protocol */
FOUNDATION_API char*      path_protocol( const char* uri );

//! Merge paths
/*! Merge and cleanup paths, allocating new string for merged path
    \param first          First path
    \param second         Second path
    \return               Path that is merged path, first + second */
FOUNDATION_API char*      path_merge( const char* first, const char* second );

//! Append path
/*! Append and cleanup paths. Modifies and returns new pointer for first string
    \param base           Base path
    \param tail           Path to append
    \return               Merged path */
FOUNDATION_API char*      path_append( char* base, const char* tail );

//! Prepend path
/*! Prepend and cleanup paths. Modifies and returns new pointer for first string
    \param tail           Tail of path
    \param base           Path to prepend
    \return               Merged path */
FOUNDATION_API char*      path_prepend( char* tail, const char* base );

//! Clean path
/*! Clean path by replacing windows backslash with slash and cleaning up multiple concurrent slashes. Any slash characters
    at end of path will be removed as well (except if resulting path is "/"). This method recognizes network protocol separator and will only clean file path part
    of such URI:s. Furthermore, any parts of the path of type "/./" will be condensed to "/". The operations are in-place,
    which means that the given string will be modified.
    \param path           Path string
    \param absolute       Flag indicating absolute path. If true (default) a slash will be inserted at start of path if missing, if false any existing leading slashes will be removed
    \return               Cleaned path string */
FOUNDATION_API char*      path_clean( char* path, bool absolute );

//! Check if path is absolute
/*! Check if path is absolute. An absolute path is either an URI or a file path
    that starts with a directory separator or a volume identificator
    \param path           Path
    \return               true if path is absolute, false if not */
FOUNDATION_API bool       path_is_absolute( const char* path );

//! Make real filesystem path absolute
/*! Make the path in real filesystem absolute by prepending the current working directory
    if the path is relative, and cleaning out ".." and "." directories. Returns empty string
    if error (invalid path).
    \param path           Path
    \return               Absolute path, empty string if error */
FOUNDATION_API char*      path_make_absolute( const char* path );

//! Create a temporary path, without extension
FOUNDATION_API char*      path_make_temporary( void );

