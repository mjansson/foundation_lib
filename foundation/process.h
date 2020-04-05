/* process.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file process.h
\brief Process execution and control

Abstraction of processes, allowing launching of new processes and reading return
codes. All set functions should be called before launching a process, except for set exit
code which should be set before exiting a process. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate a new process object. Deallocate the process object with a call to #process_deallocate.
\return New process object */
FOUNDATION_API process_t*
process_allocate(void);

/*! Deallocate process object previously allocated with a call to #process_allocate.
\param proc Process object */
FOUNDATION_API void
process_deallocate(process_t* proc);

/*! Initialize a new process object. Finalize the process object with a call to #process_finalize.
\param proc Process object */
FOUNDATION_API void
process_initialize(process_t* proc);

/*! Finalize process object previously initialized with a call to #process_initialize.
\param proc Process object */
FOUNDATION_API void
process_finalize(process_t* proc);

/*! Set working directory for process.
\param proc Process object
\param path Working directory
\param length Length of working directory */
FOUNDATION_API void
process_set_working_directory(process_t* proc, const char* path, size_t length);

/*! Set executable path.
\param proc Process object
\param path Path
\param length Length of path */
FOUNDATION_API void
process_set_executable_path(process_t* proc, const char* path, size_t length);

/*! Set arguments passed on the command line to the new process. Should not contain
the standard first argument of the executable path, it will be automatically added.
\param proc Process object
\param args Arguments array
\param count Argument count */
FOUNDATION_API void
process_set_arguments(process_t* proc, const string_const_t* args, size_t count);

/*! Set execution flags, see PROCESS_[*] flag definitions for a list of available flags.
\param proc Process object
\param flags Flags */
FOUNDATION_API void
process_set_flags(process_t* proc, unsigned int flags);

/*! Only used on Windows platforms. Set the execution verb when using ShellExecute
to launch process (i.e if the #PROCESS_WINDOWS_USE_SHELLEXECUTE flag is set).
By default the "runas" verb is used.
\param proc Process object
\param verb Verb to use
\param length Length of verb */
FOUNDATION_API void
process_set_verb(process_t* proc, const char* verb, size_t length);

/*! Set process exit code for current process.
\param code New exit code */
FOUNDATION_API void
process_set_exit_code(int code);

/*! Spawn process. Call #process_wait to reap the child process once processing is
done, to avoid zombie processes.
\param proc Process object
\return Exit code if attached, #PROCESS_STILL_ACTIVE if detached,
        #PROCESS_INVALID_ARGS if error due to invalid arguments */
FOUNDATION_API int
process_spawn(process_t* proc);

/*! Get pipe to read stdout from process (read-only stream). Only available if the
#PROCESS_STDSTREAMS flag was set prior to spawning the process.
\param proc Process object
\return Stdout pipe */
FOUNDATION_API stream_t*
process_stdout(process_t* proc);

/*! Get pipe to read stderr from process (read-only stream). Only available if the
#PROCESS_STDSTREAMS flag was set prior to spawning the process.
\param proc Process object
\return Stdout pipe */
FOUNDATION_API stream_t*
process_stderr(process_t* proc);

/*! Get pipe to write stdin to process (write-only stream). Only available
if the #PROCESS_STDSTREAMS flag was set prior to spawning the process.
\param proc Process object
\return Stdin pipe */
FOUNDATION_API stream_t*
process_stdin(process_t* proc);

/*! Wait for process termination.
\param proc Process object
\return Exit code */
FOUNDATION_API int
process_wait(process_t* proc);

/*! Kill child process
\param proc Process
\return true if process killed, false if not */
FOUNDATION_API bool
process_kill(process_t* proc);

/*! Get current process pending exit code.
\return Pending process exit code */
FOUNDATION_API int
process_exit_code(void);

/*! Terminate current process with given code.
\param code Exit code */
FOUNDATION_API void
process_exit(int code) FOUNDATION_ATTRIBUTE(noreturn);
