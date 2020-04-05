/* main.c  -  Foundation ringbuffer test  -  Public Domain  -  2013 Mattias Jansson
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
test_ringbuffer_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation ringbuffer tests"));
	app.short_name = string_const(STRING_CONST("test_ringbuffer"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_ringbuffer_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_ringbuffer_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_ringbuffer_initialize(void) {
	return 0;
}

static void
test_ringbuffer_finalize(void) {
}

DECLARE_TEST(ringbuffer, allocate) {
	ringbuffer_t* buffer;
	char store[256];

	buffer = ringbuffer_allocate(0);
	EXPECT_EQ(ringbuffer_size(buffer), 0);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	EXPECT_EQ(ringbuffer_read(buffer, store, 0), 0);
	EXPECT_EQ(ringbuffer_write(buffer, store, 0), 0);
	EXPECT_EQ(ringbuffer_size(buffer), 0);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	EXPECT_EQ(ringbuffer_read(buffer, store, 256), 0);
	EXPECT_EQ(ringbuffer_write(buffer, store, 256), 0);
	EXPECT_EQ(ringbuffer_size(buffer), 0);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	ringbuffer_deallocate(buffer);

	buffer = ringbuffer_allocate(128);
	EXPECT_EQ(ringbuffer_size(buffer), 128);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	EXPECT_EQ(ringbuffer_write(buffer, store, 0), 0);
	EXPECT_EQ(ringbuffer_read(buffer, store, 0), 0);
	EXPECT_EQ(ringbuffer_size(buffer), 128);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	EXPECT_EQ(ringbuffer_write(buffer, store, 256), 127);
	EXPECT_EQ(ringbuffer_read(buffer, store, 256), 127);
	EXPECT_EQ(ringbuffer_size(buffer), 128);
	EXPECT_EQ(ringbuffer_total_read(buffer), 127);
	EXPECT_EQ(ringbuffer_total_written(buffer), 127);

	ringbuffer_deallocate(buffer);

	buffer = ringbuffer_allocate(256);
	EXPECT_EQ(ringbuffer_size(buffer), 256);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	EXPECT_EQ(ringbuffer_write(buffer, store, 0), 0);
	EXPECT_EQ(ringbuffer_read(buffer, store, 0), 0);
	EXPECT_EQ(ringbuffer_size(buffer), 256);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	EXPECT_EQ(ringbuffer_write(buffer, store, 256), 255);
	EXPECT_EQ(ringbuffer_read(buffer, store, 256), 255);
	EXPECT_EQ(ringbuffer_size(buffer), 256);
	EXPECT_EQ(ringbuffer_total_read(buffer), 255);
	EXPECT_EQ(ringbuffer_total_written(buffer), 255);

	ringbuffer_deallocate(buffer);

	buffer = ringbuffer_allocate(512);
	EXPECT_EQ(ringbuffer_size(buffer), 512);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	EXPECT_EQ(ringbuffer_write(buffer, store, 0), 0);
	EXPECT_EQ(ringbuffer_read(buffer, store, 0), 0);
	EXPECT_EQ(ringbuffer_size(buffer), 512);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	EXPECT_EQ(ringbuffer_write(buffer, store, 256), 256);
	EXPECT_EQ(ringbuffer_read(buffer, store, 256), 256);
	EXPECT_EQ(ringbuffer_size(buffer), 512);
	EXPECT_EQ(ringbuffer_total_read(buffer), 256);
	EXPECT_EQ(ringbuffer_total_written(buffer), 256);

	EXPECT_EQ(ringbuffer_write(buffer, store, 256), 256);
	EXPECT_EQ(ringbuffer_read(buffer, store, 256), 256);
	EXPECT_EQ(ringbuffer_size(buffer), 512);
	EXPECT_EQ(ringbuffer_total_read(buffer), 512);
	EXPECT_EQ(ringbuffer_total_written(buffer), 512);

	ringbuffer_reset(buffer);
	EXPECT_EQ(ringbuffer_total_read(buffer), 0);
	EXPECT_EQ(ringbuffer_total_written(buffer), 0);

	ringbuffer_deallocate(buffer);

	return 0;
}

DECLARE_TEST(ringbuffer, io) {
	ringbuffer_t* buffer;
	char from[256];
	char to[256];
	unsigned int size, verify, loop, loops;
	unsigned int expected_size = 0;

	for (size = 0; size < 256; ++size)
		from[size] = (char)(random32() & 0xFF);

	buffer = ringbuffer_allocate(512);
	loops = 32;
	for (loop = 0; loop < loops; ++loop) {
		for (size = 0; size < 256; ++size) {
			ringbuffer_write(buffer, from, size);
			ringbuffer_read(buffer, to, size);

			for (verify = 0; verify < size; ++verify)
				EXPECT_EQ(to[verify], from[verify]);

			expected_size += size;
		}
	}
	EXPECT_EQ(ringbuffer_total_read(buffer), expected_size);
	EXPECT_EQ(ringbuffer_total_written(buffer), expected_size);

	ringbuffer_deallocate(buffer);

	return 0;
}

typedef struct {
	stream_t* stream;

	thread_t read_thread;
	thread_t write_thread;

	char* source_buffer;
	char* dest_buffer;

	unsigned int buffer_size;

	tick_t start_time;
	tick_t end_time;
} ringbufferstream_test_t;

static void*
read_thread(void* arg) {
	ringbufferstream_test_t* test = arg;
	stream_read(test->stream, test->dest_buffer, test->buffer_size);
	test->end_time = time_current();
	return 0;
}

static void*
write_thread(void* arg) {
	ringbufferstream_test_t* test = arg;
	test->start_time = time_current();
	stream_write(test->stream, test->source_buffer, test->buffer_size);
	return 0;
}

DECLARE_TEST(ringbufferstream, threadedio) {
	ringbufferstream_test_t test;
	uint32_t* srcbuffer;
	unsigned int si;
	unsigned int loop, loops;
	real elapsed;
	real rb_throughput;
	real mem_throughput;
	unsigned int mbytes;

#if FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
	mbytes = 16;
	loops = 32;
#else
	mbytes = 256;
	loops = 16;
#endif

	test.buffer_size = mbytes * 1024 * 1024;

	srcbuffer = memory_allocate(0, test.buffer_size, 0, MEMORY_PERSISTENT);

	test.source_buffer = (void*)srcbuffer;
	test.dest_buffer = memory_allocate(0, test.buffer_size, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

	for (si = 0; si < (test.buffer_size / 4); ++si)
		srcbuffer[si] = random32();

	elapsed = 0;
	for (loop = 0; loop < loops; ++loop) {
		size_t ringbuffer_size = 23477;
		test.stream = ringbuffer_stream_allocate(ringbuffer_size, test.buffer_size);
		EXPECT_NE(test.stream, 0);

		thread_initialize(&test.read_thread, read_thread, &test, STRING_CONST("reader"), THREAD_PRIORITY_NORMAL, 0);
		thread_initialize(&test.write_thread, write_thread, &test, STRING_CONST("writer"), THREAD_PRIORITY_NORMAL, 0);

		thread_start(&test.read_thread);
		thread_start(&test.write_thread);

		while (!thread_is_started(&test.read_thread) || !thread_is_started(&test.write_thread))
			thread_sleep(10);

		while (thread_is_running(&test.read_thread) || thread_is_running(&test.write_thread))
			thread_sleep(10);

		thread_finalize(&test.read_thread);
		thread_finalize(&test.write_thread);

		for (si = 0; si < test.buffer_size; ++si)
			EXPECT_EQ(test.source_buffer[si], test.dest_buffer[si]);

		EXPECT_TRUE(stream_eos(test.stream));
		EXPECT_EQ(stream_size(test.stream), test.buffer_size);
		stream_truncate(test.stream, stream_tell(test.stream) + 32);
		EXPECT_FALSE(stream_eos(test.stream));
		stream_truncate(test.stream, 0);
		EXPECT_FALSE(stream_eos(test.stream));
		stream_truncate(test.stream, 1);
		EXPECT_TRUE(stream_eos(test.stream));
		EXPECT_EQ(stream_size(test.stream), 1);
		EXPECT_SIZEEQ(stream_available_read(test.stream), 0);
		EXPECT_SIZEEQ(stream_tell(test.stream), test.buffer_size);

		stream_write(test.stream, test.source_buffer, ringbuffer_size - 1);
		EXPECT_SIZEEQ(stream_tell(test.stream), test.buffer_size);
		stream_seek(test.stream, (ssize_t)ringbuffer_size - 1, STREAM_SEEK_CURRENT);
		EXPECT_SIZEEQ(stream_tell(test.stream), test.buffer_size + ringbuffer_size - 1);
		log_enable_stdout(false);
		stream_seek(test.stream, -(ssize_t)ringbuffer_size, STREAM_SEEK_CURRENT);
		stream_seek(test.stream, 0, STREAM_SEEK_BEGIN);
		log_enable_stdout(true);
		EXPECT_SIZEEQ(stream_tell(test.stream), test.buffer_size + ringbuffer_size - 1);
		{
			tick_t curtime = time_system();
			EXPECT_TICKGE(stream_last_modified(test.stream), curtime);
		}

		stream_deallocate(test.stream);

		elapsed += time_ticks_to_seconds(time_diff(test.start_time, test.end_time));
	}
	rb_throughput = (real)((float64_t)(mbytes * loops) / (float64_t)elapsed);
	// log_infof(HASH_TEST, STRING_CONST("Ringbuffer throughput: %d MiB in %.2f sec -> %.2f MiB/sec"),
	//          (loops * mbytes), (float32_t)elapsed, (float32_t)throughput);

	elapsed = 0;
	for (loop = 0; loop < loops; ++loop) {
		test.start_time = time_current();
		memcpy(test.dest_buffer, test.source_buffer, test.buffer_size);
		test.end_time = time_current();

		for (si = 0; si < test.buffer_size; ++si)
			EXPECT_EQ(test.source_buffer[si], test.dest_buffer[si]);

		elapsed += time_ticks_to_seconds(time_diff(test.start_time, test.end_time));
	}
	mem_throughput = (real)((float64_t)(mbytes * loops) / (float64_t)elapsed);
	// log_infof(HASH_TEST, STRING_CONST("Memcpy     throughput: %d MiB in %.2f sec -> %.2f MiB/sec"),
	//          (loops * mbytes), (float32_t)elapsed, (float32_t)throughput);
	EXPECT_REALGT(mem_throughput, rb_throughput);

	memory_deallocate(test.source_buffer);
	memory_deallocate(test.dest_buffer);

	return 0;
}

static void
test_ringbuffer_declare(void) {
	ADD_TEST(ringbuffer, allocate);
	ADD_TEST(ringbuffer, io);

	ADD_TEST(ringbufferstream, threadedio);
}

static test_suite_t test_ringbuffer_suite = {test_ringbuffer_application,
                                             test_ringbuffer_memory_system,
                                             test_ringbuffer_config,
                                             test_ringbuffer_declare,
                                             test_ringbuffer_initialize,
                                             test_ringbuffer_finalize,
                                             0};

#if BUILD_MONOLITHIC

int
test_ringbuffer_run(void);

int
test_ringbuffer_run(void) {
	test_suite = test_ringbuffer_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_ringbuffer_suite;
}

#endif
