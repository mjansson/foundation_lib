/* main.c  -  Foundation error test  -  Public Domain  -  2013 Mattias Jansson
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
#include <test/test.h>

static application_t
test_error_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation error tests"));
	app.short_name = string_const(STRING_CONST("test_error"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_error_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_error_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_error_initialize(void) {
	return 0;
}

static void
test_error_finalize(void) {
}

DECLARE_TEST(error, error) {
	error();
	EXPECT_EQ(error(), ERROR_NONE);
	EXPECT_EQ(error(), ERROR_NONE);

	error_report(ERRORLEVEL_WARNING, ERROR_ACCESS_DENIED);
	EXPECT_EQ(error(), ERROR_ACCESS_DENIED);
	EXPECT_EQ(error(), ERROR_NONE);

	error_report(ERRORLEVEL_ERROR, ERROR_INVALID_VALUE);
	EXPECT_EQ(error(), ERROR_INVALID_VALUE);
	EXPECT_EQ(error(), ERROR_NONE);

	return 0;
}

DECLARE_TEST(error, context) {
	error_context_t* context = 0;

	context = error_context();
	if (context)
		EXPECT_EQ(context->depth, 0);

	error_context_push(STRING_CONST("error test"), STRING_CONST("data"));
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE(context, 0);
	EXPECT_EQ(context->depth, 1);
	EXPECT_CONSTSTRINGEQ(context->frame[0].name, string_const(STRING_CONST("error test")));
	EXPECT_CONSTSTRINGEQ(context->frame[0].data, string_const(STRING_CONST("data")));
#else
	EXPECT_EQ(context, 0);
#endif

	error_context_pop();
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE(context, 0);
	EXPECT_EQ(context->depth, 0);
#else
	EXPECT_EQ(context, 0);
#endif

	error_context_push(STRING_CONST("error test"), STRING_CONST("data"));
	error_context_push(STRING_CONST("another test"), STRING_CONST("more data"));
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE(context, 0);
	EXPECT_EQ(context->depth, 2);
	EXPECT_CONSTSTRINGEQ(context->frame[0].name, string_const(STRING_CONST("error test")));
	EXPECT_CONSTSTRINGEQ(context->frame[0].data, string_const(STRING_CONST("data")));
	EXPECT_CONSTSTRINGEQ(context->frame[1].name, string_const(STRING_CONST("another test")));
	EXPECT_CONSTSTRINGEQ(context->frame[1].data, string_const(STRING_CONST("more data")));
#else
	EXPECT_EQ(context, 0);
#endif

	error_context_pop();
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE(context, 0);
	EXPECT_EQ(context->depth, 1);
	EXPECT_CONSTSTRINGEQ(context->frame[0].name, string_const(STRING_CONST("error test")));
	EXPECT_CONSTSTRINGEQ(context->frame[0].data, string_const(STRING_CONST("data")));
#else
	EXPECT_EQ(context, 0);
#endif

	error_context_pop();

	return 0;
}

static void*
error_test_thread(void) {
	error_context_t* context = 0;

	EXPECT_EQ(error(), ERROR_NONE);
	EXPECT_EQ(error(), ERROR_NONE);

	error_report(ERRORLEVEL_WARNING, ERROR_ACCESS_DENIED);
	EXPECT_EQ(error(), ERROR_ACCESS_DENIED);
	EXPECT_EQ(error(), ERROR_NONE);

	error_report(ERRORLEVEL_ERROR, ERROR_INVALID_VALUE);
	EXPECT_EQ(error(), ERROR_INVALID_VALUE);
	EXPECT_EQ(error(), ERROR_NONE);

	context = error_context();
	if (context)
		EXPECT_EQ(context->depth, 0);

	error_context_push(STRING_CONST("error test"), STRING_CONST("data"));
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE(context, 0);
	EXPECT_EQ(context->depth, 1);
	EXPECT_CONSTSTRINGEQ(context->frame[0].name, string_const(STRING_CONST("error test")));
	EXPECT_CONSTSTRINGEQ(context->frame[0].data, string_const(STRING_CONST("data")));
#else
	EXPECT_EQ(context, 0);
#endif

	error_context_pop();
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE(context, 0);
	EXPECT_EQ(context->depth, 0);
#else
	EXPECT_EQ(context, 0);
#endif

	error_context_push(STRING_CONST("error test"), STRING_CONST("data"));
	error_context_push(STRING_CONST("another test"), STRING_CONST("more data"));
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE(context, 0);
	EXPECT_EQ(context->depth, 2);
	EXPECT_CONSTSTRINGEQ(context->frame[0].name, string_const(STRING_CONST("error test")));
	EXPECT_CONSTSTRINGEQ(context->frame[0].data, string_const(STRING_CONST("data")));
	EXPECT_CONSTSTRINGEQ(context->frame[1].name, string_const(STRING_CONST("another test")));
	EXPECT_CONSTSTRINGEQ(context->frame[1].data, string_const(STRING_CONST("more data")));
#else
	EXPECT_EQ(context, 0);
#endif

	error_context_pop();
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE(context, 0);
	EXPECT_EQ(context->depth, 1);
	EXPECT_CONSTSTRINGEQ(context->frame[0].name, string_const(STRING_CONST("error test")));
	EXPECT_CONSTSTRINGEQ(context->frame[0].data, string_const(STRING_CONST("data")));
#else
	EXPECT_EQ(context, 0);
#endif

	error_context_pop();

	return 0;
}

static void*
error_thread(void* arg) {
	int ipass = 0;
	FOUNDATION_UNUSED(arg);

	thread_sleep(10);

	for (ipass = 0; ipass < 512; ++ipass) {
		if (error_test_thread())
			return FAILED_TEST;
		thread_yield();
	}

	return 0;
}

DECLARE_TEST(error, thread) {
	// Launch 32 threads
	thread_t thread[32];
	int i;

	for (i = 0; i < 32; ++i)
		thread_initialize(&thread[i], error_thread, 0, STRING_CONST("error"), THREAD_PRIORITY_NORMAL, 0);
	for (i = 0; i < 32; ++i)
		thread_start(&thread[i]);

	test_wait_for_threads_startup(thread, 32);
	test_wait_for_threads_finish(thread, 32);

	for (i = 0; i < 32; ++i) {
		EXPECT_EQ(thread[i].result, 0);
		thread_finalize(&thread[i]);
	}

	return 0;
}

#if BUILD_ENABLE_LOG

static int
ignore_error_handler(error_level_t level, error_t err) {
	FOUNDATION_UNUSED(level);
	FOUNDATION_UNUSED(err);
	return 0;
}

static hash_t last_log_context;
static error_level_t last_log_severity;
static const char* last_log_msg;
static size_t last_log_length;

static void
log_verify_handler(hash_t context, error_level_t severity, const char* msg, size_t length) {
	last_log_context = context;
	last_log_severity = severity;
	last_log_msg = msg;
	last_log_length = length;
}

#endif

DECLARE_TEST(error, output) {
#if BUILD_ENABLE_LOG
	error_handler_fn handler_error = error_handler();
	log_handler_fn handler_log = log_handler();
	string_const_t shortmsg = string_const(STRING_CONST("Short message with prefix"));
	string_const_t longmsg = string_const(STRING_CONST("Longer message which should be output without a prefix"));

	error_set_handler(ignore_error_handler);
	log_set_handler(log_verify_handler);

	log_enable_stdout(false);
	EXPECT_EQ(log_stdout(), false);
	log_warn(HASH_TEST, WARNING_SUSPICIOUS, STRING_ARGS(shortmsg));
	log_enable_stdout(true);
	EXPECT_EQ(log_stdout(), true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_WARNING);
	EXPECT_GE(last_log_length, shortmsg.length);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), 0);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_CONST("WARNING [suspicious]"), 0), STRING_NPOS);

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_stdout(false);
	log_warn(HASH_TEST, (warning_t)0x1000, STRING_ARGS(shortmsg));
	log_enable_stdout(true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_WARNING);
	EXPECT_GE(last_log_length, shortmsg.length);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), 0);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_CONST("WARNING [4096]"), 0), STRING_NPOS);

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_prefix(false);
	log_enable_stdout(false);
	log_warn(HASH_TEST, WARNING_SYSTEM_CALL_FAIL, STRING_ARGS(longmsg));
	log_enable_stdout(true);
	log_enable_prefix(true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_WARNING);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(longmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(longmsg), 0), 0);

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_stdout(false);
	log_error(HASH_TEST, ERROR_DEPRECATED, STRING_ARGS(shortmsg));
	log_enable_stdout(true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_ERROR);
	EXPECT_GE(last_log_length, shortmsg.length);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), 0);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_CONST("ERROR [deprecated]"), 0), STRING_NPOS);

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_stdout(false);
	log_error(HASH_TEST, (error_t)0x1000, STRING_ARGS(shortmsg));
	log_enable_stdout(true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_ERROR);
	EXPECT_GE(last_log_length, shortmsg.length);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), 0);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_CONST("ERROR [4096]"), 0), STRING_NPOS);

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_prefix(false);
	log_enable_stdout(false);
	log_error(HASH_TEST, ERROR_INVALID_VALUE, STRING_ARGS(longmsg));
	log_enable_stdout(true);
	log_enable_prefix(true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_ERROR);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(longmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(longmsg), 0), 0);

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_stdout(false);
	log_panic(HASH_TEST, ERROR_DEPRECATED, STRING_ARGS(shortmsg));
	log_enable_stdout(true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_PANIC);
	EXPECT_GE(last_log_length, shortmsg.length);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), 0);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_CONST("PANIC [deprecated]"), 0), STRING_NPOS);

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_stdout(false);
	log_panic(HASH_TEST, (error_t)0x1000, STRING_ARGS(shortmsg));
	log_enable_stdout(true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_PANIC);
	EXPECT_GE(last_log_length, shortmsg.length);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(shortmsg), 0), 0);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_CONST("PANIC [4096]"), 0), STRING_NPOS);

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_prefix(false);
	log_enable_stdout(false);
	log_panic(HASH_TEST, ERROR_INVALID_VALUE, STRING_ARGS(longmsg));
	log_enable_stdout(true);
	log_enable_prefix(true);
	EXPECT_EQ(last_log_context, HASH_TEST);
	EXPECT_EQ(last_log_severity, ERRORLEVEL_PANIC);
	EXPECT_NE(string_find_string(last_log_msg, last_log_length, STRING_ARGS(longmsg), 0), STRING_NPOS);
	EXPECT_GT(string_find_string(last_log_msg, last_log_length, STRING_ARGS(longmsg), 0), 0);

#if BUILD_ENABLE_ERROR_CONTEXT

	error_context_push(STRING_CONST("one"), STRING_CONST("dataone"));
	error_context_push(STRING_CONST("two"), STRING_CONST("datatwo"));
	error_context_push(STRING_CONST("three"), STRING_CONST("datathree"));

	last_log_context = 0;
	last_log_severity = ERRORLEVEL_NONE;
	last_log_msg = nullptr;
	last_log_length = 0;

	log_enable_stdout(false);
	log_error_context(HASH_TEST, ERRORLEVEL_INFO);
	log_enable_stdout(true);

	error_context_pop();
	error_context_pop();
	error_context_pop();

	EXPECT_SIZEEQ(string_find_string(last_log_msg, last_log_length, STRING_CONST("When one: dataone"), 0), STRING_NPOS);
	EXPECT_SIZEEQ(string_find_string(last_log_msg, last_log_length, STRING_CONST("When two: datatwo"), 0), STRING_NPOS);
	EXPECT_SIZENE(string_find_string(last_log_msg, last_log_length, STRING_CONST("When three: datathree"), 0),
	              STRING_NPOS);

#endif

	log_set_handler(handler_log);
	error_set_handler(handler_error);
#endif
	return 0;
}

static void
test_error_declare(void) {
	ADD_TEST(error, error);
	ADD_TEST(error, context);
	ADD_TEST(error, thread);
	ADD_TEST(error, output);
}

static test_suite_t test_error_suite = {test_error_application,
                                        test_error_memory_system,
                                        test_error_config,
                                        test_error_declare,
                                        test_error_initialize,
                                        test_error_finalize,
                                        0};

#if BUILD_MONOLITHIC

int
test_error_run(void);

int
test_error_run(void) {
	test_suite = test_error_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_error_suite;
}

#endif
