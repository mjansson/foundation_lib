/* main.c  -  Foundation hashmap test  -  Public Domain  -  2013 Mattias Jansson
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
test_hashmap_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation hashmap tests"));
	app.short_name = string_const(STRING_CONST("test_hashmap"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_hashmap_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_hashmap_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_hashmap_initialize(void) {
	return 0;
}

static void
test_hashmap_finalize(void) {
}

DECLARE_TEST(hashmap, allocation) {
	hashmap_t* map = hashmap_allocate(0, 0);

	EXPECT_EQ(hashmap_size(map), 0);
	EXPECT_EQ(hashmap_lookup(map, 0), 0);
	EXPECT_EQ(hashmap_lookup(map, (hash_t)(uintptr_t)map), 0);

	hashmap_deallocate(map);

	map = hashmap_allocate(13, 127);

	EXPECT_EQ(hashmap_size(map), 0);
	EXPECT_EQ(hashmap_lookup(map, 0), 0);
	EXPECT_EQ(hashmap_lookup(map, (hash_t)(uintptr_t)map), 0);

	hashmap_deallocate(map);

	return 0;
}

DECLARE_TEST(hashmap, insert) {
	hashmap_t* map = hashmap_allocate(0, 0);
	void* prev = 0;

	EXPECT_EQ(hashmap_lookup(map, 0), 0);

	prev = hashmap_insert(map, 0, map);
	EXPECT_EQ(prev, 0);

	prev = hashmap_insert(map, 0, map);
	EXPECT_EQ(prev, map);

	prev = hashmap_insert(map, 0, 0);
	EXPECT_EQ(prev, map);

	prev = hashmap_insert(map, 0, map);
	EXPECT_EQ(prev, 0);

	prev = hashmap_insert(map, (hash_t)(uintptr_t)map, map);
	EXPECT_EQ(prev, 0);

	EXPECT_EQ(hashmap_size(map), 2);
	EXPECT_EQ(hashmap_lookup(map, 0), map);
	EXPECT_EQ(hashmap_lookup(map, (hash_t)(uintptr_t)map), map);

	hashmap_insert(map, 0, 0);
	hashmap_insert(map, (hash_t)(uintptr_t)map, 0);

	EXPECT_EQ(hashmap_size(map), 2);
	EXPECT_EQ(hashmap_lookup(map, 0), 0);
	EXPECT_EQ(hashmap_lookup(map, (hash_t)(uintptr_t)map), 0);

	hashmap_deallocate(map);

	return 0;
}

DECLARE_TEST(hashmap, erase) {
	hashmap_t* map = hashmap_allocate(0, 0);
	void* prev = 0;

	EXPECT_EQ(hashmap_lookup(map, 0), 0);
	EXPECT_EQ(hashmap_size(map), 0);

	prev = hashmap_insert(map, 0, map);
	EXPECT_EQ(prev, 0);
	EXPECT_EQ(hashmap_size(map), 1);
	EXPECT_TRUE(hashmap_has_key(map, 0));

	prev = hashmap_erase(map, 0);
	EXPECT_EQ(prev, map);
	EXPECT_EQ(hashmap_size(map), 0);
	EXPECT_FALSE(hashmap_has_key(map, 0));

	prev = hashmap_erase(map, 0);
	EXPECT_EQ(prev, 0);
	EXPECT_EQ(hashmap_size(map), 0);
	EXPECT_FALSE(hashmap_has_key(map, 0));

	prev = hashmap_erase(map, (hash_t)(uintptr_t)map);
	EXPECT_EQ(prev, 0);
	EXPECT_EQ(hashmap_size(map), 0);
	EXPECT_FALSE(hashmap_has_key(map, (hash_t)(uintptr_t)map));

	hashmap_deallocate(map);

	return 0;
}

DECLARE_TEST(hashmap, lookup) {
	hashmap_t* map = hashmap_allocate(31, 0);
	char* value = (void*)(uintptr_t)1234;
	hash_t key = (hash_t)4321;
	unsigned int ikey = 0;

	for (; ikey < 1024; ++ikey, ++key, ++value) {
		void* prev = hashmap_insert(map, key, value);
		EXPECT_EQ(prev, 0);
	}

	for (ikey = 0, key = (hash_t)4321, value = (void*)(uintptr_t)1234; ikey < 1024; ++ikey, ++key, ++value) {
		void* prev = hashmap_lookup(map, key);
		EXPECT_EQ(prev, value);

		EXPECT_TRUE(hashmap_has_key(map, key));
		hashmap_erase(map, key);
		EXPECT_FALSE(hashmap_has_key(map, key));
	}

	hashmap_deallocate(map);

	return 0;
}

static void
test_hashmap_declare(void) {
	ADD_TEST(hashmap, allocation);
	ADD_TEST(hashmap, insert);
	ADD_TEST(hashmap, erase);
	ADD_TEST(hashmap, lookup);
}

static test_suite_t test_hashmap_suite = {test_hashmap_application,
                                          test_hashmap_memory_system,
                                          test_hashmap_config,
                                          test_hashmap_declare,
                                          test_hashmap_initialize,
                                          test_hashmap_finalize,
                                          0};

#if BUILD_MONOLITHIC

int
test_hashmap_run(void);

int
test_hashmap_run(void) {
	test_suite = test_hashmap_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_hashmap_suite;
}

#endif
