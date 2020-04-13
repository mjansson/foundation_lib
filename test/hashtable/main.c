/* main.c  -  Foundation hashtable test  -  Public Domain  -  2013 Mattias Jansson
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

/* Internal API */
FOUNDATION_API uint32_t
hashtable32_raw(hashtable32_t* table, size_t slot);

FOUNDATION_API uint64_t
hashtable64_raw(hashtable64_t* table, size_t slot);
/* End internal API */

static application_t
test_hashtable_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation hashtable tests"));
	app.short_name = string_const(STRING_CONST("test_hashtable"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_hashtable_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_hashtable_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_hashtable_initialize(void) {
	return 0;
}

static void
test_hashtable_finalize(void) {
}

typedef struct {
	hashtable32_t* table;
	uint32_t key_offset;
	uint32_t key_num;
} producer32_arg_t;

typedef struct {
	hashtable64_t* table;
	uint64_t key_offset;
	uint64_t key_num;
} producer64_arg_t;

static void*
producer32_thread(void* arg) {
	producer32_arg_t* parg = arg;
	hashtable32_t* table = parg->table;
	uint32_t key_offset = parg->key_offset;
	uint32_t key;

	for (key = 0; key < parg->key_num; ++key)
		hashtable32_set(table, 1 + key + key_offset, 1);

	thread_yield();

	for (key = 0; key < parg->key_num / 2; ++key)
		hashtable32_erase(table, 1 + key + key_offset);

	thread_yield();

	for (key = 0; key < parg->key_num; ++key)
		hashtable32_set(table, 1 + key + key_offset, 1 + ((key + key_offset) % 17));

	return 0;
}

static void*
producer64_thread(void* arg) {
	producer64_arg_t* parg = arg;
	hashtable64_t* table = parg->table;
	uint64_t key_offset = parg->key_offset;
	uint64_t key;

	for (key = 0; key < parg->key_num; ++key)
		hashtable64_set(table, 1 + key + key_offset, 1);

	thread_yield();

	for (key = 0; key < parg->key_num / 2; ++key)
		hashtable64_erase(table, 1 + key + key_offset);

	thread_yield();

	for (key = 0; key < parg->key_num; ++key)
		hashtable64_set(table, 1 + key + key_offset, 1 + ((key + key_offset) % 17));

	return 0;
}

DECLARE_TEST(hashtable, 32bit_basic) {
	hashtable32_t* table = hashtable32_allocate(3);

	EXPECT_SIZEEQ(hashtable32_size(table), 0);

	hashtable32_set(table, 1, 1);
	EXPECT_EQ(hashtable32_get(table, 1), 1);

	hashtable32_erase(table, 1);
	EXPECT_EQ(hashtable32_get(table, 1), 0);

	hashtable32_set(table, 1, 2);
	EXPECT_EQ(hashtable32_get(table, 1), 2);

	hashtable32_set(table, 1, 3);
	EXPECT_EQ(hashtable32_get(table, 1), 3);

	hashtable32_set(table, 2, 1);
	EXPECT_EQ(hashtable32_get(table, 2), 1);

	EXPECT_SIZEEQ(hashtable32_size(table), 2);

	hashtable32_erase(table, 1);
	EXPECT_EQ(hashtable32_get(table, 1), 0);
	EXPECT_EQ(hashtable32_get(table, 2), 1);

	EXPECT_SIZEEQ(hashtable32_size(table), 1);

	hashtable32_erase(table, 2);
	EXPECT_EQ(hashtable32_get(table, 2), 0);

	EXPECT_EQ(hashtable32_size(table), 0);

	EXPECT_TRUE(hashtable32_set(table, 1, 1));
	EXPECT_TRUE(hashtable32_set(table, 2, 2));
	EXPECT_TRUE(hashtable32_set(table, 3, 3));
	EXPECT_FALSE(hashtable32_set(table, 4, 4));
	EXPECT_EQ(hashtable32_size(table), 3);
	hashtable32_erase(table, 4);
	EXPECT_EQ(hashtable32_size(table), 3);
	EXPECT_EQ(hashtable32_get(table, 4), 0);

	// Hashing regression
	EXPECT_TYPEEQ(hashtable32_raw(table, 0), 3, uint64_t, PRIu64);
	EXPECT_TYPEEQ(hashtable32_raw(table, 1), 1, uint64_t, PRIu64);
	EXPECT_TYPEEQ(hashtable32_raw(table, 2), 2, uint64_t, PRIu64);
	hashtable32_erase(table, 3);
	EXPECT_TYPEEQ(hashtable32_raw(table, 0), 0, uint64_t, PRIu64);
	EXPECT_TYPEEQ(hashtable32_raw(table, 1), 1, uint64_t, PRIu64);
	EXPECT_TYPEEQ(hashtable32_raw(table, 2), 2, uint64_t, PRIu64);

	hashtable32_deallocate(table);

	return 0;
}

DECLARE_TEST(hashtable, 32bit_threaded) {
	thread_t thread[32];
	producer32_arg_t args[32];
	unsigned int i, j;
	size_t threads_count = 32;

	hashtable32_t* table = hashtable32_allocate(32 * 16789 + 65536);

	EXPECT_EQ(hashtable32_size(table), 0);

	threads_count = math_clamp(system_hardware_threads() * 2U, 4U, 30U);
	for (i = 0; i < threads_count; ++i) {
		args[i].table = table;
		args[i].key_offset = i * 16789;
		args[i].key_num = 65535;

		thread_initialize(&thread[i], producer32_thread, args + i, STRING_CONST("table_producer"),
		                  THREAD_PRIORITY_NORMAL, 0);
	}
	for (i = 0; i < threads_count; ++i)
		thread_start(&thread[i]);

	test_wait_for_threads_startup(thread, threads_count);
	test_wait_for_threads_finish(thread, threads_count);

	for (i = 0; i < threads_count; ++i)
		thread_finalize(&thread[i]);

	for (i = 0; i < threads_count; ++i) {
		for (j = 1; j < 65535; ++j) {
			uint32_t key = (i * 16789) + j;
			EXPECT_EQ(hashtable32_get(table, 1 + key), 1 + (key % 17));
		}
	}

	// Size is potentially greater due to threading, see comment in hashtable_set
	EXPECT_SIZEGE(hashtable32_size(table), (threads_count - 1) * 16789 + 65535);
	hashtable32_clear(table);
	EXPECT_SIZEEQ(hashtable32_size(table), 0);

	for (i = 0; i < threads_count; ++i) {
		for (j = 1; j < 65535; ++j) {
			uint32_t key = (1 + (i * 16789)) + j;
			EXPECT_EQ(hashtable32_get(table, 1 + key), 0);
		}
	}

	hashtable32_deallocate(table);

	return 0;
}

DECLARE_TEST(hashtable, 64bit_basic) {
	hashtable64_t* table = hashtable64_allocate(3);

	EXPECT_SIZEEQ(hashtable64_size(table), 0);

	hashtable64_set(table, 1, 1);
	EXPECT_EQ(hashtable64_get(table, 1), 1);

	hashtable64_erase(table, 1);
	EXPECT_EQ(hashtable64_get(table, 1), 0);

	hashtable64_set(table, 1, 2);
	EXPECT_EQ(hashtable64_get(table, 1), 2);

	hashtable64_set(table, 1, 3);
	EXPECT_EQ(hashtable64_get(table, 1), 3);

	hashtable64_set(table, 2, 1);
	EXPECT_EQ(hashtable64_get(table, 2), 1);

	EXPECT_SIZEEQ(hashtable64_size(table), 2);

	hashtable64_erase(table, 1);
	hashtable64_erase(table, 3);
	EXPECT_EQ(hashtable64_get(table, 1), 0);
	EXPECT_EQ(hashtable64_get(table, 2), 1);

	EXPECT_SIZEEQ(hashtable64_size(table), 1);

	hashtable64_erase(table, 2);
	EXPECT_EQ(hashtable64_get(table, 2), 0);

	EXPECT_SIZEEQ(hashtable64_size(table), 0);

	EXPECT_TRUE(hashtable64_set(table, 1, 1));
	EXPECT_TRUE(hashtable64_set(table, 2, 2));
	EXPECT_TRUE(hashtable64_set(table, 3, 3));
	EXPECT_FALSE(hashtable64_set(table, 4, 4));
	EXPECT_EQ(hashtable64_size(table), 3);
	hashtable64_erase(table, 4);
	EXPECT_EQ(hashtable64_size(table), 3);
	EXPECT_EQ(hashtable64_get(table, 4), 0);

	// Hashing regression
	EXPECT_TYPEEQ(hashtable64_raw(table, 0), 2, uint64_t, PRIu64);
	EXPECT_TYPEEQ(hashtable64_raw(table, 1), 3, uint64_t, PRIu64);
	EXPECT_TYPEEQ(hashtable64_raw(table, 2), 1, uint64_t, PRIu64);
	hashtable64_erase(table, 3);
	EXPECT_TYPEEQ(hashtable64_raw(table, 0), 2, uint64_t, PRIu64);
	EXPECT_TYPEEQ(hashtable64_raw(table, 1), 0, uint64_t, PRIu64);
	EXPECT_TYPEEQ(hashtable64_raw(table, 2), 1, uint64_t, PRIu64);

	hashtable64_deallocate(table);

	return 0;
}

DECLARE_TEST(hashtable, 64bit_threaded) {
	thread_t thread[32];
	producer64_arg_t args[32];
	unsigned int i, j;
	size_t threads_count = 0;

	hashtable64_t* table = hashtable64_allocate(32 * 16789 + 65536);

	EXPECT_EQ(hashtable64_size(table), 0);

	threads_count = math_clamp(system_hardware_threads() * 2U, 4U, 32U);
	for (i = 0; i < threads_count; ++i) {
		args[i].table = table;
		args[i].key_offset = (i * 16789);
		args[i].key_num = 65535;

		thread_initialize(&thread[i], producer64_thread, args + i, STRING_CONST("table_producer"),
		                  THREAD_PRIORITY_NORMAL, 0);
	}
	for (i = 0; i < threads_count; ++i)
		thread_start(&thread[i]);

	test_wait_for_threads_startup(thread, threads_count);
	test_wait_for_threads_finish(thread, threads_count);

	for (i = 0; i < threads_count; ++i)
		thread_finalize(&thread[i]);

	for (i = 0; i < threads_count; ++i) {
		for (j = 1; j < 65535; ++j) {
			uint32_t key = (i * 16789) + j;
			EXPECT_EQ(hashtable64_get(table, 1 + key), 1 + (key % 17));
		}
	}

	// Size is potentially greater due to threading, see comment in hashtable_set
	EXPECT_SIZEGE(hashtable64_size(table), (threads_count - 1) * 16789 + 65535);
	hashtable64_clear(table);
	EXPECT_SIZEEQ(hashtable64_size(table), 0);

	for (i = 0; i < threads_count; ++i) {
		for (j = 1; j < 65535; ++j) {
			uint32_t key = (1 + (i * 16789)) + j;
			EXPECT_EQ(hashtable64_get(table, key), 0);
		}
	}

	hashtable64_deallocate(table);

	return 0;
}

static void
test_hashtable_declare(void) {
	ADD_TEST(hashtable, 32bit_basic);
	ADD_TEST(hashtable, 32bit_threaded);
	ADD_TEST(hashtable, 64bit_basic);
	ADD_TEST(hashtable, 64bit_threaded);
}

static test_suite_t test_hashtable_suite = {test_hashtable_application,
                                            test_hashtable_memory_system,
                                            test_hashtable_config,
                                            test_hashtable_declare,
                                            test_hashtable_initialize,
                                            test_hashtable_finalize,
                                            0};

#if BUILD_MONOLITHIC

int
test_hashtable_run(void);

int
test_hashtable_run(void) {
	test_suite = test_hashtable_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_hashtable_suite;
}

#endif
