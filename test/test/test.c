/* test.c  -  Foundation test library  -  Public Domain  -  2013 Mattias Jansson
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

#if !BUILD_MONOLITHIC
FOUNDATION_EXTERN test_suite_t
test_suite_define(void);
#endif

test_suite_t test_suite;

typedef struct {
	string_const_t name;
	test_fn fn;
} test_case_t;

typedef struct {
	string_const_t name;
	test_case_t** cases;
} test_group_t;

static test_group_t** test_groups;
static bool test_failed_flag;

#if !BUILD_MONOLITHIC
static bool test_exiting;

static void*
test_event_thread(void* arg) {
	event_block_t* block;
	event_t* event = 0;
	FOUNDATION_UNUSED(arg);

	event_stream_set_beacon(system_event_stream(), &thread_self()->beacon);

	while (!test_exiting) {
		block = event_stream_process(system_event_stream());
		while ((event = event_next(block, event))) {
			switch (event->id) {
				case FOUNDATIONEVENT_TERMINATE:
					log_warn(HASH_TEST, WARNING_SUSPICIOUS, STRING_CONST("Terminating test due to event"));
					process_exit(-2);

				default:
					break;
			}
			test_event(event);
		}
		event = 0;
		thread_wait();
	}

	return 0;
}

void
test_event(event_t* event) {
	if (test_suite.event)
		test_suite.event(event);
}

#endif

void
test_add_test(test_fn fn, const char* group_name, size_t group_length, const char* test_name, size_t test_length) {
	unsigned int ig, gsize;
	test_group_t* test_group = 0;
	test_case_t* test_case = 0;
	for (ig = 0, gsize = array_size(test_groups); ig < gsize; ++ig) {
		if (string_equal(test_groups[ig]->name.str, test_groups[ig]->name.length, group_name, group_length)) {
			test_group = test_groups[ig];
			break;
		}
	}

	if (!test_group) {
		test_group = memory_allocate(0, sizeof(test_group_t), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		test_group->name = string_const(group_name, group_length);
		array_push(test_groups, test_group);
	}

	test_case = memory_allocate(0, sizeof(test_case_t), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	test_case->name = string_const(test_name, test_length);
	test_case->fn = fn;

	array_push(test_group->cases, test_case);
}

static void
test_run(void) {
	unsigned int ig, gsize, ic, csize;
	void* result = 0;
#if !BUILD_MONOLITHIC
	thread_t thread_event;
#endif

	log_infof(HASH_TEST, STRING_CONST("Running test suite: %.*s"), (int)test_suite.application().short_name.length,
	          test_suite.application().short_name.str);

	test_failed_flag = false;
	thread_set_main();

#if !BUILD_MONOLITHIC
	thread_initialize(&thread_event, test_event_thread, 0, STRING_CONST("event_thread"), THREAD_PRIORITY_NORMAL, 0);
	thread_start(&thread_event);

	while (!thread_is_started(&thread_event))
		thread_yield();

	error_set_handler(test_error_handler);
#endif

	for (ig = 0, gsize = array_size(test_groups); ig < gsize; ++ig) {
		log_infof(HASH_TEST, STRING_CONST("Running tests from group %.*s"), STRING_FORMAT(test_groups[ig]->name));
		for (ic = 0, csize = array_size(test_groups[ig]->cases); ic < csize; ++ic) {
			log_infof(HASH_TEST, STRING_CONST("  Running %.*s tests"), STRING_FORMAT(test_groups[ig]->cases[ic]->name));
			result = test_groups[ig]->cases[ic]->fn();
			if (result != 0) {
				log_warn(HASH_TEST, WARNING_SUSPICIOUS, STRING_CONST("    FAILED"));
				test_failed_flag = true;
			} else {
				log_info(HASH_TEST, STRING_CONST("    PASSED"));
			}
#if BUILD_MONOLITHIC
			if (test_should_terminate()) {
				test_failed_flag = true;
				goto exit;
			}
#endif
		}
	}

#if !BUILD_MONOLITHIC
	test_exiting = true;
	thread_signal(&thread_event);
	thread_finalize(&thread_event);
#else
exit:
#endif

	log_infof(HASH_TEST, STRING_CONST("Finished test suite: %.*s%.*s"),
	          STRING_FORMAT(test_suite.application().short_name), !test_failed_flag ? 0 : 9, " (FAILED)");
}

static void
test_free(void) {
	unsigned int ig, gsize, ic, csize;
	for (ig = 0, gsize = array_size(test_groups); ig < gsize; ++ig) {
		for (ic = 0, csize = array_size(test_groups[ig]->cases); ic < csize; ++ic)
			memory_deallocate(test_groups[ig]->cases[ic]);
		array_deallocate(test_groups[ig]->cases);
		memory_deallocate(test_groups[ig]);
	}
	array_deallocate(test_groups);
	test_groups = 0;

	// Abort memory tracking if failed test(s)
	if (test_failed_flag)
		memory_set_tracker(memory_tracker_none());
}

int
test_run_all(void) {
	if (test_suite.initialize() < 0)
		return -1;
	test_suite.declare();

	test_run();
	test_free();

	test_suite.finalize();
	if (test_failed_flag) {
		process_set_exit_code(-1);
		return -1;
	}
	return 0;
}

void
test_set_suitable_working_directory(void) {
	char buffer[BUILD_MAX_PATHLEN];
	bool found = false;

	string_const_t last_dir;
	string_t config_dir;
	string_t configure_py;
	string_const_t working_dir = environment_executable_directory();
	do {
		last_dir = working_dir;
		config_dir = path_concat(buffer, sizeof(buffer), STRING_ARGS(working_dir), STRING_CONST("config"));
		log_debugf(HASH_TEST, STRING_CONST("Check config dir: %.*s"), STRING_FORMAT(config_dir));
		if (fs_is_directory(STRING_ARGS(config_dir))) {
			log_debugf(HASH_TEST, STRING_CONST("Found config dir: %.*s"), STRING_FORMAT(config_dir));
			found = true;
			break;
		}
		configure_py = path_concat(buffer, sizeof(buffer), STRING_ARGS(working_dir), STRING_CONST("configure.py"));
		log_debugf(HASH_TEST, STRING_CONST("Check configure script: %.*s"), STRING_FORMAT(configure_py));
		if (fs_is_file(STRING_ARGS(configure_py))) {
			log_debugf(HASH_TEST, STRING_CONST("Found configure script: %.*s"), STRING_FORMAT(configure_py));
			found = true;
			break;
		}

		working_dir = path_directory_name(STRING_ARGS(working_dir));
	} while (!string_equal(STRING_ARGS(working_dir), STRING_ARGS(last_dir)));

	if (found) {
		log_debugf(HASH_TEST, STRING_CONST("Set test working dir: %.*s"), STRING_FORMAT(working_dir));
		environment_set_current_working_directory(STRING_ARGS(working_dir));
	}
}

void
test_load_config(json_handler_fn handler) {
	sjson_parse_path(STRING_CONST("config"), handler);

#if FOUNDATION_PLATFORM_WINDOWS
	sjson_parse_path(STRING_CONST("config/windows"), handler);
#elif FOUNDATION_PLATFORM_MACOS
	sjson_parse_path(STRING_CONST("config/macos"), handler);
#elif FOUNDATION_PLATFORM_LINUX
	sjson_parse_path(STRING_CONST("config/linux"), handler);
#elif FOUNDATION_PLATFORM_BSD
	sjson_parse_path(STRING_CONST("config/bsd"), handler);
#elif FOUNDATION_PLATFORM_ANDROID
	sjson_parse_path(STRING_CONST("config/android"), handler);
#elif FOUNDATION_PLATFORM_IOS
	sjson_parse_path(STRING_CONST("config/ios"), handler);
#elif FOUNDATION_PLATFORM_TIZEN
	sjson_parse_path(STRING_CONST("config/tizen"), handler);
#endif
}

#if !BUILD_MONOLITHIC

int
main_initialize(void) {
	bool memory_tracker = true;
	size_t iarg, asize;
	int ret;
	const string_const_t* cmdline;

	memory_set_tracker(memory_tracker_local());
	assert_force_continue(true);

	log_set_suppress(0, ERRORLEVEL_INFO);

	test_suite = test_suite_define();

	ret = foundation_initialize(test_suite.memory_system(), test_suite.application(), test_suite.config());
	if (ret == 0) {
		cmdline = environment_command_line();
		for (iarg = 0, asize = array_size(cmdline); iarg < asize; ++iarg) {
			if (string_equal(STRING_ARGS(cmdline[iarg]), STRING_CONST("--no-memory-tracker")))
				memory_tracker = false;
		}
		if (!memory_tracker)
			memory_set_tracker(memory_tracker_none());
	}

	return ret;
}

int
main_run(void* main_arg) {
	FOUNDATION_UNUSED(main_arg);
	log_set_suppress(HASH_TEST, ERRORLEVEL_DEBUG);

	test_set_suitable_working_directory();

	return test_run_all();
}

void
main_finalize(void) {
	foundation_finalize();
}

#endif

void
test_wait_for_threads_startup(const thread_t* threads, size_t threads_count) {
	size_t i;
	bool waiting = true;

	while (waiting) {
		waiting = false;

		atomic_thread_fence_acquire();

		for (i = 0; i < threads_count; ++i) {
			if (!thread_is_started(threads + i)) {
				waiting = true;
				break;
			}
		}

		if (waiting)
			thread_yield();
	}
}

void
test_wait_for_threads_finish(const thread_t* threads, size_t threads_count) {
	size_t i;
	bool waiting = true;

	while (waiting) {
		waiting = false;

		atomic_thread_fence_acquire();

		for (i = 0; i < threads_count; ++i) {
			if (thread_is_running(threads + i)) {
				waiting = true;
				break;
			}
		}

		if (waiting)
			thread_yield();
	}
}

void
test_wait_for_threads_join(thread_t* threads, size_t threads_count) {
	size_t i;

	atomic_thread_fence_acquire();

	for (i = 0; i < threads_count; ++i)
		thread_join(threads + i);
}

void
FOUNDATION_ATTRIBUTE(noreturn) test_exception_handler(const char* dump_file, size_t length) {
	FOUNDATION_UNUSED(dump_file);
	FOUNDATION_UNUSED(length);
	log_set_suppress(HASH_TEST, ERRORLEVEL_DEBUG);
	log_enable_stdout(true);
	log_error(HASH_TEST, ERROR_EXCEPTION, STRING_CONST("Test raised exception"));
	process_exit(-1);
}

int
test_error_handler(error_level_t level, error_t err) {
	FOUNDATION_UNUSED(err);
	if (level == ERRORLEVEL_PANIC) {
		log_set_suppress(HASH_TEST, ERRORLEVEL_DEBUG);
		log_enable_stdout(true);
		log_error(HASH_TEST, ERROR_EXCEPTION, STRING_CONST("Test panic"));
		process_exit(-2);
	}
	return 0;
}

static void (*test_fail_hook)(void);

void
test_prefail(void) {
	atomic_thread_fence_sequentially_consistent();
	log_set_suppress(HASH_TEST, ERRORLEVEL_DEBUG);
	log_enable_stdout(true);
	if (test_fail_hook)
		test_fail_hook();
}

void*
test_failed(void) {
	return FAILED_TEST;
}

void
test_set_fail_hook(void (*hook_fn)(void)) {
	test_fail_hook = hook_fn;
}
