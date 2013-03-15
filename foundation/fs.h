/* fs.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file fs.h
    File system operations. All paths are either absolute paths in the
    OS file system, or relative paths to the current working directory */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Open a file
    \param path                 File system path
    \param mode                 Open mode
    \return                     File stream, 0 if file not found */
FOUNDATION_API stream_t*        fs_open_file( const char* path, unsigned int mode );
	
/*! Utility method for real file system, copy source file to destination path, creating directories if needed
    \param source               Source file path
    \param dest                 Destination file path */
FOUNDATION_API void             fs_copy_file( const char* source, const char* dest );

/*! Remove a file from the real file system
    \param path                 Path
    \return                     true if file was removed, false if not */
FOUNDATION_API bool             fs_remove_file( const char* path );

/*! Check if the given file exists in the real file system
    \param path                 Path
    \return                     true if the file exists, false if not */
FOUNDATION_API bool             fs_is_file( const char* path );


/*! Create path in the real file system. Can recursively create directories
    making up the path
    \param path                 Path in real file system to create
    \return                     true if success, false if not */
FOUNDATION_API bool             fs_make_directory( const char* path );

/*! Remove a directory (recursive) from the real file system
    \param path                 Path
    \return                     true if directory was removed, false if not */
FOUNDATION_API bool             fs_remove_directory( const char* path );

/*! Check if the given directory exists in the real file system
    \param path                 Path
    \return                     true if the directory exists, false if not */
FOUNDATION_API bool             fs_is_directory( const char* path );


/*! Get last modification date (last write) in milliseconds since the epoch (UNIX time)
    \param path                 File path
    \return                     File modification date, 0 if not an existing file */
FOUNDATION_API uint64_t         fs_last_modified( const char* path );

/*! Touch file and update modification date
    \param path                 File path */
FOUNDATION_API void             fs_touch( const char* path );

/*! Get file md5 digest
    \param path                 File path
    \return                     md5 digest, 0 if not an existing file or unreadable */
FOUNDATION_API uint128_t        fs_md5( const char* path );


/*! Get files matching the given pattern. Free the returned
    array with string_array_deallocate()
    \param path                 File path
    \param pattern              File name pattern (*.ext for extension search)
    \param recurse              Recursion flag
    \return                     Array of matching file names */
FOUNDATION_API char**           fs_matching_files( const char* path, const char* pattern, bool recurse );

/*! Get files in the given directory path. Free the returned
    array with string_array_deallocate()
    \return                     Array of file names */
FOUNDATION_API char**           fs_files( const char* path );

/*! Get subdirectories in the given directory path. Free the returned
    array with string_array_deallocate()
    \return                     Array of subdirectory names */
FOUNDATION_API char**           fs_subdirs( const char* path );


/*! Monitor the path (recursive) for file system changes
    \param path                 File system path */
FOUNDATION_API void             fs_monitor( const char* path );

/*! Stop monitoring the path (recursive) for file system changes
    \param path                 File system path */
FOUNDATION_API void             fs_unmonitor( const char* path );


/*! Post a file event
    \param id                   Event id
    \param path                 Path
    \param pathlen              Length of path */
FOUNDATION_API void             fs_post_event( foundation_event_id id, const char* path, unsigned int pathlen );

/*! Get file system event stream
    \return                     File system event stream */
FOUNDATION_API event_stream_t*  fs_event_stream( void );

