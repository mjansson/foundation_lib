/* main.c  -  Foundation atomic test  -  Public Domain  -  2013 Mattias Jansson
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
test_atomic_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation atomic tests"));
	app.short_name = string_const(STRING_CONST("test_atomic"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_atomic_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_atomic_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_atomic_initialize(void) {
	return 0;
}

static void
test_atomic_finalize(void) {
}

static atomic32_t val_32;
static atomic64_t val_64;
static atomicptr_t val_ptr;

static void*
inc_thread(void* arg) {
	int loop = 0;
	int icount = 0;
	FOUNDATION_UNUSED(arg);
	while (!thread_try_wait(0) && (loop < 65535)) {
		for (icount = 0; icount < 256; ++icount) {
			atomic_incr32(&val_32, memory_order_relaxed);
			atomic_incr64(&val_64, memory_order_relaxed);
		}

		++loop;
		thread_yield();
	}
	return 0;
}

static void*
dec_thread(void* arg) {
	int loop = 0;
	int icount = 0;
	FOUNDATION_UNUSED(arg);
	while (!thread_try_wait(0) && (loop < 65535)) {
		for (icount = 0; icount < 256; ++icount) {
			atomic_decr32(&val_32, memory_order_relaxed);
			atomic_decr64(&val_64, memory_order_relaxed);
		}

		++loop;
		thread_yield();
	}
	return 0;
}

static void*
add_thread(void* arg) {
	int loop = 0;
	int32_t icount = 0;
	FOUNDATION_UNUSED(arg);
	while (!thread_try_wait(0) && (loop < 65535)) {
		for (icount = 0; icount < 128; ++icount) {
			atomic_add32(&val_32, icount % 2 ? -icount : icount, memory_order_relaxed);
			atomic_exchange_and_add64(&val_64, icount % 2 ? -icount : icount, memory_order_relaxed);
		}
		for (icount = 0; icount < 128; ++icount) {
			atomic_exchange_and_add32(&val_32, icount % 2 ? icount : -icount, memory_order_relaxed);
			atomic_add64(&val_64, icount % 2 ? icount : -icount, memory_order_relaxed);
		}

		++loop;
		thread_yield();
	}
	return 0;
}

typedef struct {
	int32_t val_32;
	int64_t val_64;
	void* val_ptr;
} cas_value_t;

#define REFVAL32 0x00010002
#define REFVAL64 0x100000002LL
#define REFVALPTR 0

static void*
cas_thread(void* arg) {
	unsigned int loop = 0;
	cas_value_t val = *(cas_value_t*)arg;

	thread_sleep(10);

	while (!thread_try_wait(0) && (loop < 10000)) {
		while (!atomic_cas32(&val_32, val.val_32, REFVAL32, memory_order_release, memory_order_acquire))
			thread_yield();
		while (!atomic_cas32(&val_32, REFVAL32, val.val_32, memory_order_release, memory_order_acquire))
			thread_yield();
		while (!atomic_cas64(&val_64, val.val_64, REFVAL64, memory_order_release, memory_order_acquire))
			thread_yield();
		while (!atomic_cas64(&val_64, REFVAL64, val.val_64, memory_order_release, memory_order_acquire))
			thread_yield();
		while (!atomic_cas_ptr(&val_ptr, val.val_ptr, REFVALPTR, memory_order_release, memory_order_acquire))
			thread_yield();
		while (!atomic_cas_ptr(&val_ptr, REFVALPTR, val.val_ptr, memory_order_release, memory_order_acquire))
			thread_yield();

		++loop;
		if (!(loop % 100))
			thread_sleep(10);
		thread_sleep(1);
	}

	return 0;
}

DECLARE_TEST(atomic, incdec) {
	size_t threads_count = math_clamp(system_hardware_threads() * 2, 4, 32);
	size_t ithread;
	thread_t threads[32];

	atomic_store32(&val_32, 0, memory_order_release);
	atomic_store64(&val_64, 0, memory_order_release);

	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_initialize(&threads[ithread], ithread % 2 ? dec_thread : inc_thread, 0, ithread % 2 ? "dec" : "inc", 3,
		                  THREAD_PRIORITY_NORMAL, 0);
	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_start(&threads[ithread]);

	test_wait_for_threads_startup(threads, threads_count);
	test_wait_for_threads_finish(threads, threads_count);

	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_finalize(&threads[ithread]);

	EXPECT_INTEQ(atomic_load32(&val_32, memory_order_acquire), 0);
	EXPECT_TYPEEQ(atomic_load64(&val_64, memory_order_acquire), 0, int64_t, PRIi64);

	return 0;
}

DECLARE_TEST(atomic, add) {
	size_t threads_count = math_clamp(system_hardware_threads() * 2, 4, 32);
	size_t ithread;
	thread_t threads[32];

	atomic_store32(&val_32, 0, memory_order_release);
	atomic_store64(&val_64, 0, memory_order_release);

	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_initialize(&threads[ithread], add_thread, 0, STRING_CONST("add"), THREAD_PRIORITY_NORMAL, 0);
	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_start(&threads[ithread]);

	test_wait_for_threads_startup(threads, threads_count);
	test_wait_for_threads_finish(threads, threads_count);

	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_finalize(&threads[ithread]);

	EXPECT_INTEQ(atomic_load32(&val_32, memory_order_acquire), 0);
	EXPECT_TYPEEQ(atomic_load64(&val_64, memory_order_acquire), 0, int64_t, PRIi64);

	return 0;
}

DECLARE_TEST(atomic, cas) {
	size_t threads_count = math_clamp(system_hardware_threads() * 2, 4, 32);
	size_t ithread;
	thread_t threads[32];
	cas_value_t cas_values[32];

	atomic_store32(&val_32, REFVAL32, memory_order_release);
	atomic_store64(&val_64, REFVAL64, memory_order_release);
	atomic_store_ptr(&val_ptr, REFVALPTR, memory_order_release);

	for (ithread = 0; ithread < threads_count; ++ithread) {
		cas_values[ithread].val_32 = (int32_t)ithread + 1;
		cas_values[ithread].val_64 = (int64_t)ithread + 1;
		cas_values[ithread].val_ptr = (void*)((uintptr_t)ithread + 1);
		thread_initialize(&threads[ithread], cas_thread, &cas_values[ithread], STRING_CONST("cas"),
		                  THREAD_PRIORITY_NORMAL, 0);
	}
	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_start(&threads[ithread]);

	test_wait_for_threads_startup(threads, threads_count);
	test_wait_for_threads_finish(threads, threads_count);

	for (ithread = 0; ithread < threads_count; ++ithread)
		thread_finalize(&threads[ithread]);

	EXPECT_INTEQ(atomic_load32(&val_32, memory_order_acquire), REFVAL32);
	EXPECT_TYPEEQ(atomic_load64(&val_64, memory_order_acquire), REFVAL64, int64_t, PRIx64);
	EXPECT_EQ(atomic_load_ptr(&val_ptr, memory_order_acquire), REFVALPTR);

	return 0;
}

static void
test_atomic_declare(void) {
	ADD_TEST(atomic, incdec);
	ADD_TEST(atomic, add);
	ADD_TEST(atomic, cas);
}

static test_suite_t test_atomic_suite = {test_atomic_application,
                                         test_atomic_memory_system,
                                         test_atomic_config,
                                         test_atomic_declare,
                                         test_atomic_initialize,
                                         test_atomic_finalize,
                                         0};

#if BUILD_MONOLITHIC

int
test_atomic_run(void);

int
test_atomic_run(void) {
	test_suite = test_atomic_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_atomic_suite;
}

#endif
