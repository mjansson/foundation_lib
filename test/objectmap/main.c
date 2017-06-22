/* main.c  -  Foundation objectmap test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

static application_t
test_objectmap_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation objectmap tests"));
	app.short_name = string_const(STRING_CONST("test_objectmap"));
	app.company = string_const(STRING_CONST("Rampant Pixels"));
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
	object_base_t first;
	object_base_t second;
	object_base_t third;

	map = objectmap_allocate(129);
	
	memset(&first, 0, sizeof(first));
	memset(&second, 0, sizeof(first));
	memset(&third, 0, sizeof(first));
	atomic_store32(&first.ref, 1, memory_order_release);
	atomic_store32(&second.ref, 1, memory_order_release);
	atomic_store32(&third.ref, 1, memory_order_release);
	first.id = 1;
	second.id = 2;
	third.id = 3;

	EXPECT_EQ(objectmap_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, 1), 0);

	first.id = objectmap_reserve(map);
	EXPECT_EQ(objectmap_lookup(map, first.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);

	second.id = objectmap_reserve(map);
	EXPECT_EQ(objectmap_lookup(map, first.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	objectmap_set(map, first.id, &first);
	EXPECT_EQ(objectmap_lookup(map, first.id), &first);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), &first);
	EXPECT_EQ(objectmap_lookup(map, second.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	objectmap_set(map, second.id, &second);
	EXPECT_EQ(objectmap_lookup(map, first.id), &first);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), &first);
	EXPECT_EQ(objectmap_lookup(map, second.id), &second);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), &second);

	objectmap_free(map, first.id);
	EXPECT_EQ(objectmap_lookup(map, first.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second.id), &second);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), &second);

	objectmap_free(map, first.id);
	EXPECT_EQ(objectmap_lookup(map, first.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second.id), &second);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), &second);

	objectmap_free(map, second.id);
	EXPECT_EQ(objectmap_lookup(map, first.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	objectmap_deallocate(map);

	//Size should be clamped to three
	map = objectmap_allocate(1);

	EXPECT_EQ(objectmap_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, 1), 0);

	first.id = objectmap_reserve(map);
	EXPECT_TYPENE(first.id, 0, object_t, PRIx32);
	EXPECT_EQ(objectmap_lookup(map, first.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);

	second.id = objectmap_reserve(map);
	EXPECT_TYPENE(second.id, 0, object_t, PRIx32);
	EXPECT_EQ(objectmap_lookup(map, first.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	third.id = objectmap_reserve(map);
	EXPECT_TYPENE(third.id, 0, object_t, PRIx32);
	EXPECT_EQ(objectmap_lookup(map, first.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup(map, second.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);
	EXPECT_EQ(objectmap_lookup(map, third.id), 0);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);

	objectmap_set(map, first.id, &first);
	objectmap_set(map, second.id, &second);
	objectmap_set(map, third.id, &third);

	log_enable_stdout(false);
	EXPECT_TYPEEQ(objectmap_reserve(map), 0, object_t, PRIx32);
	EXPECT_TYPEEQ(objectmap_reserve(map), 0, object_t, PRIx32);
	log_enable_stdout(true);

	objectmap_free(map, first.id);
	objectmap_free(map, second.id);
	//Leak one object
	//objectmap_free(map, third.id);
	EXPECT_EQ(objectmap_lookup_ref(map, first.id), nullptr);
	EXPECT_EQ(objectmap_raw_lookup(map, 0), 0);
	EXPECT_EQ(objectmap_lookup_ref(map, second.id), nullptr);
	EXPECT_EQ(objectmap_raw_lookup(map, 1), 0);
	EXPECT_NE(objectmap_lookup_ref(map, third.id), nullptr);
	EXPECT_NE(objectmap_raw_lookup(map, 2), 0);

	log_enable_stdout(false);
	objectmap_deallocate(map);
	log_enable_stdout(true);

	return 0;
}

static void*
objectmap_thread(void* arg) {
	objectmap_t* map;
	object_base_t* objects;
	int obj;
	int loop;
	object_base_t* lookup;

	map = arg;
	objects = memory_allocate(0, sizeof(object_base_t) * 512, 16,
	                          MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

	thread_sleep(10);

	for (loop = 0; loop < 32; ++loop) {
		thread_yield();

		for (obj = 0; obj < 512; ++obj) {
			atomic_store32(&objects[obj].ref, 1, memory_order_release);
			objects[obj].id = objectmap_reserve(map);
			EXPECT_NE_MSGFORMAT(objects[obj].id, 0, "Unable to reserve slot for object num %d", obj);
			EXPECT_EQ_MSGFORMAT(objectmap_lookup(map, objects[obj].id), 0,
			                    "Object %d (%" PRIx32 ") already stored in map in loop %d",
			                    obj, objects[obj].id, loop);
			EXPECT_TRUE(objectmap_set(map, objects[obj].id, objects + obj));
			lookup = objectmap_lookup(map, objects[obj].id);
			EXPECT_NE_MSGFORMAT(lookup, 0, "Object num %d (%" PRIx32 ") not set in map, got null on lookup in loop %d",
			                    obj, objects[obj].id, loop);
			EXPECT_EQ_MSGFORMAT(lookup, objects + obj,
			                    "Object %d (%" PRIx32 ") 0x%" PRIfixPTR " was not set at reserved slot in map, got object 0x%"
			                    PRIfixPTR " in loop %d", obj, objects[obj].id, (uintptr_t)(objects + obj), (uintptr_t)lookup, loop);
		}

		thread_yield();

		for (obj = 0; obj < 512; ++obj) {
			void* raw = map->map[ objects[obj].id & map->mask_index ];
			lookup = objectmap_lookup(map, objects[obj].id);
			EXPECT_NE_MSGFORMAT(lookup, 0, "Object 0x%" PRIfixPTR " num %d (%" PRIx32 ") not set in map, got null on lookup in loop %d (raw 0x%" PRIfixPTR ")",
			                    (uintptr_t)(objects + obj), obj, objects[obj].id, loop, (uintptr_t)raw);
			EXPECT_EQ_MSGFORMAT(lookup, objects + obj,
			                    "Object %d (%" PRIx32 ") 0x%" PRIfixPTR " was not set at reserved slot in map, got object 0x%"
			                    PRIfixPTR " in loop %d", obj, objects[obj].id, (uintptr_t)(objects + obj), (uintptr_t)lookup, loop);
			EXPECT_TRUE(objectmap_free(map, objects[obj].id));
			lookup = objectmap_lookup(map, objects[obj].id);
			EXPECT_EQ_MSGFORMAT(lookup, 0,
			                    "Object %d (%" PRIx32 ") 0x%" PRIfixPTR " still set in map, got non-null (0x%" PRIfixPTR ") on lookup in loop %d", obj,
			                    objects[obj].id, (uintptr_t)(objects + obj), (uintptr_t)lookup, loop);
		}
	}

	memory_deallocate(objects);

	return 0;
}

DECLARE_TEST(objectmap, thread) {
	objectmap_t* map;
	thread_t thread[32];
	size_t ith;
	size_t num_threads = math_clamp(system_hardware_threads() * 4, 4, 32);

	map = objectmap_allocate((num_threads + 1) * 512);

	for (ith = 0; ith < num_threads; ++ith)
		thread_initialize(&thread[ith], objectmap_thread, map, STRING_CONST("objectmap_thread"),
		                  THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < num_threads; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, num_threads);
	test_wait_for_threads_finish(thread, num_threads);

	for (ith = 0; ith < num_threads; ++ith)
		EXPECT_EQ(thread[ith].result, 0);

	for (ith = 0; ith < num_threads; ++ith)
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

static test_suite_t test_objectmap_suite = {
	test_objectmap_application,
	test_objectmap_memory_system,
	test_objectmap_config,
	test_objectmap_declare,
	test_objectmap_initialize,
	test_objectmap_finalize,
	0
};

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
