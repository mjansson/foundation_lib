/* stacktrace.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#  include <psapi.h>
#elif FOUNDATION_PLATFORM_POSIX
#  include <foundation/posix.h>
#  if !FOUNDATION_PLATFORM_ANDROID
#    include <execinfo.h>
#  endif
#endif

#if FOUNDATION_PLATFORM_ANDROID
#  include <unwind.h>
#endif

#if FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
extern void FOUNDATION_NOINLINE _gcc_barrier_function(uint32_t fp);
void __attribute__((optimize("O0"))) _gcc_barrier_function(uint32_t fp) { FOUNDATION_UNUSED(fp); }
#endif

#if FOUNDATION_PLATFORM_WINDOWS

#  if FOUNDATION_COMPILER_MSVC
#    pragma optimize( "", off )
#  elif FOUNDATION_COMPILER_CLANG
#    undef WINAPI
#    define WINAPI STDCALL
#  endif

typedef BOOL (WINAPI* EnumProcessesFn)(DWORD* lpidProcess, DWORD cb, DWORD* cbNeeded);
typedef BOOL (WINAPI* EnumProcessModulesFn)(HANDLE hProcess, HMODULE* lphModule, DWORD cb,
                                            LPDWORD lpcbNeeded);
typedef DWORD (WINAPI* GetModuleBaseNameFn)(HANDLE hProcess, HMODULE hModule, LPSTR lpBaseName,
                                            DWORD nSize);
typedef DWORD (WINAPI* GetModuleFileNameExFn)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename,
                                              DWORD nSize);
typedef BOOL (WINAPI* GetModuleInformationFn)(HANDLE hProcess, HMODULE hModule,
                                              LPMODULEINFO lpmodinfo, DWORD cb);

typedef BOOL (WINAPI* SymInitializeFn)(HANDLE hProcess, PCSTR UserSearchPath, BOOL fInvadeProcess);
typedef DWORD (WINAPI* SymSetOptionsFn)(DWORD SymOptions);
typedef DWORD (WINAPI* SymGetOptionsFn)(VOID);
typedef DWORD64(WINAPI* SymLoadModule64Fn)(HANDLE hProcess, HANDLE hFile, PCSTR ImageName,
                                           PCSTR ModuleName, DWORD64 BaseOfDll, DWORD SizeOfDll);
typedef BOOL (WINAPI* SymSetSearchPathFn)(HANDLE hProcess, PCSTR SearchPath);
typedef BOOL (WINAPI* SymGetModuleInfo64Fn)(HANDLE hProcess, DWORD64 qwAddr,
                                            PIMAGEHLP_MODULE64 ModuleInfo);
typedef BOOL (WINAPI* SymGetLineFromAddr64Fn)(HANDLE hProcess, DWORD64 qwAddr,
                                              PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line64);
typedef BOOL (WINAPI* SymGetSymFromAddr64Fn)(HANDLE hProcess, DWORD64 qwAddr,
                                             PDWORD64 pdwDisplacement, PIMAGEHLP_SYMBOL64 Symbol);
typedef DWORD64(WINAPI* SymGetModuleBase64Fn)(HANDLE hProcess, DWORD64 qwAddr);
typedef PVOID(WINAPI* SymFunctionTableAccess64Fn)(HANDLE hProcess, DWORD64 AddrBase);

typedef BOOL (WINAPI* StackWalk64Fn)(DWORD MachineType, HANDLE hProcess, HANDLE hThread,
                                     LPSTACKFRAME64 StackFrame, PVOID ContextRecord, PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
                                     PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
                                     PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
typedef WORD(WINAPI* RtlCaptureStackBackTraceFn)(DWORD FramesToSkip, DWORD FramesToCapture,
                                                 PVOID* BackTrace, PDWORD BackTraceHash);

static EnumProcessesFn         CallEnumProcesses;
static EnumProcessModulesFn    CallEnumProcessModules;
static GetModuleBaseNameFn     CallGetModuleBaseName;
static GetModuleFileNameExFn   CallGetModuleFileNameEx;
static GetModuleInformationFn  CallGetModuleInformation;

static SymInitializeFn             CallSymInitialize;
static SymSetOptionsFn             CallSymSetOptions;
static SymGetOptionsFn             CallSymGetOptions;
static SymLoadModule64Fn           CallSymLoadModule64;
static SymSetSearchPathFn          CallSymSetSearchPath;
static SymGetModuleInfo64Fn        CallSymGetModuleInfo64;
static SymGetLineFromAddr64Fn      CallSymGetLineFromAddr64;
static SymGetSymFromAddr64Fn       CallSymGetSymFromAddr64;
static SymGetModuleBase64Fn        CallSymGetModuleBase64;
static SymFunctionTableAccess64Fn  CallSymFunctionTableAccess64;

static StackWalk64Fn               CallStackWalk64;
static RtlCaptureStackBackTraceFn  CallRtlCaptureStackBackTrace;

#  define USE_CAPTURESTACKBACKTRACE 1

#  if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG

LONG WINAPI
_stacktrace_exception_filter(LPEXCEPTION_POINTERS pointers) {
	FOUNDATION_UNUSED(pointers);
	log_error(0, ERROR_EXCEPTION, STRING_CONST("Exception occurred in stack trace!"));
	return EXCEPTION_EXECUTE_HANDLER;
}

#  endif

# if FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL

static int
_capture_stack_trace_helper(void** trace, size_t max_depth, size_t skip_frames,
                            CONTEXT* context) {
	STACKFRAME64   stack_frame;
	HANDLE         process_handle;
	HANDLE         thread_handle;
	bool           succeeded = true;
	unsigned int   current_depth = 0;
	unsigned int   machine_type	= IMAGE_FILE_MACHINE_I386;
	CONTEXT        context_copy = *context;

#if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	LPTOP_LEVEL_EXCEPTION_FILTER prev_filter;
	prev_filter = SetUnhandledExceptionFilter(_stacktrace_exception_filter);
#else
	__try
#endif
	{
		process_handle = GetCurrentProcess();
		thread_handle  = GetCurrentThread();

		memset(&stack_frame, 0, sizeof(stack_frame));

		stack_frame.AddrPC.Mode         = AddrModeFlat;
		stack_frame.AddrStack.Mode      = AddrModeFlat;
		stack_frame.AddrFrame.Mode      = AddrModeFlat;
#if FOUNDATION_ARCH_X86_64
		stack_frame.AddrPC.Offset       = context->Rip;
		stack_frame.AddrStack.Offset    = context->Rsp;
		stack_frame.AddrFrame.Offset    = context->Rbp;
		machine_type                    = IMAGE_FILE_MACHINE_AMD64;
#else
		stack_frame.AddrPC.Offset       = context->Eip;
		stack_frame.AddrStack.Offset    = context->Esp;
		stack_frame.AddrFrame.Offset    = context->Ebp;
#endif

		while (succeeded && (current_depth < max_depth)) {
			succeeded = CallStackWalk64(machine_type, process_handle, thread_handle, &stack_frame,
			                            &context_copy, 0, CallSymFunctionTableAccess64, CallSymGetModuleBase64, 0);
			if (succeeded) {
				if (!stack_frame.AddrFrame.Offset || !stack_frame.AddrPC.Offset)
					break;
				else if (skip_frames)
					--skip_frames;
				else
					trace[current_depth++] = (void*)((uintptr_t)stack_frame.AddrPC.Offset);
			}
		}
	}
#if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	SetUnhandledExceptionFilter(prev_filter);
#else
	__except (EXCEPTION_EXECUTE_HANDLER) {
		// We need to catch any exceptions within this function so they don't get sent to
		// the engine's error handler, hence causing an infinite loop.
		return EXCEPTION_EXECUTE_HANDLER;
	}
#endif

	memset(trace + current_depth, 0, sizeof(void*) * (max_depth - current_depth));

	return EXCEPTION_EXECUTE_HANDLER;
}

#endif

#define MAX_MOD_HANDLES   1024

static void
_load_process_modules() {
	/*lint -e534 */
	HMODULE       module_handles[MAX_MOD_HANDLES];
	HMODULE*      module_handle = module_handles;
	DWORD         module_count;
	DWORD         i;
	DWORD         bytes = 0;
	MODULEINFO    module_info;
	HANDLE        process_handle = GetCurrentProcess();

	if (!CallEnumProcessModules(process_handle, module_handles, sizeof(module_handles), &bytes))
		return;

	if (bytes > sizeof(module_handles)) {
		module_handle = memory_allocate(0, bytes, 0, MEMORY_TEMPORARY);
		CallEnumProcessModules(process_handle, module_handle, bytes, &bytes);
	}

	module_count = bytes / sizeof(HMODULE);

	for (i = 0; i < module_count; ++i) {
		char module_name[BUILD_MAX_PATHLEN];
		char image_name[BUILD_MAX_PATHLEN];
		char search_path[BUILD_MAX_PATHLEN];
		char* file_name = 0;

		CallGetModuleInformation(process_handle, module_handle[i], &module_info, sizeof(module_info));
		CallGetModuleFileNameEx(process_handle, module_handle[i], image_name, sizeof(image_name));
		CallGetModuleBaseName(process_handle, module_handle[i], module_name, sizeof(module_name));

		GetFullPathNameA(image_name, sizeof(search_path), search_path, &file_name);
		*file_name = 0;
		CallSymSetSearchPath(process_handle, search_path);

		CallSymLoadModule64(process_handle, module_handle[i], image_name, module_name,
		                    (uint64_t)((uintptr_t)module_info.lpBaseOfDll), module_info.SizeOfImage);
	}

	// Free the module handle pointer allocated in case the static array was insufficient.
	if (module_handle != module_handles)
		memory_deallocate(module_handle);
}

#endif

#if FOUNDATION_PLATFORM_ANDROID

struct android_trace_t {
	void**  trace;
	size_t  cur_depth;
	size_t  max_depth;
	size_t  skip_frames;
};

struct android_module_t {
	uintptr_t           address_start;
	uintptr_t           address_end;
	char                namebuf[64];
	string_t            name;
};

typedef struct android_trace_t android_trace_t;
typedef struct android_module_t android_module_t;

static android_module_t* _process_modules;
static size_t            _process_modules_size;

static void
_load_process_modules(void) {
	size_t imod = 0;
	char line_buffer[256];
	string_t line;

	if (_process_modules)
		memset(_process_modules, 0, sizeof(android_module_t) * _process_modules_size);

	stream_t* maps = fs_open_file(STRING_CONST("/proc/self/maps"), STREAM_IN);
	if (!maps) {
		log_error(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/maps"));
		return;
	}

	while (!stream_eos(maps) && (!_process_modules || (imod < _process_modules_size))) {
		line = stream_read_line_buffer(maps, line_buffer, sizeof(line_buffer), '\n');

		if (!line.length)
			continue;

		uintptr_t start = (uintptr_t)string_to_uint64(STRING_ARGS(line), true);
		size_t dashofs = string_find(STRING_ARGS(line), '-', 0) + 1;
		uintptr_t end = (uintptr_t)string_to_uint64(line.str + dashofs, line.length - dashofs, true);
		size_t modofs = (string_find_last_of(STRING_ARGS(line), STRING_CONST("/ "), STRING_NPOS) + 1);
		const char* module = line.str + modofs;

		if (!module[0] ||
		        (string_find_first_not_of(module, line.length - modofs, STRING_CONST("0123456789"),
		                                  0) == STRING_NPOS))
			continue;

		if (_process_modules && (imod > 0) &&
		        /*( start == _process_modules[imod].address_end ) && */
		        string_equal(module, line.length - modofs, STRING_ARGS(_process_modules[imod - 1].name))) {
			_process_modules[imod - 1].address_end = end;
			continue;
		}

		if (_process_modules) {
			_process_modules[imod].address_start = start;
			_process_modules[imod].address_end = end;
			_process_modules[imod].name = string_copy(_process_modules[imod].namebuf,
			                                          sizeof(_process_modules[imod].namebuf),
			                                          module, line.length - modofs);
		}
		++imod;
	}

	stream_deallocate(maps);

	//for( int i = 0; i < imod; ++i )
	//	log_infof(0, STRING_CONST("%" PRIfixPTR "-%" PRIfixPTR ": %.*s"), _process_modules[i].address_start, _process_modules[i].address_end, STRING_FORMAT(_process_modules[i].name));

	if (_process_modules && (imod == _process_modules_size))
		log_warn(0, WARNING_MEMORY, STRING_CONST("Too many modules encountered"));

	if (!_process_modules) {
		_process_modules_size = imod + 16;
		_process_modules = memory_allocate(0, sizeof(android_module_t) * _process_modules_size, 0,
		                                   MEMORY_PERSISTENT);
	}
}

#ifndef _URC_NORMAL_STOP
#  define _URC_NORMAL_STOP 4
#endif

static _Unwind_Reason_Code
unwind_stack(struct _Unwind_Context* context, void* arg) {
	android_trace_t* trace = arg;
	void* ip = (void*)(uintptr_t)_Unwind_GetIP(context);
	if (trace->skip_frames)
		--trace->skip_frames;
	else if (ip) {
		if (trace->cur_depth < trace->max_depth)
			trace->trace[trace->cur_depth++] = ip;
		else
			return _URC_NORMAL_STOP;
	}
	return _URC_NO_REASON;
}

#endif

static bool _stackwalk_initialized = false;

#if FOUNDATION_PLATFORM_WINDOWS
static void* _stacktrace_dbghelp_dll;
static void* _stacktrace_ntdll_dll;
#endif

static bool
_initialize_stackwalker(void) {
	if (_stackwalk_initialized)
		return true;

#if FOUNDATION_PLATFORM_WINDOWS
	{
		if (!_stacktrace_dbghelp_dll)
			_stacktrace_dbghelp_dll = LoadLibraryA("DBGHELP.DLL");
		CallStackWalk64 = _stacktrace_dbghelp_dll ?
		                  (StackWalk64Fn)GetProcAddress(_stacktrace_dbghelp_dll, "StackWalk64") :
		                  0;
		if (!CallStackWalk64) {
			log_warn(0, WARNING_SYSTEM_CALL_FAIL,
			         STRING_CONST("Unable to load dbghelp DLL for StackWalk64"));
			return false;
		}

		if (!_stacktrace_ntdll_dll)
			_stacktrace_ntdll_dll = LoadLibraryA("NTDLL.DLL");
		CallRtlCaptureStackBackTrace = _stacktrace_ntdll_dll ?
		                               (RtlCaptureStackBackTraceFn)GetProcAddress(_stacktrace_ntdll_dll, "RtlCaptureStackBackTrace") :
		                               0;
		if (!CallRtlCaptureStackBackTrace) {
			log_warn(0, WARNING_SYSTEM_CALL_FAIL,
			         STRING_CONST("Unable to load ntdll DLL for RtlCaptureStackBackTrace"));
			return false;
		}
	}
#endif

	_stackwalk_initialized = true;
	return true;
}

static void
_finalize_stackwalker(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	if (_stacktrace_ntdll_dll)
		FreeLibrary(_stacktrace_ntdll_dll);
	if (_stacktrace_ntdll_dll)
		FreeLibrary(_stacktrace_ntdll_dll);
	_stacktrace_dbghelp_dll = 0;
	_stacktrace_ntdll_dll = 0;
#endif
	_stackwalk_initialized = false;
}

size_t FOUNDATION_NOINLINE
stacktrace_capture(void** trace, size_t max_depth, size_t skip_frames) {
	size_t num_frames = 0;

	if (!trace)
		return 0;

	if (!max_depth)
		max_depth = _foundation_config.stacktrace_depth;

	if (max_depth > _foundation_config.stacktrace_depth)
		max_depth = _foundation_config.stacktrace_depth;

	if (!_stackwalk_initialized) {
		if (!_initialize_stackwalker()) {
			memset(trace, 0, sizeof(void*) * max_depth);
			return num_frames;
		}
	}

#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	// Add 1 skip frame for this function call
	++skip_frames;
#  if USE_CAPTURESTACKBACKTRACE
	if (CallRtlCaptureStackBackTrace) {
		num_frames = CallRtlCaptureStackBackTrace((DWORD)skip_frames, (DWORD)max_depth, trace, 0);
		if (num_frames < max_depth)
			memset(trace + num_frames, 0, sizeof(void*) * (max_depth - num_frames));
	}
	else
#  endif
	{
#  if FOUNDATION_ARCH_X86_64
		// Raise an exception so helper has access to context record.
		__try {
			RaiseException(0,  // Application-defined exception code.
			               0,  // Zero indicates continuable exception.
			               0,  // Number of arguments in args array (ignored if args is null)
			               0); // Array of arguments
		}
		__except (_capture_stack_trace_helper(trace, max_depth, skip_frames,
		                                      (GetExceptionInformation())->ContextRecord)) {
		}
#  else
		// Use a bit of inline assembly to capture the information relevant to stack walking which is
		// basically EIP and EBP.
		CONTEXT context;
		memset(&context, 0, sizeof(CONTEXT));
		context.ContextFlags = CONTEXT_FULL;

		log_warn(0, WARNING_DEPRECATED,
		         STRING_CONST("********** REIMPLEMENT FALLBACK STACKTRACE **********"));
		/* Use a fake function call to pop the return address and retrieve EIP.*/
		__asm {
			call FakeStackTraceCall
			FakeStackTraceCall:
			pop eax
			mov context.Eip, eax
			mov context.Ebp, ebp
			mov context.Esp, esp
		}

		// Capture the back trace.
		_capture_stack_trace_helper(trace, max_depth, skip_frames, &context);
#  endif
	}

#elif FOUNDATION_PLATFORM_ANDROID

	android_trace_t stack_trace = {
		.trace = trace,
		.cur_depth = 0,
		.max_depth = max_depth,
		.skip_frames = skip_frames
	};

	_Unwind_Backtrace(unwind_stack, &stack_trace);

	num_frames = stack_trace.cur_depth;

#elif FOUNDATION_PLATFORM_LINUX_RASPBERRYPI

# define READ_32BIT_MEMORY( addr ) (*(uint32_t volatile* volatile)(addr))
	volatile uint32_t fp = 0;
	volatile uint32_t pc = 0;

	//Grab initial frame pointer
	__asm volatile("mov %[result], fp\n\t" : [result] "=r"(fp));

	_gcc_barrier_function(fp);

	while (fp && (num_frames < max_depth)) {
		pc = READ_32BIT_MEMORY(fp);
		fp = READ_32BIT_MEMORY(fp - 4);

		if ((fp > 0x1000) && pc) {
			if (skip_frames > 0) {
				--skip_frames;
			}
			else {
				void* instruction = (void*)(uintptr_t)(pc & ~3);
				trace[num_frames++] = instruction;
			}
		}
		else {
			fp = 0;
		}
	}

#elif FOUNDATION_PLATFORM_POSIX

	// Add 1 skip frames for this function call
	skip_frames += 1;

	void* localframes[64];
	int ret = (int)backtrace(localframes, 64);
	if (ret > (int)skip_frames) {
		num_frames = (size_t)ret - skip_frames;
		if (num_frames > max_depth)
			num_frames = max_depth;
		memcpy(trace, localframes + skip_frames, sizeof(void*) * num_frames);
	}
	else {
		num_frames = 0;
		trace[0] = 0;
	}

#else
	FOUNDATION_UNUSED(skip_frames);
#endif

	return num_frames;
}

static bool _symbol_resolve_initialized = false;
#if FOUNDATION_PLATFORM_WINDOWS
static void* _stacktrace_psapi_dll;
#endif

static bool
_initialize_symbol_resolve() {
	if (_symbol_resolve_initialized)
		return true;

#if FOUNDATION_PLATFORM_WINDOWS
	{
		unsigned int options;
		void* dll;

		if (!_stacktrace_psapi_dll)
			_stacktrace_psapi_dll = LoadLibraryA("PSAPI.DLL");
		if (!_stacktrace_psapi_dll)
			return _symbol_resolve_initialized;

		dll = _stacktrace_psapi_dll;
		CallEnumProcesses = (EnumProcessesFn)GetProcAddress(dll, "EnumProcesses");
		CallEnumProcessModules = (EnumProcessModulesFn)GetProcAddress(dll, "EnumProcessModules");
		CallGetModuleFileNameEx = (GetModuleFileNameExFn)GetProcAddress(dll, "GetModuleFileNameExA");
		CallGetModuleBaseName = (GetModuleBaseNameFn)GetProcAddress(dll, "GetModuleBaseNameA");
		CallGetModuleInformation = (GetModuleInformationFn)GetProcAddress(dll, "GetModuleInformation");

		if (!CallEnumProcesses || !CallEnumProcessModules || !CallGetModuleFileNameEx ||
		        !CallGetModuleBaseName || !CallGetModuleInformation)
			return _symbol_resolve_initialized;

		if (!_stacktrace_dbghelp_dll)
			_stacktrace_dbghelp_dll = LoadLibraryA("DBGHELP.DLL");
		if (!_stacktrace_dbghelp_dll)
			return _symbol_resolve_initialized;

		dll = _stacktrace_dbghelp_dll;
		CallSymInitialize = (SymInitializeFn)GetProcAddress(dll, "SymInitialize");
		CallSymSetOptions = (SymSetOptionsFn)GetProcAddress(dll, "SymSetOptions");
		CallSymGetOptions = (SymGetOptionsFn)GetProcAddress(dll, "SymGetOptions");
		CallSymLoadModule64 = (SymLoadModule64Fn)GetProcAddress(dll, "SymLoadModule64");
		CallSymSetSearchPath = (SymSetSearchPathFn)GetProcAddress(dll, "SymSetSearchPath");
		CallSymGetModuleInfo64 = (SymGetModuleInfo64Fn)GetProcAddress(dll, "SymGetModuleInfo64");
		CallSymGetLineFromAddr64 = (SymGetLineFromAddr64Fn)GetProcAddress(dll, "SymGetLineFromAddr64");
		CallSymGetSymFromAddr64 = (SymGetSymFromAddr64Fn)GetProcAddress(dll, "SymGetSymFromAddr64");
		CallSymGetModuleBase64 = (SymGetModuleBase64Fn)GetProcAddress(dll, "SymGetModuleBase64");
		CallSymFunctionTableAccess64 = (SymFunctionTableAccess64Fn)GetProcAddress(dll,
		                               "SymFunctionTableAccess64");

		if (!CallSymInitialize || !CallSymSetOptions || !CallSymGetOptions || !CallSymLoadModule64 ||
		        !CallSymSetSearchPath || !CallSymGetModuleInfo64 || !CallSymGetLineFromAddr64 ||
		        !CallSymGetSymFromAddr64  || !CallSymGetModuleBase64 || !CallSymFunctionTableAccess64)
			return _symbol_resolve_initialized;

		options = CallSymGetOptions();
		options |= SYMOPT_LOAD_LINES;
		options |= SYMOPT_DEBUG;
		options |= SYMOPT_UNDNAME;
		options |= SYMOPT_LOAD_LINES;
		options |= SYMOPT_FAIL_CRITICAL_ERRORS;
		options |= SYMOPT_DEFERRED_LOADS;
		options |= SYMOPT_ALLOW_ABSOLUTE_SYMBOLS;
		options |= SYMOPT_EXACT_SYMBOLS;
		options |= SYMOPT_CASE_INSENSITIVE;
		CallSymSetOptions(options);

		CallSymInitialize(GetCurrentProcess(), 0, TRUE);
	}

	_load_process_modules();

	_symbol_resolve_initialized = true;

#elif FOUNDATION_PLATFORM_ANDROID

	_load_process_modules();

#else

	_symbol_resolve_initialized = true;

#endif

	return _symbol_resolve_initialized;
}

static void
_finalize_symbol_resolve() {
#if FOUNDATION_PLATFORM_ANDROID
	memory_deallocate(_process_modules);
	_process_modules = 0;
#endif
#if FOUNDATION_PLATFORM_WINDOWS
	if (_stacktrace_psapi_dll)
		FreeLibrary(_stacktrace_psapi_dll);
	_stacktrace_psapi_dll = 0;
#endif
	_symbol_resolve_initialized = false;
}

#if FOUNDATION_PLATFORM_WINDOWS || FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_BSD
#define UNKNOWN_SYMBOL "?""?"
#endif

static FOUNDATION_NOINLINE string_t
_resolve_stack_frames(char* buffer, size_t capacity, void** frames, size_t max_frames) {
#if FOUNDATION_PLATFORM_WINDOWS
	char                symbol_buffer[ sizeof(IMAGEHLP_SYMBOL64) + 512 ];
	PIMAGEHLP_SYMBOL64  symbol;
	DWORD               displacement = 0;
	uint64_t            displacement64 = 0;
	size_t              iaddr;
	HANDLE              process_handle = GetCurrentProcess();
	bool                last_was_main = false;
	IMAGEHLP_LINE64     line64;
	IMAGEHLP_MODULE64   module64;
	string_t            resolved = {buffer, 0};

	for (iaddr = 0; (iaddr < max_frames) && !last_was_main && (resolved.length < capacity-1); ++iaddr) {
		string_t line;
		const char* function_name = UNKNOWN_SYMBOL;
		const char* file_name = function_name;
		const char* module_name = function_name;
		unsigned int line_number = 0;

		//Allow first frame to be null in case of a function call to a null pointer
		if (iaddr && !frames[iaddr])
			break;

		// Initialize symbol.
		symbol = (PIMAGEHLP_SYMBOL64)symbol_buffer;
		memset(symbol, 0, sizeof(symbol_buffer));
		symbol->SizeOfStruct = sizeof(symbol_buffer);
		symbol->MaxNameLength = 512;

		// Get symbol from address.
		if (CallSymGetSymFromAddr64 &&
		        CallSymGetSymFromAddr64(process_handle, (uint64_t)((uintptr_t)frames[iaddr]), &displacement64,
		                                symbol)) {
			size_t offset = 0;
			while ((offset < sizeof(symbol->Name)) && (symbol->Name[offset] < 32))
				++offset;
			if (offset < sizeof(symbol->Name))
				function_name = symbol->Name + offset;
		}

		memset(&line64, 0, sizeof(line64));
		line64.SizeOfStruct = sizeof(line64);
		if (CallSymGetLineFromAddr64 &&
		        CallSymGetLineFromAddr64(process_handle, (uint64_t)((uintptr_t)frames[iaddr]), &displacement,
		                                 &line64)) {
			file_name = line64.FileName;
			line_number = line64.LineNumber;
		}

		memset(&module64, 0, sizeof(module64));
		module64.SizeOfStruct = sizeof(module64);
		if (CallSymGetModuleInfo64 &&
		        CallSymGetModuleInfo64(process_handle, (uint64_t)((uintptr_t)frames[iaddr]), &module64)) {
			size_t last_slash = STRING_NPOS;
			module_name = module64.ImageName;
			last_slash = string_rfind(module_name, string_length(module_name), '\\', STRING_NPOS);
			if (last_slash != STRING_NPOS)
				module_name += last_slash + 1;
		}

		if (resolved.length)
			resolved.str[resolved.length++] = '\n';
		line = string_format(resolved.str + resolved.length, capacity - resolved.length,
		                     STRING_CONST("[0x%" PRIfixPTR "] %s (%s:%d +%d bytes) [in %s]"),
		                     frames[iaddr], function_name, file_name, line_number,
		                     displacement, module_name);
		resolved.length += line.length;

		if (string_equal(function_name, string_length(function_name), STRING_CONST("main")))
			last_was_main = true;
	}

	return resolved;

#elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS

	//TODO: Use dladdr instead to avoid memory allocation with malloc in backtrace_symbols
	string_t symbols = (string_t) { buffer, 0 };
	char** resolved = backtrace_symbols(frames, (int)max_frames);
	for (unsigned int iframe = 0; iframe < max_frames; ++iframe) {
		size_t length = string_length(resolved[iframe]);
		if (length)
			symbols = string_append_varg(symbols.str, symbols.length, capacity, resolved[iframe], length,
			                             STRING_CONST(STRING_NEWLINE), nullptr);
	}
	free(resolved);

	return symbols;

#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_BSD

	string_const_t* args = 0;
	process_t* proc;
	unsigned int num_frames = 0;
	unsigned int requested_frames = 0;
	bool last_was_main = false;
	string_t resolved = (string_t) {buffer, 0};
	string_t line, filename, function;
	stream_t* procout;

	if (!environment_executable_path().length) {
		for (unsigned int iaddr = 0; iaddr < max_frames; ++iaddr) {
			//Allow first frame to be null in case of a function call to a null pointer
			if (iaddr && !frames[iaddr])
				break;
			line = string_format(resolved.str + resolved.length, capacity - resolved.length,
			                     STRING_CONST("[0x%" PRIfixPTR "]" STRING_NEWLINE),
			                     (uintptr_t)frames[iaddr]);
			resolved.length += line.length;
		}
		return resolved;
	}

	array_push(args, string_const(STRING_CONST("-e")));
	array_push(args, environment_executable_path());
	array_push(args, string_const(STRING_CONST("-f")));

	for (unsigned int iaddr = 0; iaddr < max_frames; ++iaddr) {
		//Allow first frame to be null in case of a function call to a null pointer
		if (iaddr && !frames[iaddr])
			break;

		line = string_format(resolved.str + resolved.length, capacity - resolved.length,
		                     STRING_CONST("0x%" PRIfixPTR), (uintptr_t)frames[iaddr]);
		resolved.length += line.length;

		array_push(args, string_const(STRING_ARGS(line)));
		++requested_frames;
	}

	proc = process_allocate();
	process_set_working_directory(proc, STRING_ARGS(environment_initial_working_directory()));
	process_set_executable_path(proc, STRING_CONST("/usr/bin/addr2line"));
	process_set_arguments(proc, args, array_size(args));
	process_set_flags(proc, PROCESS_ATTACHED | PROCESS_STDSTREAMS);

	process_spawn(proc);
	procout = process_stdout(proc);

	resolved.length = 0;
	while (!stream_eos(procout) && (num_frames < requested_frames) && !last_was_main) {
		line = string_format(resolved.str + resolved.length, capacity - resolved.length,
		                     STRING_CONST("[0x%" PRIfixPTR "] "), (uintptr_t)frames[num_frames]);
		resolved.length += line.length;

		function = stream_read_line_buffer(procout, resolved.str + resolved.length,
		                                   capacity - resolved.length, '\n');
		if (!function.length)
			function = string_copy(resolved.str + resolved.length, capacity - resolved.length,
			                       STRING_CONST(UNKNOWN_SYMBOL));
		resolved.length += function.length;

		string_copy(resolved.str + resolved.length, capacity - resolved.length, STRING_CONST(" ("));
		resolved.length += 2;

		filename = stream_read_line_buffer(procout, resolved.str + resolved.length,
		                                   capacity - resolved.length, '\n');
		if (!filename.length)
			filename = string_copy(resolved.str + resolved.length, capacity - resolved.length,
			                       STRING_CONST(UNKNOWN_SYMBOL));
		resolved.length += filename.length;

		string_copy(resolved.str + resolved.length, capacity - resolved.length,
		            STRING_CONST(")" STRING_NEWLINE));
		resolved.length += 2;

		if (string_equal(STRING_ARGS(function), STRING_CONST("main")))
			last_was_main = true;

		++num_frames;
	}

	process_wait(proc);
	process_deallocate(proc);

	array_deallocate(args);

	return resolved;

#elif FOUNDATION_PLATFORM_ANDROID

	string_t symbols = (string_t) { buffer, 0 };
	string_t line;
	char linebuf[128];

	_load_process_modules();

	for (unsigned int iaddr = 0; iaddr < max_frames; ++iaddr) {
		//Allow first frame to be null in case of a function call to a null pointer
		if (iaddr && !frames[iaddr])
			break;

		//Find the module and relative address
		uintptr_t relativeframe = (uintptr_t)frames[iaddr];
		string_const_t module = string_const(STRING_CONST("<no module found>"));

		for (size_t imod = 0; imod < _process_modules_size; ++imod) {
			if ((relativeframe >= _process_modules[imod].address_start) &&
			        (relativeframe < _process_modules[imod].address_end)) {
				relativeframe -= _process_modules[imod].address_start;
				module = string_const(STRING_ARGS(_process_modules[imod].name));
				break;
			}
		}

		line = string_format(linebuf, sizeof(linebuf),
		                     STRING_CONST("[0x%" PRIfixPTR "] 0x%" PRIfixPTR " %.*s\n"),
		                     (uintptr_t)frames[iaddr], (uintptr_t)relativeframe, STRING_FORMAT(module));
		symbols = string_append(STRING_ARGS(symbols), capacity, STRING_ARGS(line));
	}

	return symbols;

#else

	string_t symbols = (string_t) { buffer, 0 };
	string_t line;
	char linebuf[64];

	for (unsigned int iaddr = 0; iaddr < max_frames; ++iaddr) {
		//Allow first frame to be null in case of a function call to a null pointer
		if (iaddr && !frames[iaddr])
			break;

		line = string_format(linebuf, sizeof(linebuf), STRING_CONST("[0x%" PRIfixPTR "]\n"),
		                     (uintptr_t)frames[iaddr]);
		symbols = string_append(STRING_ARGS(symbols), capacity, STRING_ARGS(line));
	}

	return symbols;

#endif
}

string_t
stacktrace_resolve(char* str, size_t length, void** trace, size_t max_depth, size_t skip_frames) {
	_initialize_symbol_resolve();

	if (!max_depth)
		max_depth = _foundation_config.stacktrace_depth;
	if (max_depth + skip_frames > _foundation_config.stacktrace_depth)
		max_depth = _foundation_config.stacktrace_depth - skip_frames;

	return _resolve_stack_frames(str, length, trace + skip_frames, max_depth);
}

int
_stacktrace_initialize(void) {
#if FOUNDATION_PLATFORM_ANDROID
	_load_process_modules();
#endif
	return 0;
}

void
_stacktrace_finalize(void) {
	_finalize_symbol_resolve();
	_finalize_stackwalker();
}

