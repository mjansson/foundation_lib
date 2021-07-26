/* main.c  -  Foundation time test  -  Public Domain  -  2013 Mattias Jansson
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
test_time_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation time tests"));
	app.short_name = string_const(STRING_CONST("test_time"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_time_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_time_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_time_initialize(void) {
	return 0;
}

static void
test_time_finalize(void) {
}

DECLARE_TEST(time, builtin) {
	tick_t tick, newtick, tps;
	deltatime_t dt;

	tps = time_ticks_per_second();
	EXPECT_GT(tps, 0);

	tick = time_current();
	thread_sleep(30);
	newtick = time_current();

	EXPECT_TICKNE(tick, 0);
	EXPECT_TICKGT(newtick, tick);

	EXPECT_TICKGT(time_diff(tick, newtick), 0);
	EXPECT_GT_MSGFORMAT(time_diff(tick, newtick), (tps / 100LL),
	                    "time elapsed not more than 10ms: %" PRId64 " (%" PRId64 ")", time_diff(tick, newtick),
	                    (tps / 100));  // more than 10 ms
	EXPECT_LT_MSGFORMAT(time_diff(tick, newtick), (tps / 5LL),
	                    "time elapsed not less than 200ms: %" PRId64 " (%" PRId64 ")", time_diff(tick, newtick),
	                    (tps / 5));  // less than 200 ms
	EXPECT_REALGT(time_elapsed(tick), 0);
	EXPECT_REALGT(time_elapsed(tick), 0.01f);  // more than 10 ms
	EXPECT_REALLT(time_elapsed(tick), 0.2f);   // less than 200 ms
	EXPECT_TICKGT(time_elapsed_ticks(tick), 0);
	EXPECT_TICKGT(time_elapsed_ticks(tick), (tps / 100LL));  // more than 10 ms
	EXPECT_TICKLT(time_elapsed_ticks(tick), (tps / 5LL));    // less than 200 ms

	dt = time_ticks_to_seconds(newtick - tick);
	EXPECT_REALGT(dt, 0);
	EXPECT_GT_MSGFORMAT(dt, 0.01f, "time elapsed in seconds not more than 10ms: %.5f",
	                    dt);  // more than 10 ms
	EXPECT_LT_MSGFORMAT(dt, 0.2f, "time elapsed in seconds not less than 200ms: %.5f",
	                    dt);  // less than 30 ms

	tick = time_startup();
	EXPECT_TICKGT(tick, 0);
	EXPECT_TICKLT(tick, newtick);
	EXPECT_TICKEQ(tick, time_startup());

	tick = time_system();
	thread_sleep(100);
	newtick = time_system();

	EXPECT_TICKGT(tick, 0);
	EXPECT_TICKGT(newtick, 0);
	EXPECT_TICKGT(newtick, tick);
	EXPECT_GT_MSGFORMAT(newtick - tick, 50, "Elapsed system time less than 50ms, expected 100ms, got %" PRId64 "ms",
	                    newtick - tick);
	EXPECT_LT_MSGFORMAT(newtick - tick, 500, "Elapsed system time more than 500ms, expected 100ms, got %" PRId64 "ms",
	                    newtick - tick);

	return 0;
}

static void
test_time_declare(void) {
	ADD_TEST(time, builtin);
}

static test_suite_t test_time_suite = {test_time_application,
                                       test_time_memory_system,
                                       test_time_config,
                                       test_time_declare,
                                       test_time_initialize,
                                       test_time_finalize,
                                       0};

#if BUILD_MONOLITHIC

int
test_time_run(void);

int
test_time_run(void) {
	test_suite = test_time_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_time_suite;
}

#endif
