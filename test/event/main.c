/* main.c  -  Foundation event test  -  Public Domain  -  2013 Mattias Jansson
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

// Events are not sizeof(event_t), but aligned to 8 byte boundaries
#define EXPECTED_EVENT_SIZE 16

static application_t
test_event_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation event tests"));
	app.short_name = string_const(STRING_CONST("test_event"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_event_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_event_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_event_initialize(void) {
	return 0;
}

static void
test_event_finalize(void) {
}

static int
assert_ignore_handler(hash_t context, const char* condition, size_t cond_length, const char* file, size_t file_length,
                      unsigned int line, const char* msg, size_t msg_length) {
	FOUNDATION_UNUSED(context);
	FOUNDATION_UNUSED(condition);
	FOUNDATION_UNUSED(cond_length);
	FOUNDATION_UNUSED(file);
	FOUNDATION_UNUSED(file_length);
	FOUNDATION_UNUSED(line);
	FOUNDATION_UNUSED(msg);
	FOUNDATION_UNUSED(msg_length);
	return 0;
}

DECLARE_TEST(event, empty) {
	event_stream_t* stream;
	event_block_t* block;
	event_block_t* old_block;
	event_t* event;

	stream = event_stream_allocate(0);
	block = event_stream_process(stream);

	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	old_block = block;
	block = event_stream_process(stream);
	EXPECT_NE(old_block, block);

	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	block = event_stream_process(stream);
	EXPECT_EQ(old_block, block);

	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	event_stream_deallocate(stream);

	stream = event_stream_allocate(4096);
	block = event_stream_process(stream);

	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	old_block = block;
	block = event_stream_process(stream);
	EXPECT_NE(old_block, block);

	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	block = event_stream_process(stream);
	EXPECT_EQ(old_block, block);

	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	event_stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(event, immediate) {
	event_stream_t* stream;
	event_block_t* block;
	event_t* event;
	uint16_t last_serial = 0;
	size_t iloop;
	assert_handler_fn prev_assert_handler;
	uint8_t buffer[128];

	stream = event_stream_allocate(0);

	event_post(stream, FOUNDATIONEVENT_TERMINATE, 0, 0, 0, 0);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_INTEQ(event->id, FOUNDATIONEVENT_TERMINATE);
	EXPECT_SIZEEQ(event->size, EXPECTED_EVENT_SIZE);
	// EXPECT_UINTGT(event->serial, 0);
	EXPECT_EQ(event->object, 0);
	EXPECT_EQ(event->flags, 0);
	EXPECT_SIZEEQ(event_payload_size(event), EXPECTED_EVENT_SIZE - sizeof(event_t));
	last_serial = event->serial;

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	block = event_stream_process(stream);

	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	event_post(stream, FOUNDATIONEVENT_TERMINATE, 0, 0, buffer, 13);
	event_post_varg(stream, FOUNDATIONEVENT_TERMINATE + 1, 0, 0, buffer, (size_t)3, buffer + 3, (size_t)10, buffer + 13,
	                (size_t)24, nullptr);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_INTEQ(event->id, FOUNDATIONEVENT_TERMINATE);
	EXPECT_SIZEEQ(event->size, EXPECTED_EVENT_SIZE + 16);
	EXPECT_UINTGT(event->serial, last_serial);
	EXPECT_EQ(event->object, 0);
	EXPECT_EQ(event->flags, 0);
	EXPECT_SIZEEQ(event_payload_size(event), EXPECTED_EVENT_SIZE + 16 - sizeof(event_t));
	last_serial = event->serial;

	event = event_next(block, event);
	EXPECT_NE(event, 0);
	EXPECT_INTEQ(event->id, FOUNDATIONEVENT_TERMINATE + 1);
	EXPECT_SIZEEQ(event->size, EXPECTED_EVENT_SIZE + 40);
	EXPECT_UINTGT(event->serial, last_serial);
	EXPECT_EQ(event->object, 0);
	EXPECT_EQ(event->flags, 0);
	EXPECT_SIZEEQ(event_payload_size(event), EXPECTED_EVENT_SIZE + 40 - sizeof(event_t));
	last_serial = event->serial;

	// Test out of memory handling
	log_enable_stdout(false);
	prev_assert_handler = assert_handler();
	assert_set_handler(assert_ignore_handler);
	for (iloop = 0; iloop < 512 * 1024; ++iloop)
		event_post_varg(stream, FOUNDATIONEVENT_TERMINATE + 1, 0, 0, buffer, (size_t)3, buffer + 3, (size_t)10,
		                buffer + 13, (size_t)24, nullptr);
	assert_set_handler(prev_assert_handler);
	log_enable_stdout(true);
	EXPECT_TYPEEQ(error(), ERROR_OUT_OF_MEMORY, error_t, "d");

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_INTEQ(event->id, FOUNDATIONEVENT_TERMINATE + 1);
	EXPECT_SIZEEQ(event->size, EXPECTED_EVENT_SIZE + 40);
	EXPECT_UINTGT(event->serial, last_serial);
	EXPECT_EQ(event->object, 0);
	EXPECT_EQ(event->flags, 0);
	EXPECT_SIZEEQ(event_payload_size(event), EXPECTED_EVENT_SIZE + 40 - sizeof(event_t));

	event_stream_deallocate(stream);

	return 0;
}

typedef FOUNDATION_ALIGNED_STRUCT(producer_thread_arg, 16) {
	event_stream_t* stream;
	tick_t max_delay;
	tick_t end_time;
	unsigned int sleep_time;
	unsigned int id;
}
producer_thread_arg_t;

static void*
producer_thread(void* arg) {
	uint64_t random_delay;
	uint16_t random_id, random_size;
	uint8_t buffer[256];
	producer_thread_arg_t* args = arg;
	unsigned int produced = 0;
	tick_t timestamp = 0;

	do {
		if (args->sleep_time)
			thread_sleep(args->sleep_time);
		random_delay = args->max_delay ? random64_range(0, (uint64_t)args->max_delay) : 0ULL;
		random_id = (uint16_t)random32_range(1, 65535);
		random_size = (uint16_t)random32_range(args->max_delay ? 8 : 0, 256);
		thread_yield();
		// There is a small chance of test failure due to thread time slice contention here
		// If the producer thread is suspended between the timestamp calculation and event post
		// an event with an old timestamp might be posted after the main test thread does an
		// event process - test will fail (timestamp check will fail since it is old due to suspension)
		timestamp = args->max_delay ? time_current() + (tick_t)random_delay : 0;
		memcpy(buffer, &timestamp, sizeof(tick_t));
		event_post_varg(args->stream, random_id, args->id, timestamp, buffer, random_size / 2,
		                buffer + (random_size / 2), (size_t)(random_size - (random_size / 2)), nullptr);
		++produced;
	} while (!thread_try_wait(0) && (time_current() < args->end_time));

	return (void*)((uintptr_t)produced);
}

DECLARE_TEST(event, immediate_threaded) {
	thread_t thread[32];
	producer_thread_arg_t args[32];
	event_stream_t* stream;
	event_block_t* block;
	event_t* event;
	size_t read[32];
	unsigned int i;
	bool running = true;
	size_t threads_count = math_clamp(system_hardware_threads() * 4, 4, 32);
	size_t max_payload = 256 + EXPECTED_EVENT_SIZE - sizeof(event_t);

	stream = event_stream_allocate(0);

	for (i = 0; i < threads_count; ++i) {
		args[i].stream = stream;
		args[i].end_time = time_current() + (time_ticks_per_second() * 5);
		args[i].max_delay = 0;
		args[i].sleep_time = 5;
		args[i].id = i;

		read[i] = 0;
		thread_initialize(&thread[i], producer_thread, args + i, STRING_CONST("event_producer"), THREAD_PRIORITY_NORMAL,
		                  0);
	}
	for (i = 0; i < threads_count; ++i)
		thread_start(&thread[i]);

	test_wait_for_threads_startup(thread, threads_count);

	while (running) {
		running = false;

		for (i = 0; i < threads_count; ++i) {
			if (thread_is_running(&thread[i])) {
				running = true;
				break;
			}
		}

		thread_yield();

		block = event_stream_process(stream);
		event = event_next(block, 0);
		while (event) {
			++read[event->object];
			running = true;

			EXPECT_SIZELE(event_payload_size(event), max_payload);
			event = event_next(block, event);
		}
	}

	block = event_stream_process(stream);
	event = event_next(block, 0);
	while (event) {
		EXPECT_SIZELE(event_payload_size(event), max_payload);
		++read[event->object];
		event = event_next(block, event);
	}

	for (i = 0; i < threads_count; ++i) {
		void* result = thread[i].result;
		size_t should_have_read = (uintptr_t)result;
		EXPECT_EQ(read[i], should_have_read);
		thread_finalize(&thread[i]);
	}

	event_stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(event, delay) {
	event_stream_t* stream;
	event_block_t* block;
	event_t* event;
	tick_t delivery = 0;
	tick_t limit = 0;
	tick_t current = 0;
	tick_t halfsecond, smalltick;
	uint8_t expect_event = FOUNDATIONEVENT_TERMINATE;

	stream = event_stream_allocate(0);

	current = time_current();
	halfsecond = time_ticks_per_second() / 2;
	smalltick = time_ticks_per_second() / 100;
	delivery = current + halfsecond;
	limit = current + (halfsecond * 5);

	event_post(stream, expect_event, 0, delivery, 0, 0);
	event_post(stream, expect_event + 1, 0, delivery + smalltick, 0, 0);

	do {
		block = event_stream_process(stream);
		event = event_next(block, 0);
		current = time_current();

		if ((expect_event == FOUNDATIONEVENT_TERMINATE) && event) {
			EXPECT_EQ(event->id, expect_event);
			EXPECT_EQ(event->size, EXPECTED_EVENT_SIZE + 8);  // 8 bytes for additional timestamp payload
			EXPECT_EQ(event->object, 0);
			EXPECT_EQ(event->flags, EVENTFLAG_DELAY);
			EXPECT_SIZEEQ(event_payload_size(event), EXPECTED_EVENT_SIZE - sizeof(event_t));

			EXPECT_GE(current, delivery);

			++expect_event;

			event = event_next(block, event);
			current = time_current();
		}

		if ((expect_event == FOUNDATIONEVENT_TERMINATE + 1) && event) {
			EXPECT_EQ(event->id, expect_event);
			EXPECT_EQ(event->size, EXPECTED_EVENT_SIZE + 8);  // 8 bytes for additional timestamp payload
			EXPECT_EQ(event->object, 0);
			EXPECT_EQ(event->flags, EVENTFLAG_DELAY);
			EXPECT_SIZEEQ(event_payload_size(event), EXPECTED_EVENT_SIZE - sizeof(event_t));

			EXPECT_GE(current, delivery + smalltick);

			++expect_event;

			event = event_next(block, event);
			current = time_current();

			EXPECT_EQ(event, 0);
		}

		if ((expect_event > FOUNDATIONEVENT_TERMINATE + 1) && !event)
			break;

		thread_yield();
	} while (time_current() < limit);

	EXPECT_EQ(event, 0);
	EXPECT_EQ(expect_event, FOUNDATIONEVENT_TERMINATE + 2);
	EXPECT_LE(time_current(), limit);

	// Reverse order of delivery
	current = time_current();
	delivery = current + halfsecond;
	limit = current + (halfsecond * 5);

	expect_event = FOUNDATIONEVENT_TERMINATE;
	event_post(stream, expect_event, 0, delivery + smalltick, 0, 0);
	event_post(stream, expect_event + 1, 0, delivery, 0, 0);
	++expect_event;  // Expect second event to deliver first

	do {
		block = event_stream_process(stream);
		event = event_next(block, 0);
		current = time_current();

		if ((expect_event == FOUNDATIONEVENT_TERMINATE + 1) && event) {
			EXPECT_INTEQ(event->id, expect_event);
			EXPECT_SIZEEQ(event->size, EXPECTED_EVENT_SIZE + 8);  // 8 bytes for additional timestamp payload
			EXPECT_EQ(event->object, 0);
			EXPECT_EQ(event->flags, EVENTFLAG_DELAY);
			EXPECT_SIZEEQ(event_payload_size(event), EXPECTED_EVENT_SIZE - sizeof(event_t));

			EXPECT_GE(current, delivery);

			--expect_event;

			event = event_next(block, event);
			current = time_current();
		}

		if ((expect_event == FOUNDATIONEVENT_TERMINATE) && event) {
			EXPECT_INTEQ(event->id, expect_event);
			EXPECT_SIZEEQ(event->size, EXPECTED_EVENT_SIZE + 8);  // 8 bytes for additional timestamp payload
			EXPECT_EQ(event->object, 0);
			EXPECT_EQ(event->flags, EVENTFLAG_DELAY);
			EXPECT_SIZEEQ(event_payload_size(event), EXPECTED_EVENT_SIZE - sizeof(event_t));

			EXPECT_GE(current, delivery + smalltick);

			--expect_event;

			event = event_next(block, event);

			EXPECT_EQ(event, 0);
		}

		if ((expect_event < FOUNDATIONEVENT_TERMINATE) && !event)
			break;

		thread_yield();

	} while (time_current() < limit);

	EXPECT_EQ(event, 0);
	EXPECT_INTEQ(expect_event, FOUNDATIONEVENT_TERMINATE - 1);
	EXPECT_LE(time_current(), limit);

	event_stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(event, delay_threaded) {
	thread_t thread[32];
	producer_thread_arg_t args[32];
	event_stream_t* stream;
	event_block_t* block;
	event_t* event;
	tick_t endtime, curtime, payloadtime, begintime, prevtime;
	size_t read[32];
	unsigned int i;
	bool running = true;
	size_t threads_count = math_clamp(system_hardware_threads() * 4, 4, 32);

	stream = event_stream_allocate(0);
	begintime = time_current();

	thread_yield();

	for (i = 0; i < threads_count; ++i) {
		args[i].stream = stream;
		args[i].end_time = time_current() + (time_ticks_per_second() * 5);
		args[i].max_delay = time_ticks_per_second() * 5;
		args[i].sleep_time = 50;
		args[i].id = i;

		read[i] = 0;
		thread_initialize(&thread[i], producer_thread, args + i, STRING_CONST("event_producer"), THREAD_PRIORITY_NORMAL,
		                  0);
	}
	for (i = 0; i < threads_count; ++i)
		thread_start(&thread[i]);

	test_wait_for_threads_startup(thread, threads_count);

	while (running) {
		running = false;

		for (i = 0; i < threads_count; ++i) {
			if (thread_is_running(&thread[i])) {
				running = true;
				break;
			}
		}

		thread_yield();

		prevtime = begintime;
		begintime = time_current();

		thread_sleep(1);

		block = event_stream_process(stream);
		event = event_next(block, 0);
		curtime = time_current();

		while (event) {
			char msgbuf[64];

			running = true;
			++read[event->object];
			memcpy(&payloadtime, event->payload, sizeof(tick_t));

			string_format(msgbuf, 64, STRING_CONST("payload %" PRIu64 " - previous %" PRIu64 " (%dms)"), payloadtime,
			              prevtime, (int)(time_ticks_to_seconds(time_diff(prevtime, payloadtime)) * REAL_C(1000.0)));

			EXPECT_SIZEGE(event_payload_size(event), EXPECTED_EVENT_SIZE + 8 - sizeof(event_t));
			EXPECT_SIZELE(event_payload_size(event), EXPECTED_EVENT_SIZE + 256 - sizeof(event_t));
			EXPECT_GE_MSG(payloadtime, prevtime, msgbuf);
			EXPECT_GE(curtime, payloadtime);

			event = event_next(block, event);
			curtime = time_current();
		}
	}

	endtime = time_current() + (time_ticks_per_second() * 6);
	do {
		prevtime = begintime;
		begintime = time_current();
		block = event_stream_process(stream);
		event = event_next(block, 0);
		curtime = time_current();
		while (event) {
			++read[event->object];
			memcpy(&payloadtime, event->payload, sizeof(tick_t));

			EXPECT_SIZEGE(event_payload_size(event), EXPECTED_EVENT_SIZE + 8 - sizeof(event_t));
			EXPECT_SIZELE(event_payload_size(event), EXPECTED_EVENT_SIZE + 256 - sizeof(event_t));
			EXPECT_GE(payloadtime, prevtime);
			EXPECT_GE(curtime, payloadtime);

			event = event_next(block, event);
			curtime = time_current();
		}

		thread_sleep(10);

	} while (time_current() < endtime);

	for (i = 0; i < threads_count; ++i) {
		void* result = thread[i].result;
		size_t should_have_read = (size_t)((uintptr_t)result);
		EXPECT_EQ(read[i], should_have_read);
		thread_finalize(&thread[i]);
	}

	event_stream_deallocate(stream);

	return 0;
}

static void
test_event_declare(void) {
	ADD_TEST(event, empty);
	ADD_TEST(event, immediate);
	ADD_TEST(event, delay);
	ADD_TEST(event, immediate_threaded);
	ADD_TEST(event, delay_threaded);
}

static test_suite_t test_event_suite = {test_event_application,
                                        test_event_memory_system,
                                        test_event_config,
                                        test_event_declare,
                                        test_event_initialize,
                                        test_event_finalize,
                                        0};

#if BUILD_MONOLITHIC

int
test_event_run(void);

int
test_event_run(void) {
	test_suite = test_event_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_event_suite;
}

#endif
