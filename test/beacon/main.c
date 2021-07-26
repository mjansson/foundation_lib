/* main.c  -  Foundation beacon test  -  Public Domain  -  2013 Mattias Jansson
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
test_beacon_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation beacon tests"));
	app.short_name = string_const(STRING_CONST("test_beacon"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_beacon_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_beacon_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_beacon_initialize(void) {
	return 0;
}

static void
test_beacon_finalize(void) {
}

DECLARE_TEST(beacon, fire) {
	beacon_t* beacon;

	beacon = beacon_allocate();

	EXPECT_INTLT(beacon_try_wait(beacon, 0), 0);
	EXPECT_INTLT(beacon_try_wait(beacon, 500), 0);

	beacon_fire(beacon);

	EXPECT_INTEQ(beacon_try_wait(beacon, 500), 0);
	EXPECT_INTLT(beacon_try_wait(beacon, 0), 0);

	beacon_fire(beacon);
	beacon_fire(beacon);
	beacon_fire(beacon);
	beacon_fire(beacon);

	EXPECT_INTEQ(beacon_try_wait(beacon, 0), 0);
	EXPECT_INTLT(beacon_try_wait(beacon, 100), 0);

	beacon_deallocate(beacon);

	return 0;
}

DECLARE_TEST(beacon, multiwait) {
	beacon_t* beacon[2];
#if FOUNDATION_PLATFORM_WINDOWS
	semaphore_t semaphore;
#else
	stream_pipe_t pipe;
	char data[8] = {0};
#endif
	bool got_signal[3] = {false, false, false};
	int slot;

	beacon[0] = beacon_allocate();
	beacon[1] = beacon_allocate();
#if FOUNDATION_PLATFORM_WINDOWS
	semaphore_initialize(&semaphore, 0);
#else
	pipe_initialize(&pipe);
#endif

	EXPECT_INTEQ(beacon_add_beacon(beacon[0], beacon[1]), 1);
#if FOUNDATION_PLATFORM_WINDOWS
	EXPECT_INTEQ(beacon_add_handle(beacon[0], semaphore_event_handle(&semaphore)), 2);
#else
	EXPECT_INTEQ(beacon_add_fd(beacon[0], pipe_read_fd((stream_t*)&pipe)), 2);
#endif

	EXPECT_INTLT(beacon_try_wait(beacon[0], 0), 0);
	EXPECT_INTLT(beacon_try_wait(beacon[1], 500), 0);

	beacon_fire(beacon[0]);

	EXPECT_INTEQ(beacon_try_wait(beacon[0], 500), 0);
	EXPECT_INTLT(beacon_try_wait(beacon[0], 0), 0);

	beacon_fire(beacon[0]);
	beacon_fire(beacon[0]);
	beacon_fire(beacon[1]);
	beacon_fire(beacon[1]);

	EXPECT_INTEQ(beacon_try_wait(beacon[0], 0), 0);
	EXPECT_INTEQ(beacon_try_wait(beacon[0], 100), 1);
	EXPECT_INTEQ(beacon_try_wait(beacon[1], 0), 0);
	EXPECT_INTLT(beacon_try_wait(beacon[1], 100), 0);
	EXPECT_INTLT(beacon_try_wait(beacon[0], 100), 0);

	beacon_fire(beacon[1]);

	EXPECT_INTEQ(beacon_try_wait(beacon[0], 0), 1);
	EXPECT_INTEQ(beacon_try_wait(beacon[0], 100), 1);
	EXPECT_INTEQ(beacon_try_wait(beacon[1], 0), 0);
	EXPECT_INTLT(beacon_try_wait(beacon[0], 100), 0);
	EXPECT_INTLT(beacon_try_wait(beacon[1], 100), 0);

	beacon_fire(beacon[1]);
	beacon_fire(beacon[0]);
#if FOUNDATION_PLATFORM_WINDOWS
	semaphore_post(&semaphore);
#else
	stream_write((stream_t*)&pipe, data, sizeof(data));
#endif
	beacon_fire(beacon[1]);
	beacon_fire(beacon[0]);

	slot = beacon_try_wait(beacon[0], 0);
	EXPECT_INTGE(slot, 0);
	EXPECT_INTLE(slot, 2);
	got_signal[slot] = true;

	slot = beacon_try_wait(beacon[0], 100);
	EXPECT_INTGE(slot, 0);
	EXPECT_INTLE(slot, 2);
	got_signal[slot] = true;

	EXPECT_INTEQ(beacon_try_wait(beacon[1], 0), 0);
	EXPECT_INTLT(beacon_try_wait(beacon[1], 100), 0);

	slot = beacon_try_wait(beacon[0], 100);
	EXPECT_INTGE(slot, 0);
	EXPECT_INTLE(slot, 2);
	got_signal[slot] = true;

	EXPECT_TRUE(got_signal[0]);
	EXPECT_TRUE(got_signal[1]);
	EXPECT_TRUE(got_signal[2]);

#if FOUNDATION_PLATFORM_WINDOWS
	semaphore_try_wait(&semaphore, 0);
#else
	stream_read((stream_t*)&pipe, data, sizeof(data));
#endif

	EXPECT_INTLT(beacon_try_wait(beacon[1], 100), 0);
	EXPECT_INTLT(beacon_try_wait(beacon[0], 100), 0);

	beacon_remove_beacon(beacon[0], beacon[1]);
	beacon_fire(beacon[1]);

	EXPECT_INTLT(beacon_try_wait(beacon[0], 100), 0);
	EXPECT_INTEQ(beacon_try_wait(beacon[1], 100), 0);

#if FOUNDATION_PLATFORM_WINDOWS
	semaphore_post(&semaphore);
#else
	stream_write((stream_t*)&pipe, data, sizeof(data));
#endif

	EXPECT_INTEQ(beacon_try_wait(beacon[0], 100), 1);
	EXPECT_INTLT(beacon_try_wait(beacon[1], 100), 0);

	beacon_remove_beacon(beacon[0], beacon[1]);

	beacon_deallocate(beacon[0]);
	beacon_deallocate(beacon[1]);
#if FOUNDATION_PLATFORM_WINDOWS
	semaphore_finalize(&semaphore);
#else
	stream_finalize((stream_t*)&pipe);
#endif
	return 0;
}

static void
test_beacon_declare(void) {
	ADD_TEST(beacon, fire);
	ADD_TEST(beacon, multiwait);
}

static test_suite_t test_beacon_suite = {test_beacon_application,
                                         test_beacon_memory_system,
                                         test_beacon_config,
                                         test_beacon_declare,
                                         test_beacon_initialize,
                                         test_beacon_finalize,
                                         0};

#if BUILD_MONOLITHIC

int
test_beacon_run(void);

int
test_beacon_run(void) {
	test_suite = test_beacon_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_beacon_suite;
}

#endif
