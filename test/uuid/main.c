/* main.c  -  Foundation uuid test  -  Public Domain  -  2013 Mattias Jansson
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
test_uuid_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation uuid tests"));
	app.short_name = string_const(STRING_CONST("test_uuid"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_uuid_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_uuid_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_uuid_initialize(void) {
	return 0;
}

static void
test_uuid_finalize(void) {
}

DECLARE_TEST(uuid, generate) {
	int iloop;
	uuid_t uuid, uuid_ref;
	char name_str[] = "com.maniccoder.foundation.uuid.000000";

	uuid = uuid_null();
	uuid_ref = uuid_null();

	EXPECT_TRUE(uuid_is_null(uuid));
	EXPECT_TRUE(uuid_is_null(uuid_ref));
	EXPECT_TRUE(uuid_equal(uuid, uuid_ref));

	// Random based
	uuid = uuid_generate_random();
	uuid_ref = uuid_null();

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_TRUE(uuid_is_null(uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	uuid = uuid_generate_random();
	uuid_ref = uuid_generate_random();

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_FALSE(uuid_is_null(uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	uuid = uuid_ref;

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_FALSE(uuid_is_null(uuid_ref));
	EXPECT_TRUE(uuid_equal(uuid, uuid_ref));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	for (iloop = 0; iloop < 64000; ++iloop) {
		uuid_ref = uuid;
		uuid = uuid_generate_random();

		EXPECT_FALSE(uuid_is_null(uuid));
		EXPECT_FALSE(uuid_is_null(uuid_ref));
		EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
		EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
		EXPECT_TRUE(uuid_equal(uuid, uuid));
		EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));
	}

	// Time based
	uuid = uuid_generate_time();
	uuid_ref = uuid_null();

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_TRUE(uuid_is_null(uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	uuid = uuid_generate_time();
	uuid_ref = uuid_generate_time();

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_FALSE(uuid_is_null(uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	uuid = uuid_ref;

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_FALSE(uuid_is_null(uuid_ref));
	EXPECT_TRUE(uuid_equal(uuid, uuid_ref));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	for (iloop = 0; iloop < 64000; ++iloop) {
		uuid_ref = uuid;
		uuid = uuid_generate_time();

		EXPECT_FALSE(uuid_is_null(uuid));
		EXPECT_FALSE(uuid_is_null(uuid_ref));
		EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
		EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
		EXPECT_TRUE(uuid_equal(uuid, uuid));
		EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));
	}

	// Name based
	uuid = uuid_generate_name(UUID_DNS, STRING_CONST("com.maniccoder.foundation.uuid"));
	uuid_ref = uuid_null();

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_TRUE(uuid_is_null(uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	uuid = uuid_generate_name(UUID_DNS, STRING_CONST("com.maniccoder.foundation.uuid.1"));
	uuid_ref = uuid_generate_name(UUID_DNS, STRING_CONST("com.maniccoder.foundation.uuid.2"));

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_FALSE(uuid_is_null(uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
	EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	uuid = uuid_generate_name(UUID_DNS, STRING_CONST("com.maniccoder.foundation.uuid.2"));

	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_FALSE(uuid_is_null(uuid_ref));
	EXPECT_TRUE(uuid_equal(uuid, uuid_ref));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuid));
	EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));

	for (iloop = 0; iloop < 10000; ++iloop) {
		string_t str = string_format(name_str, 40, STRING_CONST("com.maniccoder.foundation.uuid.%05u"), iloop);

		uuid_ref = uuid;
		uuid = uuid_generate_name(UUID_DNS, str.str, str.length);

		EXPECT_FALSE(uuid_is_null(uuid));
		EXPECT_FALSE(uuid_is_null(uuid_ref));
		EXPECT_FALSE(uuid_equal(uuid, uuid_ref));
		EXPECT_FALSE(uuid_equal(uuid_ref, uuid));
		EXPECT_TRUE(uuid_equal(uuid, uuid));
		EXPECT_TRUE(uuid_equal(uuid_ref, uuid_ref));
	}

	return 0;
}

#define UUIDS_COUNT 4096
static uuid_t uuid_thread_store[32][UUIDS_COUNT];

static void*
uuid_thread_time(void* arg) {
	int i;
	int ithread = (int)(uintptr_t)arg;

	for (i = 0; i < UUIDS_COUNT; ++i)
		uuid_thread_store[ithread][i] = uuid_generate_time();

	return 0;
}

DECLARE_TEST(uuid, threaded) {
	thread_t thread[32];
	size_t ith, i, jth, j;
	size_t threads_count = math_clamp(system_hardware_threads() * 2, 4, 8);

	for (ith = 0; ith < threads_count; ++ith)
		thread_initialize(&thread[ith], uuid_thread_time, (void*)(uintptr_t)ith, STRING_CONST("uuid_thread"),
		                  THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < threads_count; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, threads_count);
	test_wait_for_threads_finish(thread, threads_count);

	for (ith = 0; ith < threads_count; ++ith)
		thread_finalize(&thread[ith]);

	for (ith = 0; ith < threads_count; ++ith) {
		for (i = 0; i < UUIDS_COUNT; ++i) {
			for (jth = ith + 1; jth < threads_count; ++jth) {
				for (j = 0; j < UUIDS_COUNT; ++j) {
					EXPECT_FALSE(uuid_equal(uuid_thread_store[ith][i], uuid_thread_store[jth][j]));
				}
			}
			for (j = i + 1; j < UUIDS_COUNT; ++j) {
				EXPECT_FALSE(uuid_equal(uuid_thread_store[ith][i], uuid_thread_store[ith][j]));
			}
		}
	}

	return 0;
}

DECLARE_TEST(uuid, string) {
	uuid_t uuid, uuidref;
	string_t str;
	string_const_t strref;
	char buffer[128];

	uuidref = uuid_generate_random();
	EXPECT_FALSE(uuid_is_null(uuidref));

	str = string_from_uuid(buffer, sizeof(buffer), uuidref);
	EXPECT_NE(str.str, 0);
	EXPECT_GT(str.length, 0);

	uuid = string_to_uuid(STRING_ARGS(str));
	EXPECT_FALSE(uuid_is_null(uuid));
	EXPECT_TRUE(uuid_equal(uuid, uuidref));

	uuid = string_to_uuid("", 0);
	strref = string_from_uuid_static(uuid);
	EXPECT_EQ_MSGFORMAT(uuid_is_null(uuid), true, "empty string did not convert to null uuid: %.*s",
	                    STRING_FORMAT(strref));

	uuid = string_to_uuid("0", 1);
	strref = string_from_uuid_static(uuid);
	EXPECT_EQ_MSGFORMAT(uuid_is_null(uuid), true, "\"0\" string did not convert to null uuid: %.*s",
	                    STRING_FORMAT(strref));

	strref = string_from_uuid_static(uuid_null());
	uuid = string_to_uuid(STRING_ARGS(strref));
	strref = string_from_uuid_static(uuid);
	EXPECT_EQ_MSGFORMAT(uuid_is_null(uuid), true,
	                    "null uuid reconvert through string did not convert to null uuid: %.*s", STRING_FORMAT(strref));

	return 0;
}

static void
test_uuid_declare(void) {
	ADD_TEST(uuid, generate);
	ADD_TEST(uuid, threaded);
	ADD_TEST(uuid, string);
}

static test_suite_t test_uuid_suite = {test_uuid_application,
                                       test_uuid_memory_system,
                                       test_uuid_config,
                                       test_uuid_declare,
                                       test_uuid_initialize,
                                       test_uuid_finalize,
                                       0};

#if BUILD_MONOLITHIC

int
test_uuid_run(void);

int
test_uuid_run(void) {
	test_suite = test_uuid_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_uuid_suite;
}

#endif
