/* main.c  -  Foundation app test  -  Public Domain  -  2013 Mattias Jansson
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
#include <mock/mock.h>

#if FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#endif

static application_t global_app;

static application_t
test_app_application(void) {
	global_app.name = string_const(STRING_CONST("Foundation application tests"));
	global_app.short_name = string_const(STRING_CONST("test_app"));
	global_app.company = string_const(STRING_CONST(""));
	global_app.version = foundation_version();
	global_app.flags = APPLICATION_UTILITY;
	global_app.exception_handler = test_exception_handler;
	return global_app;
}

static memory_system_t
test_app_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_app_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
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
	const application_t decl = test_app_application();

	// Test double init
	EXPECT_TRUE(foundation_is_initialized());
	EXPECT_EQ(foundation_initialize(test_app_memory_system(), test_app_application(), test_app_config()), 0);

#if !BUILD_MONOLITHIC
	EXPECT_CONSTSTRINGEQ(environment_application()->name, decl.name);
	EXPECT_CONSTSTRINGEQ(environment_application()->short_name, decl.short_name);
#endif
	EXPECT_CONSTSTRINGEQ(environment_application()->company, decl.company);
	EXPECT_TRUE(uint128_equal(environment_application()->version.version, decl.version.version));
	EXPECT_EQ(environment_application()->flags, APPLICATION_UTILITY);
	EXPECT_EQ(environment_application()->exception_handler, test_exception_handler);

	return 0;
}

static FOUNDATION_NOINLINE void*
memory_thread(void* arg) {
	FOUNDATION_UNUSED(arg);
	size_t iloop, lsize;

#if !BUILD_ENABLE_MEMORY_TRACKER
	for (iloop = 0, lsize = 512 * 1024; iloop < lsize; ++iloop) {
#else
	for (iloop = 0, lsize = 8 * 1024; iloop < lsize; ++iloop) {
#endif
		void* mem = memory_allocate(0, 17 + (iloop % 997), 16, MEMORY_TEMPORARY | MEMORY_ZERO_INITIALIZED);
		EXPECT_EQ((uintptr_t)mem & 0x0F, 0);
		thread_yield();
		memory_deallocate(mem);
		thread_yield();
	}

	return 0;
}

#if BUILD_ENABLE_MEMORY_STATISTICS && BUILD_ENABLE_MEMORY_TRACKER
static size_t memory_dumps;
static size_t memory_dump_size;

static int
memory_dump(const void* addr, size_t size, void* const* trace, size_t depth) {
	FOUNDATION_UNUSED(addr);
	FOUNDATION_UNUSED(trace);
	FOUNDATION_UNUSED(depth);
	/*log_infof(HASH_TEST,
	          STRING_CONST("Memory alloc: %" PRIsize " bytes @ 0x%" PRIfixPTR),
	          size, (uintptr_t)addr);*/
	++memory_dumps;
	memory_dump_size += size;
	return 0;
}
#endif

DECLARE_TEST(app, memory) {
	thread_t thread[16];
	size_t ith;
	size_t threads_count = math_clamp(system_hardware_threads() + 1, 2, 16);

	log_set_suppress(HASH_MEMORY, ERRORLEVEL_NONE);

#if BUILD_ENABLE_MEMORY_STATISTICS && BUILD_ENABLE_MEMORY_TRACKER
	memory_statistics_t prestats = memory_statistics();
	EXPECT_SIZEGT(prestats.allocations_current, 1);
	EXPECT_SIZEGT(prestats.allocated_current, 1);
	EXPECT_SIZEGE(prestats.allocations_total, prestats.allocations_current);
	EXPECT_SIZEGE(prestats.allocated_total, prestats.allocated_current);
	memory_dumps = 0;
	memory_dump_size = 0;
	memory_tracker_dump(memory_dump);
	EXPECT_SIZEGT(memory_dumps, 1);
	EXPECT_SIZEGT(memory_dump_size, 1);
#endif

	for (ith = 0; ith < threads_count; ++ith)
		thread_initialize(&thread[ith], memory_thread, 0, STRING_CONST("memory_thread"), THREAD_PRIORITY_NORMAL, 0);

	for (ith = 0; ith < threads_count; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, threads_count);
	test_wait_for_threads_finish(thread, threads_count);

	for (ith = 0; ith < threads_count; ++ith)
		EXPECT_EQ(thread[ith].result, 0);

	for (ith = 0; ith < threads_count; ++ith)
		thread_finalize(&thread[ith]);

#if BUILD_ENABLE_MEMORY_STATISTICS && BUILD_ENABLE_MEMORY_TRACKER
	memory_statistics_t poststats = memory_statistics();
	EXPECT_SIZEEQ(poststats.allocations_current, prestats.allocations_current);
	EXPECT_SIZEEQ(poststats.allocated_current, prestats.allocated_current);
	EXPECT_SIZEGT(poststats.allocations_total, prestats.allocations_total);
	EXPECT_SIZEGT(poststats.allocated_total, prestats.allocated_total);
	size_t last_dumps = memory_dumps;
	size_t last_dump_size = memory_dump_size;
	memory_dumps = 0;
	memory_dump_size = 0;
	memory_tracker_dump(memory_dump);
	EXPECT_SIZEGT(memory_dumps, 1);
	EXPECT_SIZEGT(memory_dump_size, 1);
	EXPECT_SIZEEQ(memory_dumps, last_dumps);
	EXPECT_SIZEEQ(memory_dump_size, last_dump_size);
#endif

	return 0;
}

DECLARE_TEST(app, failure) {
#if FOUNDATION_SIZE_POINTER > 4
	error_level_t last_log_suppress = log_suppress(0);
	log_set_suppress(0, ERRORLEVEL_ERROR);
	//...
	log_set_suppress(0, last_log_suppress);
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
		uint group = (uint)(core / 64);
		core &= 64;
		uint64_t mask = (1ULL << core);
		thread_set_hardware(group, mask);
		for (size_t iloop = 0, lsize = 512 * 1024; iloop < lsize; ++iloop) {
			random64();
			// Not all platforms support setting thread cpu affinity
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
	size_t threads_count = math_clamp(system_hardware_threads() * 2U, 4U, 30U);
	semaphore_t sync;

	EXPECT_TRUE(thread_is_main());

	semaphore_initialize(&sync, 1);
	testthread = thread_allocate(test_thread, &sync, STRING_CONST("test_thread"), THREAD_PRIORITY_NORMAL, 0);
	thread_start(testthread);

	test_wait_for_threads_startup(testthread, 1);
	test_wait_for_threads_finish(testthread, 1);

	EXPECT_EQ(thread_join(testthread), nullptr);
	thread_deallocate(testthread);

	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_initialize(&thread[ithread], test_thread, &sync, STRING_CONST("test_thread"), THREAD_PRIORITY_NORMAL, 0);
	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_start(&thread[ithread]);

	test_wait_for_threads_startup(thread, threads_count);
	test_wait_for_threads_finish(thread, threads_count);

	for (ithread = 0; ithread < threads_count; ++ithread) {
		EXPECT_EQ(thread_join(&thread[ithread]), nullptr);
		thread_finalize(&thread[ithread]);
	}

	EXPECT_FALSE(thread_is_main());
	thread_set_main();

	semaphore_finalize(&sync);

	testthread = thread_allocate(sleep_thread, 0, STRING_CONST("sleep_thread"), THREAD_PRIORITY_NORMAL, 0);
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
	ADD_TEST(app, memory);
	ADD_TEST(app, environment);
	ADD_TEST(app, failure);
	ADD_TEST(app, thread);
}

static test_suite_t test_app_suite = {test_app_application,
                                      test_app_memory_system,
                                      test_app_config,
                                      test_app_declare,
                                      test_app_initialize,
                                      test_app_finalize,
                                      0};

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
