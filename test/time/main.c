/* main.c  -  Foundation time test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>
#include <test/test.h>


static application_t test_time_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = "Foundation time tests";
	app.short_name = "test_time";
	app.config_dir = "test_time";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_time_memory_system( void )
{
	return memory_system_malloc();
}


static int test_time_initialize( void )
{
	return 0;
}


static void test_time_shutdown( void )
{
}


DECLARE_TEST( time, builtin )
{
	tick_t tick, newtick, tps;
	deltatime_t dt;

	tps = time_ticks_per_second();
	EXPECT_GT( tps, 0 );

	tick = time_current();
	EXPECT_NE( tick, 0 );

	thread_sleep( 20 );
	newtick = time_current();
	EXPECT_GT( newtick, tick );

	EXPECT_GT( time_diff( tick, newtick ), 0 );
	EXPECT_GT_MSGFORMAT( time_diff( tick, newtick ), ( tps / 100 ), "time elapsed not more than 10ms: %d (%d)", time_diff( tick, newtick ), ( tps / 100 ) ); //more than 10 ms
	EXPECT_LT_MSGFORMAT( time_diff( tick, newtick ), ( tps / 33 ), "time elapsed not less than 33ms: %d (%d)", time_diff( tick, newtick ), ( tps / 33 ) ); //less than 30 ms
	EXPECT_GT( time_elapsed( tick ), 0 );
	EXPECT_GT( time_elapsed( tick ), 0.01f ); //more than 10 ms
	EXPECT_GT( time_elapsed_ticks( tick ), 0 );
	EXPECT_GT( time_elapsed_ticks( tick ), ( tps / 100 ) ); //more than 10 ms

	dt = time_ticks_to_seconds( newtick - tick );
	EXPECT_GT( dt, 0 );
	EXPECT_GT( dt, 0.01f ); //more than 10 ms
	EXPECT_LT( dt, 0.03f ); //less than 30 ms

	tick = time_startup();
	EXPECT_GT( tick, 0 );
	EXPECT_LT( tick, newtick );
	EXPECT_EQ( tick, time_startup() );

	tick = time_system();
	EXPECT_GT( tick, 0 );

	thread_sleep( 100 );

	newtick = time_system();
	EXPECT_GT( newtick, 0 );
	EXPECT_GT( newtick, tick );
	EXPECT_GT( newtick - tick, 50 ); //more than 50 ms
	EXPECT_LT( newtick - tick, 200 ); //less than 200 ms

	return 0;
}


static void test_time_declare( void )
{
	ADD_TEST( time, builtin );
}


test_suite_t test_time_suite = {
	test_time_application,
	test_time_memory_system,
	test_time_declare,
	test_time_initialize,
	test_time_shutdown
};


#if BUILD_MONOLITHIC

int test_time_run( void );
int test_time_run( void )
{
	test_suite = test_time_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_time_suite;
}

#endif
