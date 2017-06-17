/* thread.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#if FOUNDATION_PLATFORM_WINDOWS
#  include <foundation/windows.h>
#  include <process.h>

typedef DWORD (WINAPI* GetCurrentProcessorNumberFn)(VOID);
typedef HRESULT (WINAPI* SetThreadDescriptionFn)(HANDLE, PCWSTR);

static DWORD WINAPI
GetCurrentProcessorNumberFallback(VOID) {
	return 0;
}

static GetCurrentProcessorNumberFn _fnGetCurrentProcessorNumber = GetCurrentProcessorNumberFallback;
static SetThreadDescriptionFn _fnSetThreadDescriptionFn;

#endif

#if FOUNDATION_PLATFORM_POSIX
#  if !FOUNDATION_PLATFORM_APPLE && !FOUNDATION_PLATFORM_BSD
#    include <sys/prctl.h>
#  endif
#  include <foundation/posix.h>
#endif

#if FOUNDATION_PLATFORM_PNACL
#  include <foundation/pnacl.h>
#endif

#if FOUNDATION_PLATFORM_ANDROID
#  include <foundation/android.h>
#  include <sys/syscall.h>
#endif

#if FOUNDATION_PLATFORM_BSD
#  include <pthread_np.h>
#endif

FOUNDATION_DECLARE_THREAD_LOCAL(thread_t*, self, 0)
static uint64_t _thread_main_id;

int
_thread_initialize(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	//TODO: look into GetCurrentProcessorNumberEx for 64+ core support
	GetCurrentProcessorNumberFn getprocidfn;
	SetThreadDescriptionFn setthreaddescfn;
	HMODULE kernel32 = GetModuleHandleA("kernel32");
	getprocidfn = (GetCurrentProcessorNumberFn)GetProcAddress(kernel32, "GetCurrentProcessorNumber");
	if (getprocidfn)
		_fnGetCurrentProcessorNumber = getprocidfn;

	setthreaddescfn = (SetThreadDescriptionFn)GetProcAddress(kernel32, "SetThreadDescription");
	if (setthreaddescfn)
		_fnSetThreadDescriptionFn = setthreaddescfn;
#endif

	_thread_main_id = thread_id();

	return 0;
}

void
_thread_finalize(void) {
	_profile_thread_finalize();

	system_thread_finalize();
	random_thread_finalize();

#if FOUNDATION_PLATFORM_ANDROID
	thread_detach_jvm();
#endif

	error_context_thread_finalize();
	memory_context_thread_finalize();
}

static int
_thread_try(void* data) {
	thread_t* thread = data;
	thread->result = thread->fn(thread->arg);
	return 0;
}

#if FOUNDATION_PLATFORM_WINDOWS && !BUILD_DEPLOY && (FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL)

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO {
	/*lint --e{958} */
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void FOUNDATION_NOINLINE
_set_debugger_thread_name(const char* threadname) {
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadname;
	info.dwThreadID = (DWORD)-1;
	info.dwFlags = 0;

#if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	LPTOP_LEVEL_EXCEPTION_FILTER prev_filter;
	prev_filter = SetUnhandledExceptionFilter(_thread_set_name_exception_filter);
#else
	__try
#endif
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
#if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	SetUnhandledExceptionFilter(prev_filter);
#else
	__except (
	    EXCEPTION_CONTINUE_EXECUTION) { //Does EXCEPTION_EXECUTE_HANDLER require a debugger present?
		atomic_thread_fence_release();
	}
#endif
}

#endif

void
thread_set_name(const char* name, size_t length) {
	thread_t* self = get_thread_self();

#if !BUILD_DEPLOY
#  if FOUNDATION_PLATFORM_WINDOWS
#    if (FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL)
	_set_debugger_thread_name(name);
#    endif
	if (_fnSetThreadDescriptionFn) {
		wchar_t wname[64];
		wstring_from_string(wname, sizeof(wname) / sizeof(wname[0]), name, length);
		_fnSetThreadDescriptionFn(GetCurrentThread(), wname);
	}
#  elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	prctl(PR_SET_NAME, name, 0, 0, 0);
#  elif FOUNDATION_PLATFORM_MACOS || FOUNDATION_PLATFORM_IOS
	pthread_setname_np(name);
#  elif FOUNDATION_PLATFORM_BSD
	pthread_set_name_np(pthread_self(), name);
#  endif
#endif

	if (self) {
		string_t newname = string_copy(self->namebuffer, sizeof(self->namebuffer), name, length);
		self->name = string_to_const(newname);
	}
}

#if FOUNDATION_PLATFORM_WINDOWS

typedef unsigned thread_return_t;
typedef void* thread_arg_t;
#define FOUNDATION_THREADCALL STDCALL
#define GET_THREAD_PTR(x) ((thread_t*)(x))

#elif FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL

typedef void* thread_return_t;
typedef void* thread_arg_t;
#if FOUNDATION_PLATFORM_PNACL
#define FOUNDATION_THREADCALL
#else
#define FOUNDATION_THREADCALL FOUNDATION_ATTRIBUTE(noreturn)
#endif
#define GET_THREAD_PTR(x) ((thread_t*)(x))

#else
#  error Not implemented
#endif

static thread_return_t FOUNDATION_THREADCALL
_thread_entry(thread_arg_t data) {
	thread_t* thread = GET_THREAD_PTR(data);
	exception_handler_fn handler = exception_handler();

	thread->osid = thread_id();

	thread_enter();

	//log_debugf(0, STRING_CONST("Starting thread '%.*s' (%" PRIx64 ") %s"),
	//           STRING_FORMAT(thread->name), thread->osid,
	//           handler ? " (exceptions handled)" : "");

	set_thread_self(thread);
	if (thread->name.length)
		thread_set_name(STRING_ARGS(thread->name));

	atomic_store32(&thread->state, 1, memory_order_release);

	if (system_debugger_attached() || !handler) {
		thread->result = thread->fn(thread->arg);
	}
	else {
		string_const_t dump_name = exception_dump_name();
		int wrapped_result = exception_try(_thread_try, thread, handler,
		                                   dump_name.str, dump_name.length);
		if (wrapped_result == FOUNDATION_EXCEPTION_CAUGHT) {
			thread->result = (void*)((uintptr_t)FOUNDATION_EXCEPTION_CAUGHT);
			log_warnf(0, WARNING_SUSPICIOUS,
			          STRING_CONST("Thread '%.*s' (%" PRIx64 ") terminated by exception"),
			          STRING_FORMAT(thread->name), thread->osid);
		}
	}

	//log_debugf(0, STRING_CONST("Exiting thread '%.*s' (%" PRIx64 ")"),
	//           STRING_FORMAT(thread->name), thread->osid);

	if (thread_is_main())
		_thread_main_id = (uint64_t)-1;

	thread->osid = 0;
	atomic_store32(&thread->state, 2, memory_order_release);

	set_thread_self(0);
	thread_exit();

#if FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL
	pthread_exit(0);
#endif
#if !FOUNDATION_PLATFORM_POSIX
	return 0;
#endif
}

thread_t*
thread_allocate(thread_fn fn, void* data, const char* name, size_t length,
                thread_priority_t priority, unsigned int stacksize) {
	thread_t* thread = memory_allocate(0, sizeof(thread_t), 0,
	                                   MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	thread_initialize(thread, fn, data, name, length, priority, stacksize);
	return thread;
}

void
thread_initialize(thread_t* thread, thread_fn fn, void* data, const char* name, size_t length,
                  thread_priority_t priority, unsigned int stacksize) {
	string_t newname;
	if (!stacksize)
		stacksize = (uint32_t)foundation_config().thread_stack_size;

	memset(thread, 0, sizeof(thread_t));
	newname = string_copy(thread->namebuffer, sizeof(thread->namebuffer), name, length);
	thread->arg = data;
	thread->name = string_to_const(newname);
	thread->fn = fn;
	thread->priority = priority;
	thread->stacksize = stacksize;
	beacon_initialize(&thread->beacon);
}

void
thread_deallocate(thread_t* thread) {
	thread_finalize(thread);
	memory_deallocate(thread);
}

void
thread_finalize(thread_t* thread) {
	thread_join(thread);
	beacon_finalize(&thread->beacon);
}

bool
thread_start(thread_t* thread) {
	//Reset beacon
	beacon_try_wait(&thread->beacon, 0);
#if FOUNDATION_PLATFORM_WINDOWS
	FOUNDATION_ASSERT(!thread->handle);
	thread->handle = _beginthreadex(nullptr, thread->stacksize, _thread_entry, thread, 0, nullptr);
	if (!thread->handle) {
		int err = system_error();
		string_const_t errmsg = system_error_message(err);
		log_errorf(0, ERROR_OUT_OF_MEMORY,
		           STRING_CONST("Unable to create thread: CreateThread failed: %.*s (%d)"),
		           STRING_FORMAT(errmsg), err);
		return false;
	}
#elif FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL
	FOUNDATION_ASSERT(!thread->handle);
	pthread_t id = 0;
	int err = pthread_create(&id, 0, _thread_entry, thread);
	if (err) {
		string_const_t errmsg = system_error_message(err);
		log_errorf(0, ERROR_OUT_OF_MEMORY,
		           STRING_CONST("Unable to create thread: pthread_create failed: %.*s (%d)"),
		           STRING_FORMAT(errmsg), err);
		return false;
	}
	thread->handle = (uintptr_t)id;
#else
#  error Not implemented
#endif

	return true;
}

void*
thread_join(thread_t* thread) {
#if FOUNDATION_PLATFORM_WINDOWS
	if (thread->handle) {
		WaitForSingleObject((HANDLE)thread->handle, INFINITE);
		CloseHandle((HANDLE)thread->handle);
		atomic_store32(&thread->state, 3, memory_order_release);
	}
	thread->handle = 0;
#elif FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL
	void* result = 0;
	if (thread->handle) {
		pthread_join((pthread_t)thread->handle, &result);
		atomic_store32(&thread->state, 3, memory_order_release);
	}
	thread->handle = 0;
#else
#  error Not implemented
#endif
	return thread->result;
}

bool
thread_is_started(const thread_t* thread) {
	return thread ? atomic_load32(&thread->state, memory_order_acquire) >= 1 : false;
}

bool
thread_is_running(const thread_t* thread) {
	atomic_thread_fence_acquire();
	return thread ? atomic_load32(&thread->state, memory_order_acquire) == 1 : false;
}

void
thread_signal(thread_t* thread) {
	beacon_fire(&thread->beacon);
}

bool
thread_wait(void) {
	thread_t* thread = get_thread_self();
	return thread ? (beacon_wait(&thread->beacon) >= 0) : false;
}

bool
thread_try_wait(unsigned int milliseconds) {
	thread_t* thread = get_thread_self();
	return thread ? (beacon_try_wait(&thread->beacon, milliseconds) >= 0) : false;
}

string_const_t
thread_name(void) {
	thread_t* thread = get_thread_self();
	return thread ? thread->name : string_null();
}

void
thread_sleep(unsigned int milliseconds) {
#if FOUNDATION_PLATFORM_WINDOWS
	SleepEx(milliseconds, 1);
#elif FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL
	struct timespec ts;
	ts.tv_sec  = milliseconds / 1000;
	ts.tv_nsec = (long)(milliseconds % 1000) * 1000000L;
	nanosleep(&ts, 0);
#else
#  error Not implemented
#endif
}

void
thread_yield(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	Sleep(0);
#elif FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL
	sched_yield();
#else
#  error Not implemented
#endif
}

uint64_t
thread_id(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	return GetCurrentThreadId();
#elif FOUNDATION_PLATFORM_APPLE
	return pthread_mach_thread_np(pthread_self());
#elif FOUNDATION_PLATFORM_BSD
	return (uint64_t)pthread_getthreadid_np() & 0x00000000FFFFFFFFULL;
#elif FOUNDATION_PLATFORM_POSIX
#  if FOUNDATION_SIZE_POINTER == 4
	return (uint64_t)pthread_self() & 0x00000000FFFFFFFFULL;
#  else
	return (uint64_t)pthread_self();
#  endif
#elif FOUNDATION_PLATFORM_PNACL
	void* self = pthread_self();
	return (uintptr_t)self;
#else
#  error Not implemented
#endif
}

unsigned int
thread_hardware(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	return _fnGetCurrentProcessorNumber();
#elif FOUNDATION_PLATFORM_LINUX
	return (unsigned int)sched_getcpu();
#elif FOUNDATION_PLATFORM_ANDROID
	unsigned int cpu = 0;
#  ifdef __NR_getcpu
	if (syscall(__NR_getcpu, &cpu, nullptr, nullptr) < 0)
		return 0;
#  endif
	return cpu;
#else
	//TODO: Implement for other platforms
	return 0;
#endif
}

void
thread_set_hardware(uint64_t mask) {
#if FOUNDATION_PLATFORM_WINDOWS
	DWORD_PTR procmask = 0;
	DWORD_PTR sysmask = 0;
	GetProcessAffinityMask(GetCurrentProcess(), &procmask, &sysmask);
	SetThreadAffinityMask(GetCurrentThread(), (DWORD_PTR)mask & procmask);
#elif FOUNDATION_PLATFORM_LINUX
	uint64_t ibit, bsize;
	cpu_set_t set;
	CPU_ZERO(&set);
	for (ibit = 0, bsize = math_min(64, CPU_SETSIZE); ibit < bsize; ++ibit) {
		if (mask & (1ULL << ibit))
			CPU_SET(ibit, &set);
	}
	if (sched_setaffinity(0, sizeof(set), &set)) {
		string_const_t errmsg = system_error_message(0);
		log_warnf(0, WARNING_SYSTEM_CALL_FAIL,
		          STRING_CONST("Unable to set thread affinity (%" PRIx64 "): %.*s"),
		          mask, STRING_FORMAT(errmsg));
	}
#else
	//TODO: Implement
	FOUNDATION_UNUSED(mask);
#endif
}

void
thread_set_main(void) {
	_thread_main_id = thread_id();
	atomic_thread_fence_release();
}

bool
thread_is_main(void) {
	atomic_thread_fence_acquire();
	return thread_id() == _thread_main_id;
}

thread_t*
thread_self(void) {
	return get_thread_self();
}

void
thread_enter(void) {
	memory_thread_initialize();
}

void
thread_exit(void) {
	_thread_finalize();
	memory_thread_finalize();
}

#if FOUNDATION_PLATFORM_ANDROID

#include <android/native_activity.h>

void*
thread_attach_jvm(void) {
	JavaVMAttachArgs attach_args;
	struct android_app* app = android_app();
	void* env = 0;

	(*app->activity->vm)->GetEnv(app->activity->vm, &env, JNI_VERSION_1_6);
	if (env)
		return env;

	attach_args.version = JNI_VERSION_1_6;
	attach_args.name = "NativeThread";
	attach_args.group = 0;

	// Attaches the current thread to the JVM
	// TODO: According to the native activity, the java env can only be used in the main thread (calling ANativeActivityCallbacks)
	jint result = (*app->activity->vm)->AttachCurrentThread(app->activity->vm,
	                                                        (const struct JNINativeInterface***)&env, &attach_args);
	if (result < 0)
		log_warnf(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to attach thread to Java VM (%d)"),
		          result);

	return env;
}

void
thread_detach_jvm(void) {
	JavaVM* java_vm = android_app()->activity->vm;
	(*java_vm)->DetachCurrentThread(java_vm);
}

#endif
