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

#define TEMPORARY_MEMORY_SIZE 256 * 1024

static application_t _global_app;

static application_t
test_app_application(void) {
	_global_app.name = string_const(STRING_CONST("Foundation application tests"));
	_global_app.short_name = string_const(STRING_CONST("test_app"));
	_global_app.company = string_const(STRING_CONST("Rampant Pixels"));
	_global_app.version = foundation_version();
	_global_app.flags = APPLICATION_UTILITY;
	_global_app.exception_handler = test_exception_handler;
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
	config.temporary_memory = TEMPORARY_MEMORY_SIZE;
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
#if !BUILD_MONOLITHIC
	EXPECT_CONSTSTRINGEQ(environment_application()->name, _global_app.name);
	EXPECT_CONSTSTRINGEQ(environment_application()->short_name, _global_app.short_name);
#endif
	EXPECT_CONSTSTRINGEQ(environment_application()->company, _global_app.company);
	EXPECT_TRUE(uint128_equal(environment_application()->version.version,
	                          _global_app.version.version));
	EXPECT_EQ(environment_application()->flags, APPLICATION_UTILITY);
	EXPECT_EQ(environment_application()->exception_handler, test_exception_handler);
	return 0;
}

static FOUNDATION_NOINLINE void*
memory_thread(void* arg) {
	FOUNDATION_UNUSED(arg);
	int iloop, lsize;
	void* min = 0;
	void* max = 0;
	size_t diff;

	for (iloop = 0, lsize = 512 * 1024; iloop < lsize; ++iloop) {
		void* mem = memory_allocate(0, 17, 16, MEMORY_TEMPORARY | MEMORY_ZERO_INITIALIZED);
#if FOUNDATION_PLATFORM_ANDROID
		EXPECT_EQ((uintptr_t)mem & 0x07, 0);
#else
		EXPECT_EQ((uintptr_t)mem & 0x0F, 0);
#endif
		if (!min || (mem < min))
			min = mem;
		if (!max || (mem > max))
			max = mem;
		thread_yield();
		memory_deallocate(mem);
		thread_yield();
	}

	diff = (uintptr_t)pointer_diff(max, min);
	EXPECT_SIZELT(diff, TEMPORARY_MEMORY_SIZE);
	EXPECT_SIZEGE(diff, 64);

	return 0;
}

DECLARE_TEST(app, memory) {
	thread_t thread[16];
	size_t ith;
	size_t num_threads = math_clamp(system_hardware_threads() * 2, 2, 16);

#if BUILD_ENABLE_MEMORY_STATISTICS
	memory_statistics_t oldstats, newstats;

	oldstats = memory_statistics();
#endif

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

#if BUILD_ENABLE_MEMORY_STATISTICS
	newstats = memory_statistics();
	EXPECT_SIZEEQ(oldstats.allocations_current, newstats.allocations_current);
	EXPECT_SIZEEQ(oldstats.allocated_current, newstats.allocated_current);
#endif

	return 0;
}

static void*
test_thread(void* arg) {
	semaphore_t* sync = (semaphore_t*)arg;

	EXPECT_FALSE(thread_is_main());

	EXPECT_CONSTSTRINGEQ(thread_name(), string_const(STRING_CONST("test_thread")));
	thread_set_name(STRING_CONST("other_thread"));
	EXPECT_CONSTSTRINGEQ(thread_name(), string_const(STRING_CONST("other_thread")));

	semaphore_wait(sync);
	thread_set_main();
	EXPECT_TRUE(thread_is_main());
	semaphore_post(sync);

	thread_set_name(STRING_CONST("test_thread"));
	EXPECT_CONSTSTRINGEQ(thread_name(), string_const(STRING_CONST("test_thread")));

	if (system_hardware_threads() > 1) {
		uint64_t core = random64_range(0, system_hardware_threads());
		uint64_t mask = (1ULL << core);
		thread_set_hardware(mask);
		for (size_t iloop = 0, lsize = 512 * 1024; iloop < lsize; ++iloop) {
			random64();
			//Not all platforms support setting thread cpu affinity
#if FOUNDATION_PLATFORM_WINDOWS || FOUNDATION_PLATFORM_LINUX
			EXPECT_UINTEQ(thread_hardware(), core);
#endif
			thread_yield();
		}
	}

	return 0;
}

static void*
sleep_thread(void* arg) {
	tick_t start, end, elapsed;
	FOUNDATION_UNUSED(arg);

	start = time_current();
	EXPECT_INTEQ(beacon_try_wait(&thread_self()->beacon, 5000), 0);
	end = time_current();
	elapsed = time_diff(start, end);
	EXPECT_REALLT(time_ticks_to_seconds(elapsed), REAL_C(6.0));

	return 0;
}

DECLARE_TEST(app, thread) {
	thread_t* testthread = 0;
	thread_t thread[32];
	size_t ithread;
	size_t num_threads = math_clamp(system_hardware_threads() * 2U, 4U, 30U);
	semaphore_t sync;

	EXPECT_TRUE(thread_is_main());

	semaphore_initialize(&sync, 1);
	testthread = thread_allocate(test_thread, &sync, STRING_CONST("test_thread"),
	                             THREAD_PRIORITY_NORMAL, 0);
	thread_start(testthread);

	test_wait_for_threads_startup(testthread, 1);
	test_wait_for_threads_finish(testthread, 1);

	EXPECT_EQ(thread_join(testthread), nullptr);
	thread_deallocate(testthread);

	for (ithread = 0; ithread < num_threads; ++ithread)
		thread_initialize(&thread[ithread], test_thread, &sync, STRING_CONST("test_thread"),
		                  THREAD_PRIORITY_NORMAL, 0);
	for (ithread = 0; ithread < num_threads; ++ithread)
		thread_start(&thread[ithread]);

	test_wait_for_threads_startup(thread, num_threads);
	test_wait_for_threads_finish(thread, num_threads);

	for (ithread = 0; ithread < num_threads; ++ithread) {
		EXPECT_EQ(thread_join(&thread[ithread]), nullptr);
		thread_finalize(&thread[ithread]);
	}

	EXPECT_FALSE(thread_is_main());
	thread_set_main();

	semaphore_finalize(&sync);

	testthread = thread_allocate(sleep_thread, 0, STRING_CONST("sleep_thread"), THREAD_PRIORITY_NORMAL,
	                             0);
	thread_start(testthread);

	test_wait_for_threads_startup(testthread, 1);
	thread_signal(testthread);
	test_wait_for_threads_finish(testthread, 1);

	EXPECT_EQ(thread_join(testthread), nullptr);
	thread_deallocate(testthread);

	return 0;
}

static void
test_app_declare(void) {
	ADD_TEST(app, environment);
	ADD_TEST(app, memory);
	ADD_TEST(app, thread);
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
