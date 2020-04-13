/* main.c  -  Foundation objectmap test  -  Public Domain  -  2013 Mattias Jansson
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
test_objectmap_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation objectmap tests"));
	app.short_name = string_const(STRING_CONST("test_objectmap"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_objectmap_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_objectmap_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_objectmap_initialize(void) {
	return 0;
}

static void
test_objectmap_finalize(void) {
}

DECLARE_TEST(objectmap, initialize) {
	objectmap_t* map;

	map = objectmap_allocate(129);
	EXPECT_EQ(objectmap_size(map), 129);
	EXPECT_EQ(objectmap_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, 1), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	objectmap_deallocate(map);

	return 0;
}

DECLARE_TEST(objectmap, store) {
	objectmap_t* map;
	void* first = (void*)0x00010001;
	void* second = (void*)0x00220022;
	void* third = (void*)0x03330333;
	object_t first_id, second_id, third_id;

	map = objectmap_allocate(129);

	EXPECT_EQ(objectmap_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, 1), 0);

	first_id = objectmap_reserve(map);
	EXPECT_EQ(objectmap_lookup(map, first_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);

	second_id = objectmap_reserve(map);
	EXPECT_EQ(objectmap_lookup(map, first_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	EXPECT_TRUE(objectmap_set(map, first_id, first));
	EXPECT_EQ(objectmap_lookup(map, first_id), first);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), first);
	EXPECT_EQ(objectmap_lookup(map, second_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	EXPECT_TRUE(objectmap_set(map, second_id, second));
	EXPECT_EQ(objectmap_lookup(map, first_id), first);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), first);
	EXPECT_EQ(objectmap_lookup(map, second_id), second);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), second);

	EXPECT_TRUE(objectmap_free(map, first_id));
	EXPECT_EQ(objectmap_lookup(map, first_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second_id), second);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), second);

	EXPECT_FALSE(objectmap_free(map, first_id));
	EXPECT_EQ(objectmap_lookup(map, first_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second_id), second);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), second);

	EXPECT_TRUE(objectmap_free(map, second_id));
	EXPECT_EQ(objectmap_lookup(map, first_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	objectmap_deallocate(map);

	// Size should be clamped to three
	map = objectmap_allocate(1);

	EXPECT_EQ(objectmap_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, 1), 0);

	first_id = objectmap_reserve(map);
	EXPECT_TYPENE(first_id, 0, object_t, PRIx32);
	EXPECT_EQ(objectmap_lookup(map, first_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);

	second_id = objectmap_reserve(map);
	EXPECT_TYPENE(second_id, 0, object_t, PRIx32);
	EXPECT_EQ(objectmap_lookup(map, first_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	third_id = objectmap_reserve(map);
	EXPECT_TYPENE(third_id, 0, object_t, PRIx32);
	EXPECT_EQ(objectmap_lookup(map, first_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);
	EXPECT_EQ(objectmap_lookup(map, third_id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	EXPECT_TRUE(objectmap_set(map, first_id, first));
	EXPECT_TRUE(objectmap_set(map, second_id, second));
	EXPECT_TRUE(objectmap_set(map, third_id, third));

	log_enable_stdout(false);
	EXPECT_TYPEEQ(objectmap_reserve(map), 0, object_t, PRIx32);
	EXPECT_TYPEEQ(objectmap_reserve(map), 0, object_t, PRIx32);
	log_enable_stdout(true);

	objectmap_free(map, first_id);
	objectmap_free(map, second_id);
	// Leak one object
	// objectmap_free(map, third_id);
	EXPECT_EQ(objectmap_acquire(map, first_id), nullptr);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_acquire(map, second_id), nullptr);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);
	EXPECT_NE(objectmap_acquire(map, third_id), nullptr);
	EXPECT_NE(objectmap_raw_lookup(map, 2), 0);

	log_enable_stdout(false);
	objectmap_deallocate(map);
	log_enable_stdout(true);

#if BUILD_DEBUG || BUILD_RELEASE
	EXPECT_EQ(error(), ERROR_MEMORY_LEAK);
#endif

	return 0;
}

// Avoid powers of two to avoid threads perfectly looping tag numbers in object IDs
// causing one threaad to quickly reuse packed ID (tag + index) from another thread just freeing it
#define OBJECTS_PER_THREAD 137

static void*
objectmap_thread(void* arg) {
	objectmap_t* map;
	void** objects;
	object_t* object_ids;
	int obj;
	int loop;
	void* lookup;

	map = arg;
	objects = memory_allocate(0, sizeof(void*) * 512, 16, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	object_ids = memory_allocate(0, sizeof(object_t) * 512, 16, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

	thread_sleep(10);

	for (loop = 0; loop < 32; ++loop) {
		for (obj = 0; obj < OBJECTS_PER_THREAD; ++obj) {
			if (!loop)
				objects[obj] = (void*)((uintptr_t)random64());
			object_ids[obj] = objectmap_reserve(map);
			EXPECT_NE_MSGFORMAT(object_ids[obj], 0, "Unable to reserve slot for object num %d", obj);
			EXPECT_EQ_MSGFORMAT(objectmap_lookup(map, object_ids[obj]), 0,
			                    "Object %d (%" PRIx32 ") already stored in map in loop %d", obj, object_ids[obj], loop);
			EXPECT_TRUE(objectmap_set(map, object_ids[obj], objects[obj]));
			lookup = objectmap_lookup(map, object_ids[obj]);
			EXPECT_NE_MSGFORMAT(lookup, 0, "Object num %d (%" PRIx32 ") not set in map, got null on lookup in loop %d",
			                    obj, object_ids[obj], loop);
			EXPECT_EQ_MSGFORMAT(lookup, objects[obj],
			                    "Object %d (%" PRIx32 ") 0x%" PRIfixPTR
			                    " was not set at reserved slot in map, got object 0x%" PRIfixPTR " in loop %d",
			                    obj, object_ids[obj], (uintptr_t)objects[obj], (uintptr_t)lookup, loop);
		}

		thread_yield();

		for (obj = 0; obj < OBJECTS_PER_THREAD; ++obj) {
			lookup = objectmap_lookup(map, object_ids[obj]);
			EXPECT_NE_MSGFORMAT(lookup, 0, "Object num %d (%" PRIx32 ") not set in map, got null on lookup in loop %d",
			                    obj, object_ids[obj], loop);
			EXPECT_EQ_MSGFORMAT(lookup, objects[obj],
			                    "Object %d (%" PRIx32 ") 0x%" PRIfixPTR
			                    " was not set at reserved slot in map, got object 0x%" PRIfixPTR " in loop %d",
			                    obj, object_ids[obj], (uintptr_t)objects[obj], (uintptr_t)lookup, loop);
		}

		thread_yield();

		for (obj = 0; obj < OBJECTS_PER_THREAD; ++obj) {
			void* raw = map->map[object_ids[obj] & OBJECTMAP_INDEXMASK].ptr;
			lookup = objectmap_lookup(map, object_ids[obj]);
			EXPECT_NE_MSGFORMAT(lookup, 0,
			                    "Object 0x%" PRIfixPTR " num %d (%" PRIx32
			                    ") not set in map, got null on lookup in loop %d (raw 0x%" PRIfixPTR ")",
			                    (uintptr_t)objects[obj], obj, object_ids[obj], loop, (uintptr_t)raw);
			EXPECT_EQ_MSGFORMAT(lookup, objects[obj],
			                    "Object %d (%" PRIx32 ") 0x%" PRIfixPTR
			                    " was not set at reserved slot in map, got object 0x%" PRIfixPTR " in loop %d",
			                    obj, object_ids[obj], (uintptr_t)objects[obj], (uintptr_t)lookup, loop);
			EXPECT_TRUE(objectmap_free(map, object_ids[obj]));
			lookup = objectmap_lookup(map, object_ids[obj]);
			EXPECT_EQ_MSGFORMAT(lookup, 0,
			                    "Object %d (%" PRIx32 ") 0x%" PRIfixPTR " still set in map, got non-null (0x%" PRIfixPTR
			                    ") on lookup in loop %d (map size %u, indexmask %x)",
			                    obj, object_ids[obj], (uintptr_t)objects[obj], (uintptr_t)lookup, loop, map->size,
			                    OBJECTMAP_INDEXMASK);
		}

		thread_sleep(30);
	}

	memory_deallocate(objects);
	memory_deallocate(object_ids);

	return 0;
}

DECLARE_TEST(objectmap, thread) {
	objectmap_t* map;
	thread_t thread[32];
	size_t ith;
	size_t threads_count = math_clamp(system_hardware_threads() + 2, 2, 8);

	map = objectmap_allocate(threads_count * OBJECTS_PER_THREAD);

	for (ith = 0; ith < threads_count; ++ith)
		thread_initialize(&thread[ith], objectmap_thread, map, STRING_CONST("objectmap_thread"), THREAD_PRIORITY_NORMAL,
		                  0);
	for (ith = 0; ith < threads_count; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, threads_count);
	test_wait_for_threads_finish(thread, threads_count);

	for (ith = 0; ith < threads_count; ++ith)
		EXPECT_EQ(thread[ith].result, 0);

	for (ith = 0; ith < threads_count; ++ith)
		thread_finalize(&thread[ith]);

	objectmap_deallocate(map);

	return 0;
}

static void
test_objectmap_declare(void) {
	ADD_TEST(objectmap, initialize);
	ADD_TEST(objectmap, store);
	ADD_TEST(objectmap, thread);
}

static test_suite_t test_objectmap_suite = {test_objectmap_application,
                                            test_objectmap_memory_system,
                                            test_objectmap_config,
                                            test_objectmap_declare,
                                            test_objectmap_initialize,
                                            test_objectmap_finalize,
                                            0};

#if BUILD_MONOLITHIC

int
test_objectmap_run(void);

int
test_objectmap_run(void) {
	test_suite = test_objectmap_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_objectmap_suite;
}

#endif
