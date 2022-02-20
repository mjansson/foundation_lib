/* assert.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <stdarg.h>

#define ASSERT_BUFFER_SIZE 1024

static mutex_t* assert_mutex;
static assert_handler_fn assert_handler_current;
static char assert_buffer[ASSERT_BUFFER_SIZE];
static bool assert_has_force_continue;
static bool assert_has_force_abort;

#if BUILD_ENABLE_ASSERT
#define ASSERT_STACKTRACE_MAX_DEPTH 128U
#define ASSERT_STACKTRACE_SKIP_FRAMES 1U
static void* assert_stacktrace[ASSERT_STACKTRACE_MAX_DEPTH];
static char assert_context_buffer[ASSERT_BUFFER_SIZE];
static char assert_stacktrace_buffer[ASSERT_BUFFER_SIZE];
static char assert_message_buffer[ASSERT_BUFFER_SIZE];
#endif

assert_handler_fn
assert_handler(void) {
	return assert_handler_current;
}

void
assert_set_handler(assert_handler_fn new_handler) {
	assert_handler_current = new_handler;
}

void
assert_force_continue(bool continue_execution) {
	assert_has_force_continue = continue_execution;
}

void
assert_force_abort(bool abort_execution) {
	assert_has_force_abort = abort_execution;
}

int
assert_report(hash_t context, const char* condition, size_t cond_length, const char* file, size_t file_length,
              unsigned int line, const char* msg, size_t msg_length) {
	static const char nocondition[] = "<Static fail>";
	static const char nofile[] = "<No file>";
	static const char nomsg[] = "<No message>";
	static const char assert_format[] =
	    "****** ASSERT FAILED ******\nCondition: %.*s\nFile/line: %.*s : %d\n%.*s%s%.*s\n%.*s\n";
#if BUILD_ENABLE_ASSERT
	string_t tracestr = {assert_stacktrace_buffer, sizeof(assert_stacktrace_buffer)};
	string_t contextstr = {assert_context_buffer, sizeof(assert_context_buffer)};
	string_t messagestr = {assert_message_buffer, sizeof(assert_message_buffer)};
#endif

	if (!condition || !cond_length) {
		condition = nocondition;
		cond_length = sizeof(nocondition);
	}
	if (!file || !file_length) {
		file = nofile;
		file_length = sizeof(nofile);
	}
	if (!msg || !msg_length) {
		msg = nomsg;
		msg_length = sizeof(nomsg);
	}

	if (assert_handler_current && (assert_handler_current != assert_report))
		return (*assert_handler_current)(context, condition, cond_length, file, file_length, line, msg, msg_length);

#if BUILD_ENABLE_ASSERT
	if (assert_mutex)
		mutex_lock(assert_mutex);

	contextstr = error_context_buffer(contextstr.str, contextstr.length);

	if (foundation_is_initialized()) {
		size_t frames_count =
		    stacktrace_capture(assert_stacktrace, ASSERT_STACKTRACE_MAX_DEPTH, ASSERT_STACKTRACE_SKIP_FRAMES);
		if (frames_count)
			tracestr = stacktrace_resolve(STRING_ARGS(tracestr), assert_stacktrace, frames_count, 0U);
		else
			tracestr = string_copy(STRING_ARGS(tracestr), STRING_CONST("<no stacktrace>"));
	} else {
		tracestr = string_copy(STRING_ARGS(tracestr), STRING_CONST("<no stacktrace - not initialized>"));
	}

	messagestr = string_format(STRING_ARGS(messagestr), assert_format, sizeof(assert_format) - 1, (int)cond_length,
	                           condition, (int)file_length, file, line, STRING_FORMAT(contextstr),
	                           (contextstr.length ? "\n" : ""), (int)msg_length, msg, STRING_FORMAT(tracestr));

	log_errorf(context, ERROR_ASSERT, STRING_CONST("%.*s"), STRING_FORMAT(messagestr));

	bool abort = !system_message_box(STRING_CONST("Assert Failure"), STRING_ARGS(messagestr), true);
	if (assert_has_force_abort)
		abort = true;
	else if (assert_has_force_continue)
		abort = false;
	else if (environment_application()->flags & APPLICATION_UTILITY)
		abort = system_debugger_attached();

	if (assert_mutex)
		mutex_unlock(assert_mutex);

	return abort ? 1 : 0;
#else
	log_errorf(context, ERROR_ASSERT, assert_format, sizeof(assert_format) - 1, (int)cond_length, condition,
	           (int)file_length, file, line, 0, "", "", (int)msg_length, msg, 0, "");
	return 1;
#endif
}

int
assert_report_formatted(hash_t context, const char* condition, size_t cond_length, const char* file, size_t file_length,
                        unsigned int line, const char* msg, size_t msg_length, ...) {
	if (msg) {
		string_t buffer = {assert_buffer, sizeof(assert_buffer)};
		va_list ap;
		va_start(ap, msg_length);
		buffer = string_vformat(STRING_ARGS(buffer), msg, msg_length, ap);
		va_end(ap);
		msg = buffer.str;
		msg_length = buffer.length;
	}
	return assert_report(context, condition, cond_length, file, file_length, line, msg, msg_length);
}

int
internal_assert_initialize(void) {
	assert_mutex = mutex_allocate(STRING_CONST("assert"));
	return 0;
}

void
internal_assert_finalize(void) {
	mutex_deallocate(assert_mutex);
	assert_mutex = 0;
}
