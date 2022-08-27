/* fs.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file fs.h
\brief File system operations

File system operations acting on the local operating system file system. All paths
are either absolute paths, or relative paths to the current working directory.
Absolute paths can optionally contain a file protocol declarator (file://)

Paths do not have to be zero terminated. Results from substring operations and other
const string segments can be passed to all file functions.

Also provides an interface to monitor local file system changes through
events on platforms supporting this. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Open a file in the file system
\param path   File system path
\param length Length of path
\param mode   Open mode
\return       File stream, 0 if file not found */
FOUNDATION_API stream_t*
fs_open_file(const char* path, size_t length, unsigned int mode);

/*! Copy source file to destination path in the file system, creating directories if needed
\param source        Source file path
\param source_length Length of source file path
\param dest          Destination file path
\param dest_length   Length of destination file path
\return              true if successful, false if failure */
FOUNDATION_API bool
fs_copy_file(const char* source, size_t source_length, const char* dest, size_t dest_length);

/*! Remove a file from the file system
\param path   Path
\param length Length of path
\return       true if file was removed, false if not */
FOUNDATION_API bool
fs_remove_file(const char* path, size_t length);

/*! Move a file in the file system
\param path_source   Source path
\param length_source Length of source path
\param path_dest     Destination path
\param length_dest   Length of destination path
\return              true if file was moved, false if not */
FOUNDATION_API bool
fs_move_file(const char* path_source, size_t length_source, const char* path_dest, size_t length_dest);

/*! Check if the given file exists in the file system
\param path   Path
\param length Length of path
\return       true if the file exists, false if not */
FOUNDATION_API bool
fs_is_file(const char* path, size_t length);

/*! Create path in the file system. Will recursively create directories making up the path.
Success includes case where all paths already exists.
\param path   Path
\param length Length of path
\return       true if success, false if not */
FOUNDATION_API bool
fs_make_directory(const char* path, size_t length);

/*! Remove a directory recursively from the file system
\param path   Path
\param length Length of path
\return       true if directory was removed, false if not */
FOUNDATION_API bool
fs_remove_directory(const char* path, size_t length);

/*! Check if the given directory exists in the file system
\param path   Path
\param length Length of path
\return       true if the directory exists, false if not */
FOUNDATION_API bool
fs_is_directory(const char* path, size_t length);

/*! Get last modification time (last write) in milliseconds since the epoch (UNIX time)
\param path   File path
\param length Length of path
\return       File modification time, 0 if not an existing file */
FOUNDATION_API tick_t
fs_last_modified(const char* path, size_t length);

/*! Touch file and update modification date. Unlike the unix touch command this function
will NOT create the file if it does not exist.
\param path   File path
\param length Length of path */
FOUNDATION_API void
fs_touch(const char* path, size_t length);

/*! Set file mode
\param path   File path
\param length Length of path
\param mode   New file mode */
FOUNDATION_API void
fs_set_mode(const char* path, size_t length, uint mode);

/*! Get file size
\param path   File path
\param length Length of path
\return       Size of file, 0 if not an existing file or unreadable */
FOUNDATION_API size_t
fs_size(const char* path, size_t length);

/*! Get file stats
\param path   File path
\param length Length of path
\return       File stats */
FOUNDATION_API fs_stat_t
fs_stat(const char* path, size_t length);

/*! Get file MD5 digest. Will read and digest the file contents on each call
of this function (slow)
\param path   File path
\param length Length of path
\return       md5 digest, 0 if not an existing file or unreadable */
FOUNDATION_API uint128_t
fs_md5(const char* path, size_t length);

/*! Get files matching the given pattern. The pattern should be a regular
expression supported by the regex parser in the library (see regex.h documentation).
For example, to find all files with a given extension ".ext", use the regex "^.*\\.ext$"
Free the returned array with #string_array_deallocate
\param path           Search path
\param path_length    Length of path
\param pattern        File name pattern
\param pattern_length Length of pattern
\param recurse        Recursion flag
\return               Array of matching file names */
FOUNDATION_API string_t*
fs_matching_files(const char* path, size_t path_length, const char* pattern, size_t pattern_length, bool recurse);

/*! Get files matching the given regex. Free the returned array with #string_array_deallocate
\param path       Search path
\param length     Length of path
\param regex      Regex
\param recurse    Recursion flag
\return           Array of matching file names */
FOUNDATION_API string_t*
fs_matching_files_regex(const char* path, size_t length, regex_t* regex, bool recurse);

/*! Get files in the given directory path. Free the returned array with #string_array_deallocate
\param path   Path of directory
\param length Length of path
\return       Array of file names */
FOUNDATION_API string_t*
fs_files(const char* path, size_t length);

/*! Get subdirs matching the given pattern. The pattern should be a regular
expression supported by the regex parser in the library (see regex.h documentation).
Free the returned array with #string_array_deallocate
\param path       Search path
\param length     Length of path
\param pattern    Subdir name pattern
\param patternlen Length of pattern
\param recurse    Recursion flag
\return           Array of matching file names */
FOUNDATION_API string_t*
fs_matching_subdirs(const char* path, size_t length, const char* pattern, size_t patternlen, bool recurse);

/*! Get subdirs matching the given regex. Free the returned array with #string_array_deallocate
\param path       Search path
\param length     Length of path
\param regex      Regex
\param recurse    Recursion flag
\return           Array of matching file names */
FOUNDATION_API string_t*
fs_matching_subdirs_regex(const char* path, size_t length, regex_t* regex, bool recurse);

/*! Get subdirectories in the given directory path. Free the returned array with
#string_array_deallocate
\param path   Path of directory
\param length Length of path
\return       Array of subdirectory names */
FOUNDATION_API string_t*
fs_subdirs(const char* path, size_t length);

/*! Monitor the path (recursive) for file system changes. Changes are notified as file system
events in the event stream returned by #fs_event_stream
\param path   File system path
\param length Length of path
\return true if successful, false if not */
FOUNDATION_API bool
fs_monitor(const char* path, size_t length);

/*! Stop monitoring the path (recursive) for file system changes
\param path   File system path
\param length Length of path */
FOUNDATION_API void
fs_unmonitor(const char* path, size_t length);

/*! Create a temporary file. The file will be located in the temporary directory given
by #environment_temporary_directory and normally does not persist between executions
of the same program.
\return Temporary file */
FOUNDATION_API stream_t*
fs_temporary_file(void);

/*! Post a file event
\param id     Event id
\param path   Path
\param length Length of path */
FOUNDATION_API void
fs_event_post(foundation_event_id id, const char* path, size_t length);

/*! Get path from a file event
\param event Event
\return Path */
FOUNDATION_API string_const_t
fs_event_path(const event_t* event);

/*! Get file system event stream
\return File system event stream */
FOUNDATION_API event_stream_t*
fs_event_stream(void);
