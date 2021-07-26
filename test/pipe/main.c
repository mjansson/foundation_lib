/* main.c  -  Foundation pipe test  -  Public Domain  -  2013 Mattias Jansson
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
test_pipe_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation pipe tests"));
	app.short_name = string_const(STRING_CONST("test_pipe"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_pipe_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_pipe_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_pipe_initialize(void) {
	return 0;
}

static void
test_pipe_finalize(void) {
}

static void*
read_thread(void* arg) {
	stream_t* pipe = arg;
	int i;
	unsigned char dest_buffer[256];
	uint64_t was_read, was_write;

	for (i = 0; i < 64; ++i) {
		was_read = stream_read(pipe, dest_buffer + i * 4, 4);
		EXPECT_EQ(was_read, 4);
	}
	for (i = 0; i < 256; ++i)
		EXPECT_EQ(dest_buffer[i], (unsigned char)i);

	for (i = 0; i < 64; ++i) {
		was_write = stream_write(pipe, dest_buffer + i * 4, 4);
		EXPECT_EQ(was_write, 4);
	}
	thread_sleep(2000);

	memset(dest_buffer, 0, 256);
	for (i = 0; i < 64; ++i) {
		was_read = stream_read(pipe, dest_buffer + i * 4, 4);
		EXPECT_EQ(was_read, 4);
	}
	for (i = 0; i < 256; ++i)
		EXPECT_EQ(dest_buffer[i], (unsigned char)i);

	for (i = 0; i < 64; ++i) {
		was_write = stream_write(pipe, dest_buffer + i * 4, 4);
		EXPECT_EQ(was_write, 4);
	}

	return 0;
}

static void*
write_thread(void* arg) {
	stream_t* pipe = arg;
	unsigned char src_buffer[256];
	int i;
	uint64_t was_read, was_write;

	for (i = 0; i < 256; ++i)
		src_buffer[i] = (unsigned char)i;
	was_write = stream_write(pipe, src_buffer, 69);
	EXPECT_EQ(was_write, 69);
	thread_sleep(100);
	was_write = stream_write(pipe, src_buffer + 69, 256 - 69);
	EXPECT_EQ(was_write, 256 - 69);
	thread_sleep(1000);

	memset(src_buffer, 0, 256);
	was_read = stream_read(pipe, src_buffer, 137);
	EXPECT_EQ(was_read, 137);
	was_read = stream_read(pipe, src_buffer + 137, 256 - 137);
	EXPECT_EQ(was_read, 256 - 137);
	for (i = 0; i < 256; ++i)
		EXPECT_EQ(src_buffer[i], (unsigned char)i);

	was_write = stream_write(pipe, src_buffer, 199);
	EXPECT_EQ(was_write, 199);
	thread_sleep(100);
	was_write = stream_write(pipe, src_buffer + 199, 256 - 199);
	EXPECT_EQ(was_write, 256 - 199);
	thread_sleep(3000);

	memset(src_buffer, 0, 256);
	was_read = stream_read(pipe, src_buffer, 255);
	EXPECT_EQ(was_read, 255);
	was_read = stream_read(pipe, src_buffer + 255, 256 - 255);
	EXPECT_EQ(was_read, 256 - 255);
	for (i = 0; i < 256; ++i)
		EXPECT_EQ(src_buffer[i], (unsigned char)i);

	return 0;
}

DECLARE_TEST(pipe, readwrite) {
	stream_t* pipe;
	thread_t reader;
	thread_t writer;

	pipe = pipe_allocate();

#if FOUNDATION_PLATFORM_WINDOWS
	EXPECT_NE(pipe_read_handle(pipe), 0);
	EXPECT_NE(pipe_write_handle(pipe), 0);
	EXPECT_EQ(pipe_read_handle(0), 0);
	EXPECT_EQ(pipe_write_handle(0), 0);
#endif
	EXPECT_NE(pipe_read_fd(pipe), 0);
	EXPECT_NE(pipe_write_fd(pipe), 0);
	EXPECT_EQ(pipe_read_fd(0), 0);
	EXPECT_EQ(pipe_write_fd(0), 0);

	EXPECT_EQ(stream_size(pipe), 0);
	EXPECT_EQ(stream_tell(pipe), 0);

	stream_flush(pipe);
	stream_truncate(pipe, 100);
	stream_seek(pipe, 10, STREAM_SEEK_BEGIN);

	EXPECT_EQ(stream_tell(pipe), 0);
	EXPECT_EQ(stream_available_read(pipe), 0);

	thread_initialize(&reader, read_thread, pipe, STRING_CONST("reader"), THREAD_PRIORITY_NORMAL, 0);
	thread_initialize(&writer, write_thread, pipe, STRING_CONST("writer"), THREAD_PRIORITY_NORMAL, 0);

	thread_start(&reader);
	thread_start(&writer);

	while (!thread_is_started(&reader) || !thread_is_started(&writer))
		thread_sleep(10);

	while (thread_is_running(&reader) && thread_is_running(&writer))
		thread_sleep(10);

	if (!thread_is_running(&reader))
		EXPECT_EQ(reader.result, 0);
	if (!thread_is_running(&writer))
		EXPECT_EQ(writer.result, 0);

	while (thread_is_running(&reader) || thread_is_running(&writer))
		thread_sleep(10);

	EXPECT_EQ(reader.result, 0);
	EXPECT_EQ(writer.result, 0);

	thread_finalize(&reader);
	thread_finalize(&writer);

	stream_deallocate(pipe);

	return 0;
}

static void
test_pipe_declare(void) {
	ADD_TEST(pipe, readwrite);
}

static test_suite_t test_pipe_suite = {test_pipe_application,
                                       test_pipe_memory_system,
                                       test_pipe_config,
                                       test_pipe_declare,
                                       test_pipe_initialize,
                                       test_pipe_finalize,
                                       0};

#if BUILD_MONOLITHIC

int
test_pipe_run(void);

int
test_pipe_run(void) {
	test_suite = test_pipe_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_pipe_suite;
}

#endif
