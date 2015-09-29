/* crash.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <foundation/internal.h>

static crash_dump_callback_fn  _crash_dump_callback;
static string_const_t          _crash_dump_name;

void
crash_guard_set(crash_dump_callback_fn callback, const char* name, size_t length) {
	_crash_dump_callback = callback;
	_crash_dump_name     = (string_const_t) { name, length };
}

string_const_t
crash_guard_name(void) {
	return _crash_dump_name;
}

crash_dump_callback_fn
crash_guard_callback(void) {
	return _crash_dump_callback;
}

#if FOUNDATION_PLATFORM_WINDOWS

#  include <foundation/windows.h>
#  include <stdio.h>
#  include <stdarg.h>

typedef BOOL (STDCALL* MiniDumpWriteDumpFn)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
                                            CONST PMINIDUMP_EXCEPTION_INFORMATION, CONST PMINIDUMP_USER_STREAM_INFORMATION,
                                            CONST PMINIDUMP_CALLBACK_INFORMATION);

static string_t
_crash_create_mini_dump(EXCEPTION_POINTERS* pointers, string_const_t name, string_t dump_file) {
	MINIDUMP_EXCEPTION_INFORMATION info;
	HANDLE file;
	SYSTEMTIME local_time;
	string_const_t temp_dir;
	string_const_t uuid;

	GetLocalTime(&local_time);

	if (!name.length)
		name = environment_application()->short_name;
	temp_dir = environment_temporary_directory();
	uuid = string_from_uuid_static(environment_application()->instance);
	dump_file = string_format(STRING_ARGS(dump_file),
	                          STRING_CONST("%.*s/%s%s%.*s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
	                          STRING_FORMAT(temp_dir), name.length ? name.str : "", name.length ? "-" : "",
	                          STRING_FORMAT(uuid),
	                          local_time.wYear, local_time.wMonth, local_time.wDay,
	                          local_time.wHour, local_time.wMinute, local_time.wSecond,
	                          GetCurrentProcessId(), GetCurrentThreadId());
	if (!fs_is_directory(STRING_ARGS(temp_dir)))
		fs_make_directory(STRING_ARGS(temp_dir));
	file = CreateFileA(dump_file.str, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0,
	                   CREATE_ALWAYS, 0, 0);

	if (file && (file != INVALID_HANDLE_VALUE)) {
		BOOL success = FALSE;
		HMODULE lib = LoadLibraryA("dbghelp.dll");

		if (lib) {
			MiniDumpWriteDumpFn write = (MiniDumpWriteDumpFn)GetProcAddress(lib, "MiniDumpWriteDump");
			if (write) {
				info.ThreadId          = GetCurrentThreadId();
				info.ExceptionPointers = pointers;
				info.ClientPointers    = TRUE;

				success = write(GetCurrentProcess(), GetCurrentProcessId(), file,
				                MiniDumpWithDataSegs | MiniDumpWithProcessThreadData | MiniDumpWithThreadInfo, &info, 0, 0);
				if (!success)
					dump_file.length = 0;
			}

			FreeLibrary(lib);
		}
		if (success)
			FlushFileBuffers(file);
		CloseHandle(file);

		return dump_file;
	}

	return (string_t){0, 0};
}

#  if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG

struct crash_exception_closure_t {
	crash_dump_callback_fn      callback;
	string_const_t              name;
};

typedef struct crash_exception_closure_t crash_exception_closure_t;

//TODO: Set per-thread
crash_exception_closure_t _crash_exception_closure;

LONG WINAPI
_crash_exception_filter(LPEXCEPTION_POINTERS pointers) {
	string_t dump_file = { _crash_dump_file_buffer, sizeof(_crash_dump_file_buffer) };
	_crash_create_mini_dump(pointers, _crash_exception_closure.name, dump_file);
	if (_crash_exception_closure.callback)
		_crash_exception_closure.callback(_crash_dump_file);
	else
		log_errorf(0, ERROR_EXCEPTION, "Exception occurred! Minidump written to: %ls", _crash_dump_file);
	return EXCEPTION_EXECUTE_HANDLER;
}

#  endif

#endif

#if FOUNDATION_PLATFORM_POSIX //&& !FOUNDATION_PLATFORM_APPLE

#include <foundation/posix.h>
//#include <ucontext.h>

FOUNDATION_DECLARE_THREAD_LOCAL(crash_dump_callback_fn, crash_callback, 0)
FOUNDATION_DECLARE_THREAD_LOCAL(const char*, crash_callback_name, 0)

#if FOUNDATION_PLATFORM_ANDROID
#  define crash_env_t long int*
#elif FOUNDATION_PLATFORM_APPLE
#  define crash_env_t int*
#elif FOUNDATION_PLATFORM_BSD
#  define crash_env_t struct _sigjmp_buf*
#else
#  define crash_env_t struct __jmp_buf_tag*
#endif
FOUNDATION_DECLARE_THREAD_LOCAL(crash_env_t, crash_env, 0)

static string_t
_crash_guard_minidump(void* context, string_const_t name, string_t dump_file) {
	string_const_t tmp_dir;
	string_const_t uuid_str;
	if (!name.length)
		name = environment_application()->short_name;
	tmp_dir = environment_temporary_directory();
	uuid_str = string_from_uuid_static(environment_application()->instance);
	dump_file = string_format(dump_file.str, dump_file.length,
	                          STRING_CONST("%.*s/%.*s%s%.*s-%" PRIx64 ".dmp"),
	                          STRING_FORMAT(tmp_dir), STRING_FORMAT(name), name.length ? "-" : "",
	                          STRING_FORMAT(uuid_str), time_system());
	fs_make_directory(tmp_dir.str, tmp_dir.length);

	//TODO: Write dump file
	//ucontext_t* user_context = context;
	FOUNDATION_UNUSED(context);

	return dump_file;
}

static void
_crash_guard_sigaction(int sig, siginfo_t* info, void* arg) {
	FOUNDATION_UNUSED(sig);
	FOUNDATION_UNUSED(info);
	FOUNDATION_UNUSED(arg);

	log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Caught crash guard signal: %d"), sig);

	crash_dump_callback_fn callback = get_thread_crash_callback();
	if (callback) {
		char file_name_buffer[ BUILD_MAX_PATHLEN ];
		const char* name = get_thread_crash_callback_name();
		string_t dump_file = (string_t) {file_name_buffer, sizeof(file_name_buffer)};
		dump_file = _crash_guard_minidump(arg, (string_const_t) {name, string_length(name)}, dump_file);
		callback(dump_file.str, dump_file.length);
	}

	error_context_clear();

	crash_env_t guard_env = get_thread_crash_env();
	if (guard_env)
		siglongjmp(guard_env, FOUNDATION_CRASH_DUMP_GENERATED);
	else
		log_warn(0, WARNING_SUSPICIOUS, STRING_CONST("No sigjmp_buf for thread"));
}

#endif

int
crash_guard(crash_guard_fn fn, void* data, crash_dump_callback_fn callback, const char* name,
            size_t length) {
#if FOUNDATION_PLATFORM_WINDOWS && (FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL)
	int ret = FOUNDATION_CRASH_DUMP_GENERATED;
	string_t crash_dump_file = string_allocate(0, BUILD_MAX_PATHLEN);
#endif

	//Make sure path is initialized
	environment_temporary_directory();

#if FOUNDATION_PLATFORM_WINDOWS

#  if FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL// || FOUNDATION_COMPILER_CLANG
	__try {
		ret = fn(data);
	}
	__except (_crash_create_mini_dump(GetExceptionInformation(), string_const(name, length), crash_dump_file),
	          EXCEPTION_EXECUTE_HANDLER) {
		if (callback)
			callback(STRING_ARGS(crash_dump_file));

		error_context_clear();
	}
	string_deallocate(crash_dump_file.str);
	return ret;
#  else
	SetUnhandledExceptionFilter(_crash_exception_filter);
	_crash_exception_closure.callback = callback;
	_crash_exception_closure.name = name;
	return fn(data);
#  endif

#elif FOUNDATION_PLATFORM_POSIX
	sigjmp_buf guard_env;

	struct sigaction action;
	memset(&action, 0, sizeof(action));

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif

	//Signals we process globally
	action.sa_sigaction = _crash_guard_sigaction;
	action.sa_flags = SA_SIGINFO;
	if ((sigaction(SIGTRAP, &action, 0) < 0) ||
	    (sigaction(SIGABRT, &action, 0) < 0) ||
	    (sigaction(SIGFPE,  &action, 0) < 0) ||
	    (sigaction(SIGSEGV, &action, 0) < 0) ||
	    (sigaction(SIGBUS,  &action, 0) < 0) ||
	    (sigaction(SIGILL,  &action, 0) < 0) ||
	    (sigaction(SIGSYS,  &action, 0) < 0)) {
		log_warn(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to set crash guard signal actions"));
	}

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic pop
#endif

	set_thread_crash_callback(callback);
	set_thread_crash_callback_name(length ? name : 0);

	memset(&guard_env, 0, sizeof(guard_env));
	int ret = sigsetjmp(guard_env, 1);
	if (ret == 0) {
		set_thread_crash_env(guard_env);
		return fn(data);
	}
	return ret;

#else

	FOUNDATION_UNUSED(callback);
	FOUNDATION_UNUSED(name);
	FOUNDATION_UNUSED(length);

	//No guard mechanism in place yet for this platform
	return fn(data);

#endif
}

#if FOUNDATION_PLATFORM_WINDOWS
extern __declspec(dllimport) void STDCALL DebugBreak(void);
#endif

void
crash_debug_break(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	DebugBreak();
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	__builtin_trap();
#else
	(*(volatile int*)3 = 0);
#endif
}

void
crash_dump(void) {
#if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	__builtin_trap();
#else
	(*(volatile int*)3 = 0);
#endif
}
