/* main.c  -  Foundation profile test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

//Must be >30000 since we assume that in forced fail test
#define TEST_PROFILE_BUFFER_SIZE  1024*1024

static char*       _test_profile_buffer;
static size_t      _test_profile_offset;
static atomic32_t  _test_profile_output_counter;

static void
test_profile_output(void* buffer, size_t size) {
	FOUNDATION_UNUSED(buffer);
	FOUNDATION_UNUSED(size);
	atomic_incr32(&_test_profile_output_counter);
}

static application_t
test_profile_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation profile tests"));
	app.short_name = string_const(STRING_CONST("test_profile"));
	app.company = string_const(STRING_CONST("Rampant Pixels"));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static foundation_config_t
test_profile_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static memory_system_t
test_profile_memory_system(void) {
	return memory_system_malloc();
}

static int
test_profile_initialize(void) {
	profile_set_output(test_profile_output);

	_test_profile_buffer = memory_allocate(0, TEST_PROFILE_BUFFER_SIZE, 0, MEMORY_PERSISTENT);

	return 0;
}

static void
test_profile_finalize(void) {
	profile_set_output(0);
	memory_deallocate(_test_profile_buffer);
}

DECLARE_TEST(profile, initialize) {
	error_t err = error();

	_test_profile_offset = 0;
	atomic_store32(&_test_profile_output_counter, 0);

	profile_initialize(STRING_CONST("test_profile"), _test_profile_buffer, TEST_PROFILE_BUFFER_SIZE);
	profile_enable(true);

	profile_log(STRING_CONST("testing"));

	thread_sleep(1000);

	profile_enable(false);
	profile_finalize();

#if BUILD_ENABLE_PROFILE
	EXPECT_GT(atomic_load32(&_test_profile_output_counter), 0);
#else
	EXPECT_EQ(atomic_load32(&_test_profile_output_counter), 0);
#endif

	err = error();
	EXPECT_EQ(err, ERROR_NONE);

	return 0;
}

DECLARE_TEST(profile, output) {
	error_t err = error();

	_test_profile_offset = 0;
	atomic_store32(&_test_profile_output_counter, 0);

	profile_initialize(STRING_CONST("test_profile"), _test_profile_buffer, TEST_PROFILE_BUFFER_SIZE);
	profile_enable(true);

	profile_log(STRING_CONST("testing"));

	thread_sleep(1000);

	profile_enable(false);
	profile_finalize();

#if BUILD_ENABLE_PROFILE
	EXPECT_GT(atomic_load32(&_test_profile_output_counter), 0);
	//TODO: Implement parsing output results
#else
	EXPECT_EQ(atomic_load32(&_test_profile_output_counter), 0);
#endif

	err = error();
	EXPECT_EQ(err, ERROR_NONE);

	_test_profile_offset = 0;
	atomic_store32(&_test_profile_output_counter, 0);

	profile_initialize(STRING_CONST("test_profile"), _test_profile_buffer, TEST_PROFILE_BUFFER_SIZE);
	profile_enable(false);

	profile_log(STRING_CONST("testing again"));

	thread_sleep(1000);

	profile_enable(false);
	profile_finalize();

	EXPECT_EQ(atomic_load32(&_test_profile_output_counter), 0);

	err = error();
	EXPECT_EQ(err, ERROR_NONE);

	return 0;
}

static void*
_profile_fail_thread(void* arg) {
	FOUNDATION_UNUSED(arg);
	thread_sleep(10);

	while (!thread_try_wait(1)) {
		profile_log(STRING_CONST("Thread message"));

		profile_begin_block(STRING_CONST("Thread block"));
		{
			profile_update_block();

			profile_begin_block(STRING_CONST("Thread subblock"));
			{
				profile_log(STRING_CONST("Sub message"));

				profile_trylock(STRING_CONST("Trylock"));

				profile_lock(STRING_CONST("Trylock"));

				profile_wait(STRING_CONST("Wait"));
				profile_signal(STRING_CONST("Signal"));

				profile_unlock(STRING_CONST("Trylock"));

				profile_log(STRING_CONST("End sub"));

				thread_yield();
			}
			profile_end_block();
		}
		profile_end_block();
	}

	return 0;
}

DECLARE_TEST(profile, thread) {
	thread_t thread[32];
	int ith;
	uint64_t frame;
	error_t err = error();

	_test_profile_offset = 0;
	atomic_store32(&_test_profile_output_counter, 0);

	profile_initialize(STRING_CONST("test_profile"), _test_profile_buffer, 30000);
	profile_enable(true);
	profile_set_output_wait(1);

	log_enable_stdout(false);
	for (ith = 0; ith < 32; ++ith)
		thread_initialize(&thread[ith], _profile_fail_thread, 0, STRING_CONST("profile_thread"),
		                  THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < 32; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, 32);

	for (frame = 0; frame < 1000; ++frame) {
		thread_sleep(16);
		profile_end_frame(frame);
	}

	for (ith = 0; ith < 32; ++ith)
		thread_signal(&thread[ith]);

	test_wait_for_threads_finish(thread, 32);

	for (ith = 0; ith < 32; ++ith)
		thread_finalize(&thread[ith]);
	log_enable_stdout(true);

	err = error();
	thread_sleep(1000);

	profile_enable(false);
	profile_finalize();

#if BUILD_ENABLE_PROFILE
	EXPECT_INTGT(atomic_load32(&_test_profile_output_counter), 0);
	//TODO: Implement parsing output results
#else
	EXPECT_INTEQ(atomic_load32(&_test_profile_output_counter), 0);
#endif
	EXPECT_INTEQ(err, ERROR_NONE);

	return 0;
}

static stream_t* _profile_stream;
static atomic64_t _profile_generated_blocks;

static void
_profile_file_writer(void* buffer, size_t size) {
	if (_profile_stream)
		stream_write(_profile_stream, buffer, size);
}

static void*
_profile_stream_thread(void* arg) {
	FOUNDATION_UNUSED(arg);
	thread_yield();

	while (!thread_try_wait(4)) {
		profile_log(STRING_CONST("Thread message"));

		profile_begin_block(STRING_CONST("Thread block"));
		{
			profile_update_block();

			profile_begin_block(STRING_CONST("Thread subblock"));
			{
				profile_log(STRING_CONST("Sub message"));

				profile_trylock(STRING_CONST("Trylock"));
				profile_lock(STRING_CONST("Trylock"));

				profile_wait(STRING_CONST("Wait"));
				profile_signal(STRING_CONST("Signal"));

				thread_sleep(2);

				profile_unlock(STRING_CONST("Trylock"));

				profile_log(STRING_CONST("End sub"));
			}
			profile_end_block();

			profile_begin_block(STRING_CONST("Thread second subblock"));
			{
				profile_update_block();

				profile_begin_block(STRING_CONST("Thread subblock"));
				{
				}
				profile_end_block();
			}
			profile_end_block();

			profile_trylock(STRING_CONST("Trylock"));
			thread_sleep(1);

			profile_lock(STRING_CONST("Trylock"));
			thread_sleep(4);

			profile_unlock(STRING_CONST("Trylock"));
		}
		profile_end_block();

		atomic_add64(&_profile_generated_blocks, 14);
	}

	return 0;
}

DECLARE_TEST(profile, stream) {
	thread_t thread[32];
	int ith;
	uint64_t frame;
	string_t filename;

	error(); //Clear error

	filename = path_allocate_concat(STRING_ARGS(environment_temporary_directory()),
	                                STRING_CONST("test.profile"));
	//log_infof(HASH_TEST, STRING_CONST("Output to profile file: %.*s"), STRING_FORMAT(filename));
	fs_make_directory(STRING_ARGS(environment_temporary_directory()));
	_profile_stream = fs_open_file(STRING_ARGS(filename), STREAM_OUT | STREAM_BINARY);
	string_deallocate(filename.str);

	profile_initialize(STRING_CONST("test_profile"), _test_profile_buffer, TEST_PROFILE_BUFFER_SIZE);
	profile_set_output(_profile_file_writer);
	profile_set_output_wait(10);
	profile_enable(true);

	for (ith = 0; ith < 32; ++ith)
		thread_initialize(&thread[ith], _profile_stream_thread, 0, STRING_CONST("profile_thread"),
		                  THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < 32; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, 32);

	for (frame = 0; frame < 1000; ++frame) {
		thread_sleep(16);
		profile_log(
		    STRING_CONST("This is a really long profile log line that should break into multiple profile blocks automatically without causing any issues whatsoever if everything works as expected which it should or the code needs to be fixed"));
		profile_end_frame(frame++);
		if ((frame % 30) == 0) {
			profile_enable(false);
			thread_sleep(10);
			profile_enable(true);
		}
	}

	for (ith = 0; ith < 32; ++ith)
		thread_signal(&thread[ith]);

	test_wait_for_threads_finish(thread, 32);

	for (ith = 0; ith < 32; ++ith)
		thread_finalize(&thread[ith]);

	profile_end_frame(frame++);
	profile_set_output_wait(10000);

	thread_sleep(1000);

	profile_begin_block(STRING_CONST("Should be cleaned up"));
	profile_end_block();

	profile_enable(false);
	profile_finalize();

	error();

	stream_deallocate(_profile_stream);

//TODO: Validate that output is sane
	log_debugf(HASH_TEST, STRING_CONST("Generated %" PRId64 " blocks"),
	           atomic_load64(&_profile_generated_blocks));

	return 0;
}

static void
test_profile_declare(void) {
	ADD_TEST(profile, initialize);
	ADD_TEST(profile, output);
	ADD_TEST(profile, thread);
	ADD_TEST(profile, stream);
}

static test_suite_t test_profile_suite = {
	test_profile_application,
	test_profile_memory_system,
	test_profile_config,
	test_profile_declare,
	test_profile_initialize,
	test_profile_finalize,
	0
};

#if BUILD_MONOLITHIC

int
test_profile_run(void);

int
test_profile_run(void) {
	test_suite = test_profile_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_profile_suite;
}

#endif
