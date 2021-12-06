/* exception.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#include <foundation/internal.h>

#if FOUNDATION_PLATFORM_WINDOWS && (FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL || \
                                    (FOUNDATION_COMPILER_CLANG && FOUNDATION_CLANG_VERSION >= 30900))
#define FOUNDATION_USE_SEH 1
#else
#define FOUNDATION_USE_SEH 0
#endif

#if FOUNDATION_COMPILER_CLANG
#if __has_warning("-Wdisabled-macro-expansion")
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif
#endif

static exception_handler_fn exception_handler_user;
static string_const_t exception_dump_name_user;

void
exception_set_handler(exception_handler_fn handler, const char* name, size_t length) {
	exception_handler_user = handler;
	exception_dump_name_user = (string_const_t){name, length};
}

string_const_t
exception_dump_name(void) {
	return exception_dump_name_user;
}

exception_handler_fn
exception_handler(void) {
	return exception_handler_user;
}

#if FOUNDATION_PLATFORM_WINDOWS

#include <foundation/windows.h>

typedef BOOL(STDCALL* MiniDumpWriteDumpFn)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, CONST PMINIDUMP_EXCEPTION_INFORMATION,
                                           CONST PMINIDUMP_USER_STREAM_INFORMATION,
                                           CONST PMINIDUMP_CALLBACK_INFORMATION);

static void
create_mini_dump(EXCEPTION_POINTERS* pointers, const char* name, size_t namelen, char* dump_file, size_t* capacity) {
	MINIDUMP_EXCEPTION_INFORMATION info;
	HANDLE file;
	SYSTEMTIME local_time;
	string_const_t temp_dir;
	string_const_t uuid;
	string_t filename;

	GetLocalTime(&local_time);

	if (!namelen) {
		string_const_t short_name = environment_application()->short_name;
		name = short_name.str;
		namelen = short_name.length;
	}
	temp_dir = environment_temporary_directory();
	uuid = string_from_uuid_static(environment_application()->instance);
	filename = string_format(dump_file, *capacity, STRING_CONST("%.*s/%s%s%.*s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
	                         STRING_FORMAT(temp_dir), namelen ? name : "", namelen ? "-" : "", STRING_FORMAT(uuid),
	                         local_time.wYear, local_time.wMonth, local_time.wDay, local_time.wHour, local_time.wMinute,
	                         local_time.wSecond, GetCurrentProcessId(), GetCurrentThreadId());
	if (!fs_is_directory(STRING_ARGS(temp_dir)))
		fs_make_directory(STRING_ARGS(temp_dir));
	file = CreateFileA(filename.str, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	if (file && (file != INVALID_HANDLE_VALUE)) {
		BOOL success = FALSE;
		HMODULE lib = LoadLibraryA("dbghelp.dll");

		if (lib) {
			MiniDumpWriteDumpFn write = (MiniDumpWriteDumpFn)GetProcAddress(lib, "MiniDumpWriteDump");
			if (write) {
				info.ThreadId = GetCurrentThreadId();
				info.ExceptionPointers = pointers;
				info.ClientPointers = FALSE;

				success = write(GetCurrentProcess(), GetCurrentProcessId(), file,
				                MiniDumpWithThreadInfo,  //(MINIDUMP_TYPE)(MiniDumpWithDataSegs |
				                                         // MiniDumpWithProcessThreadData |
				                                         // MiniDumpWithThreadInfo),
				                &info, 0, 0);
			} else {
				string_const_t errmsg = system_error_message(0);
				log_errorf(0, ERROR_EXCEPTION,
				           STRING_CONST("Exception occurred! Unable open get symbol from dbghelp library: %.*s"),
				           STRING_FORMAT(errmsg));
			}

			FreeLibrary(lib);
		} else {
			string_const_t errmsg = system_error_message(0);
			log_errorf(0, ERROR_EXCEPTION, STRING_CONST("Exception occurred! Unable open dbghelp library: %.*s"),
			           STRING_FORMAT(errmsg));
		}
		if (success) {
			log_errorf(0, ERROR_EXCEPTION, STRING_CONST("Exception occurred! Minidump written to: %.*s"),
			           STRING_FORMAT(filename));
			FlushFileBuffers(file);
		}
		CloseHandle(file);
		if (success) {
			*capacity = filename.length;
			return;
		}
	} else {
		log_errorf(0, ERROR_EXCEPTION, STRING_CONST("Exception occurred! Unable to write mini dump to: %.*s"),
		           STRING_FORMAT(filename));
	}

	// TODO: At least print out the exception records in log
	*capacity = 0;
}

#if !FOUNDATION_USE_SEH

#include <setjmp.h>

typedef VOID(WINAPI* RtlRestoreContextFn)(PCONTEXT, PEXCEPTION_RECORD);
typedef VOID(WINAPI* RtlCaptureContextFn)(PCONTEXT);

static RtlRestoreContextFn _RtlRestoreContext;
static RtlCaptureContextFn _RtlCaptureContext;

struct exception_closure_t {
	exception_handler_fn handler;
	string_const_t name;
	bool initialized;
	bool triggered;
	CONTEXT context;
	jmp_buf jmpbuf;
};

typedef struct exception_closure_t exception_closure_t;

static FOUNDATION_THREADLOCAL exception_closure_t exception_closure;

static LONG WINAPI
exception_filter(LPEXCEPTION_POINTERS pointers) {
	if (exception_closure.initialized) {
		char dump_file_buffer[MAX_PATH];
		size_t dump_file_len = sizeof(dump_file_buffer);
		exception_closure.triggered = true;
		create_mini_dump(pointers, STRING_ARGS(exception_closure.name), dump_file_buffer, &dump_file_len);
		if (exception_closure.handler)
			exception_closure.handler(dump_file_buffer, dump_file_len);
		if (_RtlRestoreContext)
			_RtlRestoreContext(&exception_closure.context, 0);
		else
			longjmp(exception_closure.jmpbuf, 0);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif

#endif

#if FOUNDATION_PLATFORM_POSIX  //&& !FOUNDATION_PLATFORM_APPLE

#include <foundation/posix.h>
//#include <ucontext.h>

FOUNDATION_DECLARE_THREAD_LOCAL(exception_handler_fn, exception_handler, 0)
FOUNDATION_DECLARE_THREAD_LOCAL(const char*, dump_name, 0)

#if FOUNDATION_PLATFORM_ANDROID
#define exception_env_t long int*
#elif FOUNDATION_PLATFORM_APPLE
#define exception_env_t int*
#elif FOUNDATION_PLATFORM_BSD
#define exception_env_t struct _sigjmp_buf*
#else
#define exception_env_t struct __jmp_buf_tag*
#endif
FOUNDATION_DECLARE_THREAD_LOCAL(exception_env_t, exception_env, 0)

static string_t
create_mini_dump(void* context, string_const_t name, string_t dump_file) {
	string_const_t tmp_dir;
	string_const_t uuid_str;
	if (!name.length)
		name = environment_application()->short_name;
	tmp_dir = environment_temporary_directory();
	uuid_str = string_from_uuid_static(environment_application()->instance);
	dump_file = string_format(dump_file.str, dump_file.length, STRING_CONST("%.*s/%.*s%s%.*s-%" PRIx64 ".dmp"),
	                          STRING_FORMAT(tmp_dir), STRING_FORMAT(name), name.length ? "-" : "",
	                          STRING_FORMAT(uuid_str), time_system());
	fs_make_directory(tmp_dir.str, tmp_dir.length);

	// TODO: Write dump file
	// ucontext_t* user_context = context;
	FOUNDATION_UNUSED(context);

	return dump_file;
}

static void
FOUNDATION_ATTRIBUTE(noreturn) exception_sigaction(int sig, siginfo_t* info, void* arg) {
	FOUNDATION_UNUSED(sig);
	FOUNDATION_UNUSED(info);
	FOUNDATION_UNUSED(arg);

	log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Caught signal: %d"), sig);

	char file_name_buffer[BUILD_MAX_PATHLEN];
	const char* name = get_thread_dump_name();
	string_t dump_file = (string_t){file_name_buffer, sizeof(file_name_buffer)};
	dump_file = create_mini_dump(arg, (string_const_t){name, string_length(name)}, dump_file);
	exception_handler_fn handler = get_thread_exception_handler();
	if (handler)
		handler(dump_file.str, dump_file.length);

	error_context_clear();

	exception_env_t exception_env = get_thread_exception_env();
	if (exception_env)
		siglongjmp(exception_env, FOUNDATION_EXCEPTION_CAUGHT);

	// sigaction(sig, 0, 0);
	signal(sig, SIG_DFL);
	raise(sig);
	_exit(-1);
}

#endif

int
exception_try(exception_try_fn fn, void* data, exception_handler_fn handler, const char* name, size_t length) {
#if FOUNDATION_PLATFORM_WINDOWS
	int ret;
#if FOUNDATION_USE_SEH
	char buffer[MAX_PATH];
	size_t capacity = sizeof(buffer);
#endif
#endif

	// Make sure path is initialized
	environment_temporary_directory();

#if FOUNDATION_PLATFORM_WINDOWS

#if FOUNDATION_USE_SEH
#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif
	__try {
		ret = fn(data);
	} /*lint -e534*/
	__except (create_mini_dump(GetExceptionInformation(), name, length, buffer, &capacity), EXCEPTION_EXECUTE_HANDLER) {
		ret = FOUNDATION_EXCEPTION_CAUGHT;
		if (handler)
			handler(buffer, capacity);

		error_context_clear();
	}
#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
	return ret;
#else
	exception_closure.handler = handler;
	exception_closure.name = string_const(name, length);
	exception_closure.triggered = false;
	exception_closure.initialized = true;
	if (_RtlCaptureContext)
		_RtlCaptureContext(&exception_closure.context);
	else
		setjmp(exception_closure.jmpbuf);
	atomic_thread_fence_release();
	if (exception_closure.triggered) {
		ret = FOUNDATION_EXCEPTION_CAUGHT;
		error_context_clear();
	} else {
		ret = fn(data);
	}
	return ret;
#endif

#elif FOUNDATION_PLATFORM_POSIX
	sigjmp_buf exception_env;

	if (!handler)
		return fn(data);

	set_thread_exception_handler(handler);
	set_thread_dump_name(length ? name : 0);

	memset(&exception_env, 0, sizeof(exception_env));
	int ret = sigsetjmp(exception_env, 1);
	if (ret == 0) {
		set_thread_exception_env(exception_env);
		return fn(data);
	}
	return ret;

#else

	FOUNDATION_UNUSED(handler);
	FOUNDATION_UNUSED(name);
	FOUNDATION_UNUSED(length);

	// No guard mechanism in place yet for this platform
	return fn(data);

#endif
}

#if FOUNDATION_PLATFORM_WINDOWS && !FOUNDATION_USE_SEH
static HMODULE kernel_lib;
#endif

int
internal_exception_initialize(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	SetErrorMode(SEM_FAILCRITICALERRORS);
#if !FOUNDATION_USE_SEH
	SetUnhandledExceptionFilter(exception_filter);
	kernel_lib = LoadLibraryA("kernel32.dll");
	if (kernel_lib) {
		_RtlCaptureContext = (RtlCaptureContextFn)GetProcAddress(kernel_lib, "RtlCaptureContext");
		_RtlRestoreContext = (RtlRestoreContextFn)GetProcAddress(kernel_lib, "RtlRestoreContext");
	}
	if (!_RtlCaptureContext || !_RtlRestoreContext) {
		_RtlCaptureContext = 0;
		_RtlRestoreContext = 0;
	}
#endif
#elif FOUNDATION_PLATFORM_POSIX
	struct sigaction action;
	memset(&action, 0, sizeof(action));

	// Signals we process globally
	action.sa_sigaction = exception_sigaction;
	action.sa_flags = SA_SIGINFO;
	if ((sigaction(SIGTRAP, &action, 0) < 0) || (sigaction(SIGABRT, &action, 0) < 0) ||
	    (sigaction(SIGFPE, &action, 0) < 0) || (sigaction(SIGSEGV, &action, 0) < 0) ||
	    (sigaction(SIGBUS, &action, 0) < 0) || (sigaction(SIGILL, &action, 0) < 0) ||
	    (sigaction(SIGSYS, &action, 0) < 0)) {
		log_warn(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to set signal actions"));
	}

#endif

	return 0;
}

void
internal_exception_finalize(void) {
#if FOUNDATION_PLATFORM_WINDOWS && !FOUNDATION_USE_SEH
	_RtlCaptureContext = 0;
	_RtlRestoreContext = 0;
	if (kernel_lib)
		FreeLibrary(kernel_lib);
#endif
}

void
exception_raise_debug_break(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	DebugBreak();
#else
	__builtin_trap();
#endif
}

void
exception_raise_abort(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	DebugBreak();
	process_exit(-1);
#else
	__builtin_trap();
#endif
}
