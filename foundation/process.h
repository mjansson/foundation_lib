/* process.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file process.h
    Abstraction of process, allowing launching of new processes and reading return codes. */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Allocate a new process object
    \return                           New process object */
FOUNDATION_API process_t*             process_allocate( void );

/*! Deallocate process object
    \param proc                       Process object */
FOUNDATION_API void                   process_deallocate( process_t* proc );

/*! Set working directory
    \param proc                       Process object
    \param path                       Working directory */
FOUNDATION_API void                   process_set_working_directory( process_t* proc, const char* path );

/*! Set executable path
    \param proc                       Process object
    \param path                       Path */
FOUNDATION_API void                   process_set_executable_path( process_t* proc, const char* path );

/*! Set arguments
    \param proc                       Process object
    \param args                       Arguments array
    \param num                        Number of arguments */
FOUNDATION_API void                   process_set_arguments( process_t* proc, const char** args, unsigned int num );

/*! Set flags
    \param proc                       Process object
    \param flags                      Flags */
FOUNDATION_API void                   process_set_flags( process_t* proc, unsigned int flags );

/*! Spawn process
    \param proc                       Process object
    \return                           Exit code if attached, PROCESS_STILL_ACTIVE if detached, PROCESS_INVALID_ARGS if error due to invalid arguments */
FOUNDATION_API int                    process_spawn( process_t* proc );

/*! Get pipe to read stdout from process (read-only stream)
    \return                           Stdout pipe */
FOUNDATION_API stream_t*              process_stdout( process_t* proc );

/*! Get pipe to write stdin to process (write-only stream)
    \return                           Stdin pipe */
FOUNDATION_API stream_t*              process_stdin( process_t* proc );

/*! Wait for termination
    \param proc                       Process object
    \return                           Exit code */
FOUNDATION_API int                    process_wait( process_t* proc );

/*! Set execution verb
    Only used on Windows platforms. Set the execution verb when using ShellExecute to launch process
	(i.e if the WINDOWS_USE_SHELLEXECUTE flag is set). By default the "runas" verb is used.
    \param proc                       Process object
	\param verb                       Verb to use */
FOUNDATION_API void                   process_set_verb( process_t* proc, const char* verb );

/*! Get current process pending exit code
    \return                           Pending process exit code */
FOUNDATION_API int                    process_exit_code( void );

/*! Set process exit code for current process
    \param code                       New exit code */
FOUNDATION_API void                   process_set_exit_code( int code );

/*! Terminate current process with given code
    \param code                       Exit code */
FOUNDATION_API void                   process_exit( int code );
