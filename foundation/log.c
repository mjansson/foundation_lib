/* log.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <stdio.h>
#include <stdarg.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#if BUILD_ENABLE_LOG && (FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL || FOUNDATION_COMPILER_CLANG)
#define snprintf(p, s, ...) _snprintf_s(p, s, _TRUNCATE, __VA_ARGS__)
#define vsnprintf(s, n, format, arg) _vsnprintf_s(s, n, _TRUNCATE, format, arg)
#endif
#endif

#if FOUNDATION_PLATFORM_ANDROID
#include <android/log.h>
#endif

#if FOUNDATION_PLATFORM_TIZEN
#include <foundation/tizen.h>
#endif

#if FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#endif

#if BUILD_ENABLE_LOG || BUILD_ENABLE_DEBUG_LOG

static bool log_stdout_enabled = true;
static bool log_prefix = true;
static log_handler_fn log_handler_user;
static hashtable64_t* log_suppress_table;
static error_level_t log_suppress_default;
static bool FOUNDATION_THREADLOCAL log_auto_newline = true;

#define LOG_WARNING_NAMES 11
static char* log_warning_name[LOG_WARNING_NAMES] = {"performance", "deprecated", "invalid value",    "memory",
                                                    "unsupported", "suspicious", "system call fail", "deadlock",
                                                    "script",      "resource",   "network"};

#define LOG_ERROR_NAMES 18
static char* log_error_name[LOG_ERROR_NAMES] = {"none",
                                                "invalid value",
                                                "unsupported",
                                                "not implemented",
                                                "out of memory",
                                                "memory leak",
                                                "memory alignment",
                                                "internal failure",
                                                "access denied",
                                                "exception",
                                                "system call fail",
                                                "unknown type",
                                                "unknown resource",
                                                "deprecated",
                                                "assert",
                                                "script",
                                                "corrupt data",
                                                "network"};

struct log_timestamp_t {
	int hours;
	int minutes;
	int seconds;
	int milliseconds;
};

typedef struct log_timestamp_t log_timestamp_t;

static log_timestamp_t
log_make_timestamp(void) {
	tick_t elapsed;
	tick_t ticks_per_sec;
	tick_t milliseconds;
	tick_t seconds;
	tick_t minutes;

	log_timestamp_t timestamp;

	ticks_per_sec = time_ticks_per_second();
	if (!ticks_per_sec) {
		memset(&timestamp, 0, sizeof(timestamp));
		return timestamp;
	}

	elapsed = time_current() - time_startup();
	milliseconds = ((elapsed % ticks_per_sec) * 1000LL) / ticks_per_sec;
	seconds = elapsed / ticks_per_sec;
	minutes = seconds / 60LL;

	timestamp.milliseconds = (milliseconds % 1000LL);
	timestamp.seconds = (seconds % 60LL);
	timestamp.minutes = (minutes % 60LL);
	timestamp.hours = (int)(minutes / 60LL);

	return timestamp;
}

#endif

#if BUILD_ENABLE_LOG

static int log_hwthread_width;
static int log_tid_width;

static void
FOUNDATION_PRINTFCALL(5, 0)
    log_outputf(hash_t context, error_level_t severity, const char* prefix, size_t prefix_length, const char* format,
                size_t format_length, va_list list, void* std) {
	log_timestamp_t timestamp = log_make_timestamp();
	uint64_t tid = thread_id();
	unsigned int hwthreadid = thread_hardware();
	int need, more, remain;
	int size = 388;
	char local_buffer[390];
	char* buffer = local_buffer;
	FOUNDATION_UNUSED(format_length);
	/*lint -e716 */
	while (1) {
		// This is guaranteed to always fit in minimum size of 388 bytes defined above, so need is
		// always > 0
		if (log_prefix) {
			if (tid >= 0x1000000)
				log_tid_width = math_max(8, log_tid_width);
			else if (tid >= 0x100000)
				log_tid_width = math_max(6, log_tid_width);
			else if (tid >= 0x10000)
				log_tid_width = math_max(5, log_tid_width);
			need = snprintf(buffer, (unsigned int)size, "[%d:%02d:%02d.%03d] <%-*" PRIx64 ":%-*u> %.*s",
			                timestamp.hours, timestamp.minutes, timestamp.seconds, timestamp.milliseconds,
			                log_tid_width, tid, log_hwthread_width, hwthreadid, (int)prefix_length, prefix);
		} else {
			need = snprintf(buffer, (unsigned int)size, "%.*s", (int)prefix_length, prefix);
		}

		remain = size - need;
		{
			va_list clist;
			va_copy(clist, list);
			more = vsnprintf(buffer + need, (unsigned int)remain, format, clist);
			va_end(clist);
		}

		if ((more > -1) && (more < remain)) {
			int endl = need + more;
			if (log_auto_newline || (severity >= ERRORLEVEL_WARNING))
				buffer[endl++] = '\n';
			buffer[endl] = 0;

#if FOUNDATION_PLATFORM_WINDOWS
			if (log_stdout_enabled)
				OutputDebugStringA(buffer);
#endif

#if FOUNDATION_PLATFORM_ANDROID
			FOUNDATION_UNUSED(std);
			if (log_stdout_enabled)
				__android_log_write(ANDROID_LOG_DEBUG + severity - 1, environment_application()->short_name.str,
				                    buffer);
#elif FOUNDATION_PLATFORM_TIZEN
			FOUNDATION_UNUSED(std);
			if (log_stdout_enabled)
				dlog_print(DLOG_DEBUG + severity - 1, environment_application()->short_name.str, "%s", buffer);
#else
			if (log_stdout_enabled && std) {
				fprintf(std, "%s", buffer);
				if (!log_auto_newline)
					fflush(std);
			}
#endif

			if (log_handler_user)
				log_handler_user(context, severity, buffer, (unsigned int)(endl - 1));

			break;
		}

		if ((more > -1) && (need > -1))
			size = more + need + 1;
		else
			size *= 2;

		if (buffer != local_buffer)
			memory_deallocate(buffer);
		buffer = memory_allocate(0, (unsigned int)(size + 2), 0, MEMORY_TEMPORARY);
	}
	if (buffer != local_buffer)
		memory_deallocate(buffer);
}

#endif

#if BUILD_ENABLE_LOG && BUILD_ENABLE_DEBUG_LOG

void
log_debugf(hash_t context, const char* format, size_t length, ...) {
	va_list list;
	va_start(list, length);
	if (log_suppress(context) < ERRORLEVEL_DEBUG)
		log_outputf(context, ERRORLEVEL_DEBUG, "", 0, format, length, list, stdout);
	va_end(list);
}

void
log_debug(hash_t context, const char* msg, size_t length) {
	log_debugf(context, STRING_CONST("%.*s"), (int)length, msg);
}

#endif

#if BUILD_ENABLE_LOG

void
log_infof(hash_t context, const char* format, size_t length, ...) {
	va_list list;
	va_start(list, length);
	if (log_suppress(context) < ERRORLEVEL_INFO)
		log_outputf(context, ERRORLEVEL_INFO, "", 0, format, length, list, stdout);
	va_end(list);
}

void
log_info(hash_t context, const char* msg, size_t length) {
	log_infof(context, STRING_CONST("%.*s"), (int)length, msg);
}

void
log_warnf(hash_t context, warning_t warn, const char* format, size_t length, ...) {
	char buffer[32];
	string_t prefix;
	va_list list;

	if (log_suppress(context) >= ERRORLEVEL_WARNING)
		return;

	log_error_context(context, ERRORLEVEL_WARNING);

	if (warn < LOG_WARNING_NAMES)
		prefix = string_format(buffer, sizeof(buffer), STRING_CONST("WARNING [%s]: "), log_warning_name[warn]);
	else
		prefix = string_format(buffer, sizeof(buffer), STRING_CONST("WARNING [%d]: "), warn);

	va_start(list, length);
	log_outputf(context, ERRORLEVEL_WARNING, prefix.str, prefix.length, format, length, list, stdout);
	va_end(list);
}

void
log_warn(hash_t context, warning_t warn, const char* msg, size_t length) {
	log_warnf(context, warn, STRING_CONST("%.*s"), (int)length, msg);
}

void
log_errorf(hash_t context, error_t err, const char* format, size_t length, ...) {
	char buffer[32];
	string_t prefix;
	va_list list;

	error_report(ERRORLEVEL_ERROR, err);

	if (log_suppress(context) >= ERRORLEVEL_ERROR)
		return;

	log_error_context(context, ERRORLEVEL_ERROR);

	if (err < LOG_ERROR_NAMES)
		prefix = string_format(buffer, sizeof(buffer), STRING_CONST("ERROR [%s]: "), log_error_name[err]);
	else
		prefix = string_format(buffer, sizeof(buffer), STRING_CONST("ERROR [%d]: "), err);

	va_start(list, length);
	log_outputf(context, ERRORLEVEL_ERROR, prefix.str, prefix.length, format, length, list, stderr);
	va_end(list);
}

void
log_error(hash_t context, error_t err, const char* msg, size_t length) {
	log_errorf(context, err, STRING_CONST("%.*s"), (int)length, msg);
}

void
log_panicf(hash_t context, error_t err, const char* format, size_t length, ...) {
	char buffer[32];
	string_t prefix;
	va_list list;

	error_report(ERRORLEVEL_PANIC, err);

	log_error_context(context, ERRORLEVEL_PANIC);

	if (err < LOG_ERROR_NAMES)
		prefix = string_format(buffer, sizeof(buffer), STRING_CONST("PANIC [%s]: "), log_error_name[err]);
	else
		prefix = string_format(buffer, sizeof(buffer), STRING_CONST("PANIC [%d]: "), err);

	va_start(list, length);
	log_outputf(context, ERRORLEVEL_PANIC, prefix.str, prefix.length, format, length, list, stderr);
	va_end(list);
}

void
log_panic(hash_t context, error_t err, const char* msg, size_t length) {
	log_panicf(context, err, STRING_CONST("%.*s"), (int)length, msg);
}

static void
FOUNDATION_PRINTFCALL(4, 6)
    log_error_contextf(hash_t context, error_level_t error_level, void* std, const char* format, size_t length, ...) {
	va_list list;
	va_start(list, length);
	log_outputf(context, error_level, "", 0, format, length, list, std);
	va_end(list);
}

void
log_error_context(hash_t context, error_level_t error_level) {
	size_t i;
	error_context_t* err_context = error_context();
	if (err_context && (log_suppress(context) < error_level)) {
		error_frame_t* frame = err_context->frame;
		for (i = 0; i < err_context->depth; ++i, ++frame)
			log_error_contextf(context, error_level, error_level > ERRORLEVEL_WARNING ? stderr : stdout,
			                   STRING_CONST("When %.*s%.*s%.*s"), (int)frame->name.length, frame->name.str,
			                   frame->data.length ? 2 : 0, frame->data.length ? ": " : "", (int)frame->data.length,
			                   frame->data.str);
	}
}

void
log_enable_stdout(bool enable) {
	log_stdout_enabled = enable;
}

bool
log_stdout(void) {
	return log_stdout_enabled;
}

log_handler_fn
log_handler(void) {
	return log_handler_user;
}

void
log_set_handler(log_handler_fn handler) {
	log_handler_user = handler;
}

void
log_enable_prefix(bool enable) {
	log_prefix = enable;
}

void
log_enable_auto_newline(bool enable) {
	log_auto_newline = enable;
}

void
log_set_suppress(hash_t context, error_level_t level) {
	if (!context)
		log_suppress_default = level;
	else if (log_suppress_table)
		hashtable64_set(log_suppress_table, context, (unsigned int)(level + 1));
}

error_level_t
log_suppress(hash_t context) {
	if (!context)
		return log_suppress_default;
	else if (log_suppress_table) {
		uint64_t level = hashtable64_get(log_suppress_table, context);
		if (level > 0)
			return (error_level_t)(level - 1);
	}
	return log_suppress_default;
}

void
log_suppress_clear(hash_t context) {
	if (!context) {
		log_suppress_default = ERRORLEVEL_NONE;
		if (log_suppress_table)
			hashtable64_clear(log_suppress_table);
	} else {
		if (log_suppress_table)
			hashtable64_set(log_suppress_table, context, 0);
	}
}

#endif

#if FOUNDATION_PLATFORM_WINDOWS && BUILD_ENABLE_LOG

static void
enable_vt100(HANDLE stream) {
	DWORD current_mode = 0;
	GetConsoleMode(stream, &current_mode);
	SetConsoleMode(stream, current_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
}

#endif

int
internal_log_initialize(void) {
#if BUILD_ENABLE_LOG
	log_suppress_table = hashtable64_allocate(149);

	size_t hwthread_count = system_hardware_threads();
	if (hwthread_count < 10)
		log_hwthread_width = 1;
	else if (hwthread_count < 100)
		log_hwthread_width = 2;
	else if (hwthread_count < 1000)
		log_hwthread_width = 3;
	else
		log_hwthread_width = 8;
	log_tid_width = 4;

#if FOUNDATION_PLATFORM_WINDOWS
	// In Windows 10 and later we can use VT100 controls if we enable it
	enable_vt100(GetStdHandle(STD_OUTPUT_HANDLE));
	enable_vt100(GetStdHandle(STD_ERROR_HANDLE));
#endif
#endif
	return 0;
}

void
internal_log_finalize(void) {
#if BUILD_ENABLE_LOG
	hashtable64_deallocate(log_suppress_table);
	log_suppress_table = 0;
#endif
}
