/* main.c  -  Foundation app test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <test/test.h>

static application_t _global_app;

static application_t
test_app_application(void) {
	_global_app.name = string_const(STRING_CONST("Foundation library test suite"));
	_global_app.short_name = string_const(STRING_CONST("test_app"));
	_global_app.config_dir = string_const(STRING_CONST("test_app"));
	_global_app.version = foundation_version();
	_global_app.flags = APPLICATION_UTILITY;
	_global_app.dump_callback = test_crash_handler;
	return _global_app;
}

static memory_system_t
test_app_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_app_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	config.temporary_memory = 128 * 1024;
	return config;
}

static int
test_app_initialize(void) {
	return 0;
}

static void
test_app_finalize(void) {
}

DECLARE_TEST(app, environment) {
	EXPECT_CONSTSTRINGEQ(environment_application()->name, _global_app.name);
#if !BUILD_MONOLITHIC
	EXPECT_CONSTSTRINGEQ(environment_application()->short_name, _global_app.short_name);
	EXPECT_CONSTSTRINGEQ(environment_application()->config_dir, _global_app.config_dir);
#endif
	EXPECT_TRUE(uint128_equal(environment_application()->version.version,
	                          _global_app.version.version));
	EXPECT_EQ(environment_application()->flags, APPLICATION_UTILITY);
	EXPECT_EQ(environment_application()->dump_callback, test_crash_handler);
	return 0;
}

static void*
memory_thread(void* arg) {
	FOUNDATION_UNUSED(arg);
	int iloop, lsize;
	void* min = 0;
	void* max = 0;
	size_t diff;

	for (iloop = 0, lsize = 512 * 1024; iloop < lsize; ++iloop) {
		void* mem = memory_allocate(0, 17, 16, MEMORY_TEMPORARY | MEMORY_ZERO_INITIALIZED);
		EXPECT_EQ((uintptr_t)mem & 0x0F, 0);
		thread_yield();
		memory_deallocate(mem);
		if (!min || (mem < min))
			min = mem;
		if (!max || (mem > max))
			max = mem;
	}

	diff = (uintptr_t)pointer_diff(max, min);
	EXPECT_SIZELE(diff, 128 * 1024);
	EXPECT_SIZEGE(diff, 32);

	return 0;
}

DECLARE_TEST(app, memory) {
	thread_t thread[32];
	size_t ith;
	size_t num_threads = math_clamp(system_hardware_threads() * 2, 2, 32);
	memory_statistics_t oldstats, newstats;

	oldstats = memory_statistics();

	for (ith = 0; ith < num_threads; ++ith)
		thread_initialize(&thread[ith], memory_thread, 0, STRING_CONST("memory_thread"),
		                  THREAD_PRIORITY_NORMAL, 0);

	for (ith = 0; ith < num_threads; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, num_threads);
	test_wait_for_threads_finish(thread, num_threads);

	for (ith = 0; ith < num_threads; ++ith)
		EXPECT_EQ(thread[ith].result, 0);

	for (ith = 0; ith < num_threads; ++ith)
		thread_finalize(&thread[ith]);

	newstats = memory_statistics();

#if BUILD_ENABLE_MEMORY_STATISTICS
	EXPECT_SIZEEQ(oldstats.allocated_current, newstats.allocated_current);
#endif

	return 0;
}

static void
test_app_declare(void) {
	ADD_TEST(app, environment);
	ADD_TEST(app, memory);
}

static test_suite_t test_app_suite = {
	test_app_application,
	test_app_memory_system,
	test_app_config,
	test_app_declare,
	test_app_initialize,
	test_app_finalize
};

#if BUILD_MONOLITHIC

int
test_app_run(void);

int
test_app_run(void) {
	test_suite = test_app_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_app_suite;
}

#endif
