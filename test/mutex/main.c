/* main.c  -  Foundation mutex test  -  Public Domain  -  2013 Mattias Jansson
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
test_mutex_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation mutex tests"));
	app.short_name = string_const(STRING_CONST("test_mutex"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_mutex_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_mutex_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_mutex_initialize(void) {
	return 0;
}

static void
test_mutex_finalize(void) {
}

DECLARE_TEST(mutex, basic) {
	mutex_t* mutex;

	mutex = mutex_allocate(STRING_CONST("test"));

	EXPECT_CONSTSTRINGEQ(mutex_name(mutex), string_const(STRING_CONST("test")));
	EXPECT_TRUE(mutex_try_lock(mutex));
	EXPECT_TRUE(mutex_lock(mutex));
	EXPECT_TRUE(mutex_try_lock(mutex));
	EXPECT_TRUE(mutex_lock(mutex));

	EXPECT_TRUE(mutex_unlock(mutex));
	EXPECT_TRUE(mutex_unlock(mutex));
	EXPECT_TRUE(mutex_unlock(mutex));
	EXPECT_TRUE(mutex_unlock(mutex));

	log_set_suppress(0, ERRORLEVEL_WARNING);
	EXPECT_FALSE(mutex_unlock(mutex));
	log_set_suppress(0, ERRORLEVEL_INFO);

	mutex_signal(mutex);
	thread_yield();
	EXPECT_TRUE(mutex_try_wait(mutex, 1));
	EXPECT_TRUE(mutex_unlock(mutex));

	mutex_signal(mutex);
	thread_yield();
	EXPECT_TRUE(mutex_wait(mutex));
	EXPECT_TRUE(mutex_unlock(mutex));

	log_set_suppress(0, ERRORLEVEL_WARNING);
	EXPECT_FALSE(mutex_try_wait(mutex, 100));
	EXPECT_FALSE(mutex_unlock(mutex));
	log_set_suppress(0, ERRORLEVEL_INFO);

	mutex_signal(mutex);
	thread_yield();
	EXPECT_TRUE(mutex_try_wait(mutex, 1));
	log_set_suppress(0, ERRORLEVEL_WARNING);
	EXPECT_FALSE(mutex_try_wait(mutex, 100));
	EXPECT_TRUE(mutex_unlock(mutex));
	EXPECT_FALSE(mutex_unlock(mutex));
	log_set_suppress(0, ERRORLEVEL_INFO);

	mutex_deallocate(mutex);

	return 0;
}

static size_t thread_counter;

static void*
mutex_thread(void* arg) {
	mutex_t* mutex = arg;
	size_t i;

	for (i = 0; i < 128; ++i) {
		if (!mutex_try_lock(mutex))
			mutex_lock(mutex);

		++thread_counter;

		mutex_unlock(mutex);
	}

	return 0;
}

DECLARE_TEST(mutex, sync) {
	mutex_t* mutex;
	thread_t thread[32];
	size_t ith;

	mutex = mutex_allocate(STRING_CONST("test"));
	mutex_lock(mutex);

	for (ith = 0; ith < 32; ++ith)
		thread_initialize(&thread[ith], mutex_thread, mutex, STRING_CONST("mutex_thread"), THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < 32; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, 32);

	mutex_unlock(mutex);

	test_wait_for_threads_finish(thread, 32);

	for (ith = 0; ith < 32; ++ith)
		thread_finalize(&thread[ith]);

	mutex_deallocate(mutex);

	EXPECT_EQ(thread_counter, 32 * 128);

	return 0;
}

static atomic32_t thread_waiting;
static atomic32_t thread_waited;

static void*
thread_waiter(void* arg) {
	mutex_t* mutex = arg;

	atomic_incr32(&thread_waiting, memory_order_release);

	if (mutex_try_wait(mutex, 30000)) {
		atomic_incr32(&thread_waited, memory_order_release);
		mutex_unlock(mutex);
	} else {
		log_warn(HASH_TEST, WARNING_SUSPICIOUS, STRING_CONST("Thread timeout"));
	}

	return 0;
}

DECLARE_TEST(mutex, signal) {
	mutex_t* mutex;
	thread_t thread[32];
	size_t ith;

	mutex = mutex_allocate(STRING_CONST("test"));
	mutex_lock(mutex);

	for (ith = 0; ith < 32; ++ith)
		thread_initialize(&thread[ith], thread_waiter, mutex, STRING_CONST("thread_wait"), THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < 32; ++ith)
		thread_start(&thread[ith]);

	mutex_unlock(mutex);

	test_wait_for_threads_startup(thread, 32);

	while (atomic_load32(&thread_waiting, memory_order_acquire) < 32)
		thread_yield();
	thread_sleep(1000);  // Hack wait to give threads time to progress from atomic_incr to mutex_wait

	mutex_signal(mutex);

	test_wait_for_threads_finish(thread, 32);

	for (ith = 0; ith < 32; ++ith)
		thread_finalize(&thread[ith]);

	EXPECT_EQ(atomic_load32(&thread_waited, memory_order_acquire), 32);

	EXPECT_FALSE(mutex_try_wait(mutex, 500));

	mutex_deallocate(mutex);

	return 0;
}

static void
test_mutex_declare(void) {
	ADD_TEST(mutex, basic);
	ADD_TEST(mutex, sync);
	ADD_TEST(mutex, signal);
}

static test_suite_t test_mutex_suite = {test_mutex_application,
                                        test_mutex_memory_system,
                                        test_mutex_config,
                                        test_mutex_declare,
                                        test_mutex_initialize,
                                        test_mutex_finalize,
                                        0};

#if BUILD_MONOLITHIC

int
test_mutex_run(void);

int
test_mutex_run(void) {
	test_suite = test_mutex_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_mutex_suite;
}

#endif
