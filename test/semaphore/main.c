/* main.c  -  Foundation semaphore test  -  Public Domain  -  2013 Mattias Jansson
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

#include <foundation/posix.h>

static application_t
test_semaphore_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation semaphore tests"));
	app.short_name = string_const(STRING_CONST("test_semaphore"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_semaphore_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_semaphore_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_semaphore_initialize(void) {
	return 0;
}

static void
test_semaphore_finalize(void) {
}

DECLARE_TEST(semaphore, initialize) {
	semaphore_t sem;

	semaphore_initialize(&sem, 0);
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));
	semaphore_finalize(&sem);

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_ANDROID
	semaphore_initialize_named(&sem, STRING_CONST("/rp-foundation-test"), 0);
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));
	semaphore_finalize(&sem);
#endif

	semaphore_initialize(&sem, 1);
	EXPECT_TRUE(semaphore_try_wait(&sem, 100));
	semaphore_post(&sem);  // Restored value
	semaphore_finalize(&sem);

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_ANDROID
	semaphore_initialize_named(&sem, STRING_CONST("/rp-foundation-test"), 1);
	EXPECT_TRUE(semaphore_try_wait(&sem, 100));
	semaphore_post(&sem);  // Restored value
	semaphore_finalize(&sem);
#endif

	semaphore_initialize(&sem, 2);
	EXPECT_TRUE(semaphore_wait(&sem));
	EXPECT_TRUE(semaphore_try_wait(&sem, 100));
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));
	semaphore_post(&sem);
	semaphore_post(&sem);  // Restored value
	semaphore_finalize(&sem);

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_ANDROID
	semaphore_initialize_named(&sem, STRING_CONST("/rp-foundation-test"), 2);
	EXPECT_TRUE(semaphore_wait(&sem));
	EXPECT_TRUE(semaphore_try_wait(&sem, 100));
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));
	semaphore_post(&sem);
	semaphore_post(&sem);  // Restored value
	semaphore_finalize(&sem);
#endif

	return 0;
}

DECLARE_TEST(semaphore, postwait) {
	semaphore_t sem;
	tick_t start, end;

	semaphore_initialize(&sem, 0);
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));

	semaphore_post(&sem);
	EXPECT_TRUE(semaphore_wait(&sem));
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));

	semaphore_post(&sem);
	semaphore_post(&sem);
	EXPECT_TRUE(semaphore_wait(&sem));
	EXPECT_TRUE(semaphore_try_wait(&sem, 100));
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));

	start = time_current();
	semaphore_try_wait(&sem, 0);
	end = time_current();
	EXPECT_LE(end - start, time_ticks_per_second() / 1000);

	start = time_current();
	semaphore_try_wait(&sem, 510);
	end = time_current();
	EXPECT_GE(end - start, time_ticks_per_second() / 2);

	semaphore_finalize(&sem);

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_ANDROID
	semaphore_initialize_named(&sem, STRING_CONST("/rp-foundation-test"), 0);
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));

	semaphore_post(&sem);
	EXPECT_TRUE(semaphore_wait(&sem));
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));

	semaphore_post(&sem);
	semaphore_post(&sem);
	EXPECT_TRUE(semaphore_wait(&sem));
	EXPECT_TRUE(semaphore_try_wait(&sem, 100));
	EXPECT_FALSE(semaphore_try_wait(&sem, 100));

	start = time_current();
	semaphore_try_wait(&sem, 0);
	end = time_current();
	EXPECT_LE(end - start, time_ticks_per_second() / 1000);

	start = time_current();
	semaphore_try_wait(&sem, 510);
	end = time_current();
	EXPECT_GE(end - start, time_ticks_per_second() / 2);

	semaphore_finalize(&sem);
#endif

	return 0;
}

typedef struct {
	semaphore_t read;
	semaphore_t write;
	int loopcount;
	atomic32_t counter;
} semaphore_test_t;

static void*
semaphore_waiter(void* arg) {
	semaphore_test_t* sem = arg;
	int loop;

	for (loop = 0; loop < sem->loopcount; ++loop) {
		thread_yield();
		semaphore_wait(&sem->read);
		atomic_incr32(&sem->counter, memory_order_relaxed);
		semaphore_post(&sem->write);
	}

	return 0;
}

DECLARE_TEST(semaphore, threaded) {
	thread_t thread[32];
	int ith;
	int failed_waits;

	semaphore_test_t test;
	semaphore_initialize(&test.read, 0);
	semaphore_initialize(&test.write, 0);
	test.loopcount = 128;
	atomic_store32(&test.counter, 0, memory_order_relaxed);

	for (ith = 0; ith < 32; ++ith)
		thread_initialize(&thread[ith], semaphore_waiter, &test, STRING_CONST("semaphore_waiter"),
		                  THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < 32; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, 32);

	failed_waits = 0;
	for (ith = 0; ith < test.loopcount * 32; ++ith) {
		semaphore_post(&test.read);
		thread_yield();
		if (!semaphore_try_wait(&test.write, 200)) {
			failed_waits++;
			EXPECT_TRUE(semaphore_wait(&test.write));
		}
	}

	test_wait_for_threads_finish(thread, 32);

	for (ith = 0; ith < 32; ++ith)
		thread_finalize(&thread[ith]);

	EXPECT_EQ(atomic_load32(&test.counter, memory_order_acquire), test.loopcount * 32);
	EXPECT_EQ(failed_waits, 0);

	semaphore_finalize(&test.read);
	semaphore_finalize(&test.write);

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_ANDROID
	semaphore_initialize_named(&test.read, STRING_CONST("foundation_test_read"), 0);
	semaphore_initialize_named(&test.write, STRING_CONST("foundation_test_write"), 0);
	test.loopcount = 128;
	atomic_store32(&test.counter, 0, memory_order_relaxed);

	for (ith = 0; ith < 32; ++ith)
		thread_initialize(&thread[ith], semaphore_waiter, &test, STRING_CONST("semaphore_waiter"),
		                  THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < 32; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, 32);

	failed_waits = 0;
	for (ith = 0; ith < test.loopcount * 32; ++ith) {
		semaphore_post(&test.read);
		thread_yield();
		if (!semaphore_try_wait(&test.write, 200)) {
			failed_waits++;
			EXPECT_TRUE(semaphore_wait(&test.write));
		}
	}

	test_wait_for_threads_finish(thread, 32);

	EXPECT_EQ(atomic_load32(&test.counter, memory_order_acquire), test.loopcount * 32);
	EXPECT_EQ(failed_waits, 0);

	for (ith = 0; ith < 32; ++ith)
		thread_finalize(&thread[ith]);

	semaphore_finalize(&test.read);
	semaphore_finalize(&test.write);
#endif

	return 0;
}

DECLARE_TEST(semaphore, failure) {
	semaphore_t sem;

	error_level_t last_log_suppress = log_suppress(0);
	log_set_suppress(0, ERRORLEVEL_ERROR);

#if FOUNDATION_PLATFORM_POSIX && !FOUNDATION_PLATFORM_APPLE && !FOUNDATION_PLATFORM_ANDROID

	sem_init_mock(-1, EINVAL);
	EXPECT_FALSE(semaphore_initialize(&sem, 0));
	EXPECT_EQ(error(), ERROR_SYSTEM_CALL_FAIL);
	semaphore_finalize(&sem);
	sem_init_unmock();

#elif FOUNDATION_PLATFORM_APPLE

	dispatch_semaphore_create_mock(0, EINVAL);
	EXPECT_FALSE(semaphore_initialize(&sem, 0));
	EXPECT_EQ(error(), ERROR_SYSTEM_CALL_FAIL);
	semaphore_finalize(&sem);
	dispatch_semaphore_create_unmock();

#endif

#if FOUNDATION_PLATFORM_POSIX && !FOUNDATION_PLATFORM_IOS

	sem_open_mock(SEM_FAILED, EINVAL);
	EXPECT_FALSE(semaphore_initialize_named(&sem, STRING_CONST("/rp-foundation-test"), 0));
	EXPECT_EQ(error(), ERROR_SYSTEM_CALL_FAIL);
	semaphore_finalize(&sem);
	sem_open_unmock();

#endif

#if FOUNDATION_PLATFORM_WINDOWS
	FOUNDATION_UNUSED(sem);
#endif

	log_set_suppress(0, last_log_suppress);

	return 0;
}

static void
test_semaphore_declare(void) {
	ADD_TEST(semaphore, initialize);
	ADD_TEST(semaphore, postwait);
	ADD_TEST(semaphore, threaded);
	ADD_TEST(semaphore, failure);
}

static test_suite_t test_semaphore_suite = {test_semaphore_application,
                                            test_semaphore_memory_system,
                                            test_semaphore_config,
                                            test_semaphore_declare,
                                            test_semaphore_initialize,
                                            test_semaphore_finalize,
                                            0};

#if BUILD_MONOLITHIC

int
test_semaphore_run(void);

int
test_semaphore_run(void) {
	test_suite = test_semaphore_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_semaphore_suite;
}

#endif
