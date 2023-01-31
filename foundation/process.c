/* process.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <foundation/foundation.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#elif FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#endif

#if FOUNDATION_PLATFORM_MACOS
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <foundation/apple.h>
#include <sys/event.h>
#endif

static int process_exit_code_pending;

process_t*
process_allocate(void) {
	process_t* proc = memory_allocate(0, sizeof(process_t), 0, MEMORY_PERSISTENT);
	process_initialize(proc);
	return proc;
}

void
process_initialize(process_t* proc) {
	memset(proc, 0, sizeof(process_t));
	proc->flags = PROCESS_ATTACHED;
}

void
process_deallocate(process_t* proc) {
	if (!proc)
		return;
	process_finalize(proc);
	memory_deallocate(proc);
}

void
process_finalize(process_t* proc) {
	if (!(proc->flags & PROCESS_DETACHED))
		process_wait(proc);

	stream_deallocate(proc->pipeout);
	stream_deallocate(proc->pipeerr);
	stream_deallocate(proc->pipein);
	string_deallocate(proc->wd.str);
	string_deallocate(proc->path.str);
	string_array_deallocate(proc->args);
#if FOUNDATION_PLATFORM_WINDOWS
	string_deallocate(proc->verb.str);
#endif
}

void
process_set_working_directory(process_t* proc, const char* path, size_t length) {
	if (proc->wd.length <= length)
		proc->wd =
		    string_resize(proc->wd.str, proc->wd.length, proc->wd.length ? proc->wd.length + 1 : 0, length + 1, 0);
	proc->wd = string_copy(proc->wd.str, length + 1, path, length);
}

void
process_set_executable_path(process_t* proc, const char* path, size_t length) {
	if (proc->path.length <= length)
		proc->path =
		    string_resize(STRING_ARGS(proc->path), proc->path.length ? proc->path.length + 1 : 0, length + 1, 0);
	proc->path = string_copy(proc->path.str, length + 1, path, length);
}

void
process_set_arguments(process_t* proc, const string_const_t* args, size_t count) {
	size_t ia;
	string_array_deallocate(proc->args);
	for (ia = 0; ia < count; ++ia)
		array_push(proc->args, string_clone(args[ia].str, args[ia].length));
}

void
process_set_flags(process_t* proc, unsigned int flags) {
	proc->flags = flags;
}

void
process_set_verb(process_t* proc, const char* verb, size_t length) {
#if FOUNDATION_PLATFORM_WINDOWS
	if (proc->verb.length <= length)
		proc->verb = string_resize(proc->verb.str, proc->verb.length, proc->verb.length ? proc->verb.length + 1 : 0,
		                           length + 1, 0);
	proc->verb = string_copy(proc->verb.str, length + 1, verb, length);
#else
	FOUNDATION_UNUSED(proc);
	FOUNDATION_UNUSED(verb);
	FOUNDATION_UNUSED(length);
#endif
}

int
process_spawn(process_t* proc) {
	static const string_const_t unescaped = {
	    STRING_CONST("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.:/\\")};
	size_t i, arg_count;
#if FOUNDATION_PLATFORM_WINDOWS
	wchar_t* wcmdline;
	wchar_t* wwd;
	string_t cmdline;
#endif
#if !FOUNDATION_PLATFORM_POSIX
	size_t capacity;
#endif

	proc->code = PROCESS_INVALID_ARGS;
	if (!proc->path.length)
		return proc->code;

		// Always escape path on Windows platforms
#if FOUNDATION_PLATFORM_POSIX
	if (string_find_first_not_of(STRING_ARGS(proc->path), STRING_ARGS(unescaped), 0) != STRING_NPOS)
#endif
	{
		bool preesc = (proc->path.str[0] != '"');
		bool postesc = (proc->path.str[proc->path.length - 1] != '"');
		if (preesc || postesc) {
			char* buffer = memory_allocate(HASH_STRING, proc->path.length + 4, 0, MEMORY_PERSISTENT);
			string_t pathesc = string_concat_varg(buffer, proc->path.length + 4, "\"", (size_t)(preesc ? 1 : 0),
			                                      STRING_ARGS(proc->path), "\"", (size_t)(postesc ? 1 : 0), nullptr);
			string_deallocate(proc->path.str);
			proc->path = pathesc;
		}
	}

	arg_count = array_size(proc->args);
	for (i = 0; i < arg_count; ++i) {
		string_t arg = proc->args[i];

		if (!arg.length)
			continue;

#if !FOUNDATION_PLATFORM_POSIX
		if (string_find_first_not_of(arg.str, arg.length, unescaped.str, unescaped.length, 0) != STRING_NPOS) {
			if (arg.str[0] != '"') {
				// Check if we need to escape " characters
				string_t escarg;
				size_t pos = string_find(arg.str, arg.length, '"', 0);
				while (pos != STRING_NPOS) {
					if (arg.str[pos - 1] != '\\') {
						string_const_t right = string_substr(STRING_ARGS(arg), 0, pos);
						string_const_t left = string_substr(STRING_ARGS(arg), pos, STRING_NPOS);
						capacity = arg.length + 2;
						escarg = string_allocate(0, capacity);
						escarg = string_concat(escarg.str, capacity, STRING_ARGS(right), STRING_CONST("\\"));
						escarg = string_append(STRING_ARGS(escarg), capacity, STRING_ARGS(left));
						string_deallocate(arg.str);
						arg = escarg;
					}
					pos = string_find(STRING_ARGS(arg), '"', pos + 2);
				}
				escarg = string_allocate_concat_varg(STRING_CONST("\""), STRING_ARGS(arg), STRING_CONST("\""), nullptr);
				string_deallocate(arg.str);
				proc->args[i] = escarg;
			}
		}
#endif
	}

#if FOUNDATION_PLATFORM_WINDOWS

#ifndef SEE_MASK_NOASYNC
#define SEE_MASK_NOASYNC 0x00000100
#endif

	capacity = BUILD_MAX_PATHLEN;
	cmdline = string_allocate(0, capacity);

	// Don't prepend exe path to parameters if using ShellExecute
	if (!(proc->flags & PROCESS_WINDOWS_USE_SHELLEXECUTE))
		cmdline = string_copy(cmdline.str, capacity, STRING_ARGS(proc->path));

	// Build command line string
	for (i = 0; i < arg_count; ++i) {
		string_t arg = proc->args[i];

		if (!arg.length)
			continue;
		if (cmdline.length + arg.length + 2 >= capacity) {
			string_t newline;
			capacity *= 2;
			newline = string_allocate(0, capacity);
			newline = string_copy(newline.str, capacity, STRING_ARGS(cmdline));
		}
		if (cmdline.length)
			cmdline = string_append(STRING_ARGS(cmdline), capacity, STRING_CONST(" "));
		cmdline = string_append(STRING_ARGS(cmdline), capacity, STRING_ARGS(arg));
	}

	if (!proc->wd.length)
		proc->wd = string_clone(STRING_ARGS(environment_current_working_directory()));

	wcmdline = wstring_allocate_from_string(STRING_ARGS(cmdline));
	wwd = wstring_allocate_from_string(STRING_ARGS(proc->wd));

	if (proc->flags & PROCESS_WINDOWS_USE_SHELLEXECUTE) {
		SHELLEXECUTEINFOW sei;
		wchar_t* wverb;
		wchar_t* wpath;

		wverb = (proc->verb.length ? wstring_allocate_from_string(STRING_ARGS(proc->verb)) : 0);
		wpath = wstring_allocate_from_string(STRING_ARGS(proc->path));

		ZeroMemory(&sei, sizeof(sei));

		sei.cbSize = sizeof(SHELLEXECUTEINFOW);
		sei.hwnd = 0;
		sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = wverb;
		sei.lpFile = wpath;
		sei.lpParameters = wcmdline;
		sei.lpDirectory = wwd;
		sei.nShow = SW_SHOWNORMAL;

		if (!(proc->flags & PROCESS_CONSOLE))
			sei.fMask |= SEE_MASK_NO_CONSOLE;

		if (proc->flags & PROCESS_STDSTREAMS)
			log_warn(0, WARNING_UNSUPPORTED,
			         STRING_CONST("Unable to redirect standard in/out"
			                      " through pipes when using ShellExecute for process spawning"));

		// log_debugf(0, STRING_CONST("Spawn process (ShellExecute): %.*s %.*s"),
		//           STRING_FORMAT(proc->path), STRING_FORMAT(cmdline));

		if (!ShellExecuteExW(&sei)) {
			string_const_t errstr = system_error_message(0);
			log_warnf(0, WARNING_SYSTEM_CALL_FAIL,
			          STRING_CONST("Unable to spawn process (ShellExecute) for executable '%.*s': %.*s"),
			          STRING_FORMAT(proc->path), STRING_FORMAT(errstr));
		} else {
			proc->hp = sei.hProcess;
			proc->ht = 0;
			proc->code = 0;
		}

		wstring_deallocate(wverb);
		wstring_deallocate(wpath);
	} else {
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;
		BOOL inherit_handles = FALSE;

		memset(&si, 0, sizeof(si));
		memset(&pi, 0, sizeof(pi));
		si.cb = sizeof(si);

		if (proc->flags & PROCESS_STDSTREAMS) {
			proc->pipeout = pipe_allocate();
			proc->pipeerr = pipe_allocate();
			proc->pipein = pipe_allocate();

			si.dwFlags |= STARTF_USESTDHANDLES;
			si.hStdOutput = pipe_write_handle(proc->pipeout);
			si.hStdInput = pipe_read_handle(proc->pipein);
			si.hStdError = pipe_write_handle(proc->pipeerr);

			// Don't inherit wrong ends of pipes
			SetHandleInformation(pipe_read_handle(proc->pipeout), HANDLE_FLAG_INHERIT, 0);
			SetHandleInformation(pipe_read_handle(proc->pipeerr), HANDLE_FLAG_INHERIT, 0);
			SetHandleInformation(pipe_write_handle(proc->pipein), HANDLE_FLAG_INHERIT, 0);

			inherit_handles = TRUE;
		}

		// log_debugf(0, STRING_CONST("Spawn process (CreateProcess): %.*s %.*s"),
		//           STRING_FORMAT(proc->path), STRING_FORMAT(cmdline));

		if (!CreateProcessW(0, wcmdline, 0, 0, inherit_handles,
		                    (proc->flags & PROCESS_CONSOLE) ? CREATE_NEW_CONSOLE : 0, 0, wwd, &si, &pi)) {
			string_const_t errstr = system_error_message(0);
			log_warnf(0, WARNING_SYSTEM_CALL_FAIL,
			          STRING_CONST("Unable to spawn process (CreateProcess) for executable '%.*s': %.*s"),
			          STRING_FORMAT(proc->path), STRING_FORMAT(errstr));

			stream_deallocate(proc->pipeout);
			stream_deallocate(proc->pipeerr);
			stream_deallocate(proc->pipein);

			proc->pipeout = 0;
			proc->pipeerr = 0;
			proc->pipein = 0;
		} else {
			proc->hp = pi.hProcess;
			proc->ht = pi.hThread;
			proc->code = 0;
		}

		if (proc->pipeout)
			pipe_close_write(proc->pipeout);
		if (proc->pipeerr)
			pipe_close_write(proc->pipeerr);
		if (proc->pipein)
			pipe_close_read(proc->pipein);
	}

	wstring_deallocate(wcmdline);
	wstring_deallocate(wwd);
	string_deallocate(cmdline.str);

	if (proc->code < 0)
		return proc->code;  // Error

#endif

#if FOUNDATION_PLATFORM_MACOS

	if (proc->flags & PROCESS_MACOS_USE_OPENAPPLICATION) {
		proc->pid = 0;

		FSRef* fsref = memory_allocate(0, sizeof(FSRef), 0, MEMORY_TEMPORARY | MEMORY_ZERO_INITIALIZED);

		string_const_t pathstripped = string_strip(proc->path.str, proc->path.length, STRING_CONST("\""));
		size_t localcap = pathstripped.length + 5;
		string_t localpath = string_allocate(0, localcap);
		// Need it zero terminated for FSPathRef
		localpath = string_copy(localpath.str, localcap, STRING_ARGS(pathstripped));

		OSStatus status = 0;
		status = FSPathMakeRef((uint8_t*)localpath.str, fsref, 0);
		if (status < 0) {
			localpath = string_append(localpath.str, localpath.length, localcap, STRING_CONST(".app"));
			status = FSPathMakeRef((uint8_t*)localpath.str, fsref, 0);
		}

		CFStringRef* args = 0;
		// App gets executable path automatically, don't include
		for (i = 0, arg_count = array_size(proc->args); i < arg_count; ++i)
			array_push(args, CFStringCreateWithBytes(0, (const uint8_t*)proc->args[i].str,
			                                         (CFIndex)proc->args[i].length, kCFStringEncodingUTF8, false));

		CFArrayRef argvref = CFArrayCreate(0, (const void**)args, (CFIndex)array_size(args), 0);

		LSApplicationParameters params;
		ProcessSerialNumber psn;

		memset(&params, 0, sizeof(LSApplicationParameters));
		memset(&psn, 0, sizeof(ProcessSerialNumber));

		params.flags = kLSLaunchDefaults;
		params.application = fsref;
		params.argv = argvref;

		// log_debugf(0, STRING_CONST("Spawn process (LSOpenApplication): %.*s"),
		// STRING_FORMAT(localpath));

		status = LSOpenApplication(&params, &psn);
		if (status != 0) {
			int err = status;
			string_const_t errmsg = system_error_message(err);
			proc->code = status;
			log_errorf(0, ERROR_SYSTEM_CALL_FAIL,
			           STRING_CONST("Unable to spawn process for executable '%.*s': %.*s (%d)"),
			           STRING_FORMAT(localpath), STRING_FORMAT(errmsg), err);
		}

		CFRelease(argvref);
		for (i = 0, arg_count = array_size(args); i < arg_count; ++i)
			CFRelease(args[i]);
		array_deallocate(args);

		memory_deallocate(fsref);
		string_deallocate(localpath.str);

		if (status == 0) {
			pid_t pid = 0;
			GetProcessPID(&psn, &pid);

			proc->pid = pid;

			// Always "detached" with LSOpenApplication, not a child process at all
			// Setup a kqueue to watch when process terminates so we can emulate a wait
			proc->kq = kqueue();
			if (proc->kq < 0) {
				string_const_t errmsg = system_error_message(proc->kq);
				log_errorf(0, ERROR_SYSTEM_CALL_FAIL,
				           STRING_CONST("Unable to create kqueue for process watch: %.*s (%d)"), STRING_FORMAT(errmsg),
				           proc->kq);
				proc->kq = 0;
			} else {
				struct kevent changes;
				EV_SET(&changes, (pid_t)pid, EVFILT_PROC, EV_ADD | EV_RECEIPT, NOTE_EXIT | NOTE_EXITSTATUS, 0, 0);
				int ret = kevent(proc->kq, &changes, 1, &changes, 1, 0);
				if (ret != 1) {
					int err = errno;
					string_const_t errmsg = system_error_message(err);
					log_errorf(0, ERROR_SYSTEM_CALL_FAIL,
					           STRING_CONST("Unable to setup kqueue for process watch, failed to "
					                        "add event to kqueue: %.*s (%d)"),
					           STRING_FORMAT(errmsg), err);
					close(proc->kq);
					proc->kq = 0;
				}
			}
		}

		goto exit;
	} else if (string_ends_with(STRING_ARGS(proc->path), STRING_CONST(".app"))) {
		string_const_t exe_name = path_base_file_name(STRING_ARGS(proc->path));
		string_t modpath = path_allocate_concat_varg(STRING_ARGS(proc->path), STRING_CONST("Contents"),
		                                             STRING_CONST("MacOS"), STRING_ARGS(exe_name), NULL);
		string_deallocate(proc->path.str);
		proc->path = modpath;
	}
#endif

#if FOUNDATION_PLATFORM_POSIX

	// Insert executable arg at start and null ptr at end
	size_t arg;
	size_t argc = array_size(proc->args) + 1;
	array_grow(proc->args, 2);
	for (arg = argc - 1; arg > 0; --arg)
		proc->args[arg] = proc->args[arg - 1];
	proc->args[0] = string_clone(STRING_ARGS(proc->path));
	proc->args[argc] = (string_t){0, 0};

	char* argv_stack[16];
	char** argv = argv_stack;
	if (argc >= (sizeof(argv_stack) / sizeof(argv_stack[0])))
		argv = memory_allocate(0, sizeof(char*) * (argc + 1), 0, MEMORY_PERSISTENT);
	for (arg = 0; arg < argc; ++arg)
		argv[arg] = proc->args[arg].str;
	argv[argc] = 0;

	if (proc->flags & PROCESS_STDSTREAMS) {
		proc->pipeout = pipe_allocate();
		proc->pipeerr = pipe_allocate();
		proc->pipein = pipe_allocate();
	}

	proc->pid = 0;
	pid_t pid = fork();

	if (pid == 0) {
		// Child
		if (proc->wd.length) {
			// log_debugf(0, STRING_CONST("Spawned child process, setting working directory to
			// %.*s"),
			//           STRING_FORMAT(proc->wd));
			environment_set_current_working_directory(STRING_ARGS(proc->wd));
		}

		// log_debugf(0, STRING_CONST("Child process executing: %.*s"), STRING_FORMAT(proc->path));

		if (proc->flags & PROCESS_STDSTREAMS) {
			pipe_close_read(proc->pipeout);
			dup2(pipe_write_fd(proc->pipeout), STDOUT_FILENO);

			pipe_close_read(proc->pipeerr);
			dup2(pipe_write_fd(proc->pipeerr), STDERR_FILENO);

			pipe_close_write(proc->pipein);
			dup2(pipe_read_fd(proc->pipein), STDIN_FILENO);
		}

		int code = execv(proc->path.str, (char* const*)argv);

		// Error
		int err = errno;
		string_const_t errmsg = system_error_message(err);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Child process failed execve() '%.*s': %.*s (%d) (%d)"),
		           STRING_FORMAT(proc->path), STRING_FORMAT(errmsg), err, code);
		if (proc->flags & PROCESS_STDSTREAMS) {
			stream_deallocate(proc->pipeout);
			stream_deallocate(proc->pipeerr);
			stream_deallocate(proc->pipein);
		}
		process_exit(PROCESS_EXIT_FAILURE);
	}

	if (argv != argv_stack)
		memory_deallocate(argv);

	if (pid > 0) {
		// log_debugf(0, STRING_CONST("Child process forked, pid %d"), pid);

		proc->pid = pid;

		if (proc->flags & PROCESS_STDSTREAMS) {
			pipe_close_write(proc->pipeout);
			pipe_close_write(proc->pipeerr);
			pipe_close_read(proc->pipein);
		}

		/*if (proc->flags & PROCESS_DETACHED) {
		    int cstatus = 0;
		    pid_t err = waitpid(pid, &cstatus, WNOHANG);
		    if (err == 0) {
		        //TODO: Ugly wait to make sure process spawned correctly
		        thread_sleep(500);
		        err = waitpid(pid, &cstatus, WNOHANG);
		    }
		    if (err > 0) {
		        //Process exited, check code
		        proc->pid = 0;
		        proc->code = (int)((char)WEXITSTATUS(cstatus));
		        log_debugf(0, STRING_CONST("Child process returned: %d"), proc->code);
		        return proc->code;
		    }
		}*/
	} else {
		// Error
		string_const_t errmsg;
		errmsg = system_error_message(proc->code);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to spawn process '%.*s': %.*s (%d)"),
		           STRING_FORMAT(proc->path), STRING_FORMAT(errmsg), proc->code);

		if (proc->flags & PROCESS_STDSTREAMS) {
			stream_deallocate(proc->pipeout);
			stream_deallocate(proc->pipeerr);
			stream_deallocate(proc->pipein);
		}
		proc->pipeout = 0;
		proc->pipeerr = 0;
		proc->pipein = 0;
		proc->code = PROCESS_SYSTEM_CALL_FAILED;

		return proc->code;
	}

#endif

#if !FOUNDATION_PLATFORM_WINDOWS && !FOUNDATION_PLATFORM_POSIX
	FOUNDATION_ASSERT_FAIL("Process spawning not supported on platform");
#endif

#if FOUNDATION_PLATFORM_MACOS
exit:
#endif

	if (proc->flags & PROCESS_DETACHED)
		return PROCESS_STILL_ACTIVE;

	return process_wait(proc);
}

stream_t*
process_stdout(process_t* proc) {
	return proc ? proc->pipeout : 0;
}

stream_t*
process_stderr(process_t* proc) {
	return proc ? proc->pipeerr : 0;
}

stream_t*
process_stdin(process_t* proc) {
	return proc ? proc->pipein : 0;
}

bool
process_kill(process_t* proc) {
#if FOUNDATION_PLATFORM_WINDOWS
	if (!proc->hp || !TerminateProcess(proc->hp, PROCESS_TERMINATED_SIGNAL))
		return false;
#elif FOUNDATION_PLATFORM_POSIX
	if (!proc->pid || (kill(proc->pid, SIGKILL) < 0))
		return false;
#else
#error Not implemented
#endif

	return true;
}

int
process_wait(process_t* proc) {
#if FOUNDATION_PLATFORM_POSIX
	int cstatus;
	pid_t ret;
#endif

#if FOUNDATION_PLATFORM_WINDOWS

	if (!proc->hp)
		return proc->code;

	while (GetExitCodeProcess(proc->hp, (LPDWORD)&proc->code)) {
		if ((proc->code != (int)STILL_ACTIVE) || (proc->flags & PROCESS_DETACHED))
			break;
		thread_sleep(50);
		proc->code = -1;
	}

	if ((proc->code == (int)STILL_ACTIVE) && (proc->flags & PROCESS_DETACHED))
		return PROCESS_STILL_ACTIVE;

	if (proc->ht)
		CloseHandle(proc->ht);
	if (proc->hp)
		CloseHandle(proc->hp);

	proc->hp = 0;
	proc->ht = 0;

#elif FOUNDATION_PLATFORM_POSIX

	if (!proc->pid)
		return proc->code;

#if FOUNDATION_PLATFORM_MACOS
	if (proc->flags & PROCESS_MACOS_USE_OPENAPPLICATION) {
		if (proc->kq) {
			struct kevent event;
			ret = kevent(proc->kq, 0, 0, &event, 1, 0);
			if (ret != 1) {
				int err = errno;
				string_const_t errmsg = system_error_message(err);
				log_warnf(0, WARNING_SYSTEM_CALL_FAIL,
				          STRING_CONST("Unable to wait on process, failed to read event from kqueue: %.*s (%d)"),
				          STRING_FORMAT(errmsg), err);
			}

			close(proc->kq);
			proc->kq = 0;
			proc->code = (int)event.data;
		} else {
			proc->code = 0;
			log_warn(0, WARNING_INVALID_VALUE,
			         STRING_CONST("Unable to wait on a process started with "
			                      "PROCESS_MACOS_USE_OPENAPPLICATION and no kqueue"));
			return PROCESS_WAIT_FAILED;
		}
		proc->pid = 0;
		return proc->code;
	}
#endif

	cstatus = 0;
	ret = waitpid(proc->pid, &cstatus, (proc->flags & PROCESS_DETACHED) ? WNOHANG : 0);
	if (ret > 0) {
		if (WIFEXITED(cstatus))
			proc->code = (int)((char)WEXITSTATUS(cstatus));
		else if (WIFSIGNALED(cstatus)) {
			proc->code = PROCESS_TERMINATED_SIGNAL;
#ifdef WCOREDUMP
			// if( WCOREDUMP( cstatus ) )
			//...
#endif
			// proc->signal = WTERMSIG( cstatus );
		} else {
			proc->code = PROCESS_WAIT_FAILED;
		}
		proc->pid = 0;
	} else {
		int err = errno;
		if ((ret == 0) && (proc->flags & PROCESS_DETACHED))
			return PROCESS_STILL_ACTIVE;
		if ((ret < 0) && (err == EINTR))
			return PROCESS_WAIT_INTERRUPTED;
		string_const_t errmsg = system_error_message(err);
		log_warnf(0, WARNING_INVALID_VALUE, STRING_CONST("waitpid(%d) failed: %.*s (%d) (returned %d)"), proc->pid,
		          STRING_FORMAT(errmsg), err, ret);
		return PROCESS_WAIT_FAILED;
	}

#else
#error Not implemented
#endif

	return proc->code;
}

int
process_exit_code(void) {
	return process_exit_code_pending;
}

void
process_set_exit_code(int code) {
	process_exit_code_pending = code;
}

void
process_exit(int code) {
	exit(code);
}
