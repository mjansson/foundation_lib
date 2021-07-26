/* main.c  -  Foundation bufferstream test  -  Public Domain  -  2013 Mattias Jansson
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
test_bufferstream_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation bufferstream tests"));
	app.short_name = string_const(STRING_CONST("test_bufferstream"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_bufferstream_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_bufferstream_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_bufferstream_initialize(void) {
	return 0;
}

static void
test_bufferstream_finalize(void) {
}

DECLARE_TEST(bufferstream, null) {
	stream_t* stream = 0;
	tick_t curtime = time_current();
	uint8_t readbuffer[1024] = {0};
	uint8_t writebuffer[1024] = {0};
	uint128_t md5zero;

	md5zero = uint128_null();

	stream = buffer_stream_allocate(0, 0, 0, 0, false, false);
	EXPECT_NE(stream, 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_CONSTSTRINGEQ(string_const(stream_path(stream).str, 11), string_const(STRING_CONST("buffer://0x")));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5zero));

	// No buffer and not growing - all read/write ops should do nothing
	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 0);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5zero));

	stream_truncate(stream, 1024);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5zero));

	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 0);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5zero));

	stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(bufferstream, zero) {
	stream_t* stream = 0;
	tick_t curtime = time_current();
	uint8_t readbuffer[1024] = {0};
	uint8_t writebuffer[1024] = {0};
	uint8_t backing_store[1024] = {0};
	uint128_t md5null;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize(md5);
		md5null = md5_get_digest_raw(md5);
		md5_deallocate(md5);
	}

	stream = buffer_stream_allocate(backing_store, STREAM_IN | STREAM_OUT, 0, 0, false, false);
	EXPECT_NE(stream, 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_CONSTSTRINGEQ(string_const(stream_path(stream).str, 11), string_const(STRING_CONST("buffer://0x")));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	// No buffer and not growing - all read/write ops should do nothing
	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 0);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	stream_truncate(stream, 1024);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 0);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(bufferstream, null_grow) {
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	string_t writestr;
	uint128_t md5null;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize(md5);
		md5null = md5_get_digest_raw(md5);
		md5_deallocate(md5);
	}

	stream = buffer_stream_allocate(0, STREAM_IN | STREAM_OUT, 0, 0, true, true);
	EXPECT_NE(stream, 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_CONSTSTRINGEQ(string_const(stream_path(stream).str, 11), string_const(STRING_CONST("buffer://0x")));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	writestr = string_copy(writebuffer, sizeof(writebuffer),
	                       STRING_CONST("MD5 test string for which the value is precomputed"));
	EXPECT_EQ(stream_write(stream, writestr.str, writestr.length), writestr.length);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), writestr.length);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ(stream_read(stream, readbuffer, 1024), writestr.length);
	EXPECT_TRUE(string_equal(readbuffer, writestr.length, writebuffer, writestr.length));
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), writestr.length);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_truncate(stream, 1024);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 1024);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 1024 - writestr.length);

	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 1024 - writestr.length);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 1024);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 1024 * 2);
	EXPECT_EQ(stream_tell(stream), 1024 * 2);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_deallocate(stream);

	// Test invalid parameter combo (size > capacity) (!adopt && grow)
	log_enable_stdout(false);
	stream = buffer_stream_allocate(0, STREAM_IN | STREAM_OUT, 256, 0, false, true);
	log_enable_stdout(true);
	EXPECT_NE(stream, 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_CONSTSTRINGEQ(string_const(stream_path(stream).str, 11), string_const(STRING_CONST("buffer://0x")));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	writestr = string_copy(writebuffer, sizeof(writebuffer),
	                       STRING_CONST("MD5 test string for which the value is precomputed"));
	EXPECT_EQ(stream_write(stream, writestr.str, writestr.length), 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);

	stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(bufferstream, zero_grow) {
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint8_t* backing_store = memory_allocate(0, 315, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	uint128_t md5null;
	string_t writestr;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize(md5);
		md5null = md5_get_digest_raw(md5);
		md5_deallocate(md5);
	}

	stream = buffer_stream_allocate(backing_store, STREAM_IN | STREAM_OUT, 0, 315, true, true);
	EXPECT_NE(stream, 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_TRUE(string_equal(stream_path(stream).str, 11, "buffer://0x", 11));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	writestr = string_copy(writebuffer, 1024, STRING_CONST("MD5 test string for which the value is precomputed"));
	EXPECT_EQ(stream_write(stream, writebuffer, writestr.length), writestr.length);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), writestr.length);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ(stream_read(stream, readbuffer, 1024), writestr.length);
	EXPECT_TRUE(string_equal(readbuffer, writestr.length, writebuffer, writestr.length));
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), writestr.length);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_truncate(stream, 1024);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 1024);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 1024 - writestr.length);

	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 1024 - writestr.length);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 1024);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 1024 * 2);
	EXPECT_EQ(stream_tell(stream), 1024 * 2);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(bufferstream, zero_nogrow) {
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint8_t* backing_store = memory_allocate(0, 1024, 0, MEMORY_PERSISTENT);
	uint128_t md5null;
	string_t writestr;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize(md5);
		md5null = md5_get_digest_raw(md5);
		md5_deallocate(md5);
	}

	stream = buffer_stream_allocate(backing_store, STREAM_IN | STREAM_OUT, 0, 1024, true, false);
	EXPECT_NE(stream, 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_TRUE(string_equal(stream_path(stream).str, 11, "buffer://0x", 11));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	writestr = string_copy(writebuffer, 1024, STRING_CONST("MD5 test string for which the value is precomputed"));
	EXPECT_EQ(stream_write(stream, writebuffer, writestr.length), writestr.length);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), writestr.length);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ(stream_read(stream, readbuffer, 1024), writestr.length);
	EXPECT_TRUE(string_equal(readbuffer, writestr.length, writebuffer, writestr.length));
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), writestr.length);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_truncate(stream, 2048);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 1024);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 1024 - writestr.length);

	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 1024 - writestr.length);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 1024);
	EXPECT_EQ(stream_tell(stream), 1024);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(bufferstream, sized_grow) {
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint8_t* backing_store;
	uint128_t md5null;
	string_t writestr;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize(md5);
		md5null = md5_get_digest_raw(md5);
		md5_deallocate(md5);
	}

	backing_store = memory_allocate(0, 1024, 0, MEMORY_PERSISTENT);
	stream = buffer_stream_allocate(backing_store, STREAM_IN | STREAM_OUT, 315, 1024, true, true);
	EXPECT_NE(stream, 0);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_TRUE(string_equal(stream_path(stream).str, 11, "buffer://0x", 11));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 315);
	EXPECT_FALSE(uint128_equal(stream_md5(stream), md5null));

	writestr = string_copy(writebuffer, 1024, STRING_CONST("MD5 test string for which the value is precomputed"));
	EXPECT_EQ(stream_write(stream, writebuffer, writestr.length), writestr.length);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 315 - writestr.length);

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 315);
	EXPECT_TRUE(string_equal(readbuffer, writestr.length, STRING_ARGS(writestr)));
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), 315);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_truncate(stream, 2048);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 2048);
	EXPECT_EQ(stream_tell(stream), 315);
	EXPECT_EQ(stream_available_read(stream), 2048 - 315);

	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 1024);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 1024);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 2048 + 315);
	EXPECT_EQ(stream_tell(stream), 2048 + 315);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_deallocate(stream);

	backing_store = memory_allocate(0, 1024, 0, MEMORY_PERSISTENT);
	stream = buffer_stream_allocate(backing_store, STREAM_IN | STREAM_OUT | STREAM_TRUNCATE, 315, 1024, true, true);
	EXPECT_NE(stream, 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 0);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_TRUE(string_equal(stream_path(stream).str, 11, "buffer://0x", 11));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 0);
	EXPECT_TRUE(uint128_equal(stream_md5(stream), md5null));

	writestr = string_copy(writebuffer, 1024, STRING_CONST("MD5 test string for which the value is precomputed"));
	EXPECT_EQ(stream_write(stream, writebuffer, writestr.length), writestr.length);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), writestr.length);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ(stream_read(stream, readbuffer, 1024), writestr.length);
	EXPECT_TRUE(string_equal(readbuffer, writestr.length, STRING_ARGS(writestr)));
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), writestr.length);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_truncate(stream, 2048);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 2048);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 2048 - writestr.length);

	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 1024);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 1024);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 2048 + writestr.length);
	EXPECT_EQ(stream_tell(stream), 2048 + writestr.length);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_deallocate(stream);

	return 0;
}

DECLARE_TEST(bufferstream, sized_nogrow) {
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint8_t* backing_store = memory_allocate(0, 1024, 0, MEMORY_PERSISTENT);
	uint128_t md5null;
	string_t writestr;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize(md5);
		md5null = md5_get_digest_raw(md5);
		md5_deallocate(md5);
	}

	stream = buffer_stream_allocate(backing_store, STREAM_IN | STREAM_OUT, 315, 1024, false, false);
	EXPECT_NE(stream, 0);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), 0);
	EXPECT_FALSE(stream_is_binary(stream));
	EXPECT_FALSE(stream_is_sequential(stream));
	EXPECT_TRUE(stream_is_reliable(stream));
	EXPECT_TRUE(stream_is_inorder(stream));
	EXPECT_TRUE(string_equal(stream_path(stream).str, 11, "buffer://0x", 11));
	EXPECT_GE(stream_last_modified(stream), curtime);
	EXPECT_EQ(stream_available_read(stream), 315);
	EXPECT_FALSE(uint128_equal(stream_md5(stream), md5null));

	writestr = string_copy(writebuffer, 1024, STRING_CONST("MD5 test string for which the value is precomputed"));
	EXPECT_SIZEEQ(stream_write(stream, writebuffer, writestr.length), writestr.length);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), writestr.length);
	EXPECT_EQ(stream_available_read(stream), 315 - writestr.length);

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	EXPECT_SIZEEQ(stream_read(stream, readbuffer, 1024), 315);
	EXPECT_TRUE(string_equal(readbuffer, writestr.length, STRING_ARGS(writestr)));
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), 315);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_seek(stream, -310, STREAM_SEEK_CURRENT);
	EXPECT_SIZEEQ(stream_read(stream, readbuffer, 1024), 310);
	EXPECT_TRUE(string_equal(readbuffer, writestr.length - 5, writestr.str + 5, writestr.length - 5));
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), 315);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_seek(stream, -310, STREAM_SEEK_END);
	EXPECT_SIZEEQ(stream_read(stream, readbuffer, 1024), 310);
	EXPECT_TRUE(string_equal(readbuffer, writestr.length - 5, writestr.str + 5, writestr.length - 5));
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), 315);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_seek(stream, 16, STREAM_SEEK_CURRENT);
	EXPECT_SIZEEQ(stream_read(stream, readbuffer, 1024), 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), 315);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_truncate(stream, 2048);
	EXPECT_FALSE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 1024);
	EXPECT_EQ(stream_tell(stream), 315);
	EXPECT_EQ(stream_available_read(stream), 1024 - 315);

	stream_flush(stream);
	EXPECT_EQ(stream_read(stream, readbuffer, 1024), 1024 - 315);
	EXPECT_EQ(stream_write(stream, writebuffer, 1024), 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 1024);
	EXPECT_EQ(stream_tell(stream), 1024);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_truncate(stream, 8);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 8);
	EXPECT_EQ(stream_tell(stream), 8);
	EXPECT_EQ(stream_available_read(stream), 0);

	stream_deallocate(stream);

	stream = buffer_stream_allocate(backing_store, STREAM_IN | STREAM_OUT | STREAM_ATEND, 315, 1024, false, false);
	EXPECT_NE(stream, 0);
	EXPECT_TRUE(stream_eos(stream));
	EXPECT_EQ(stream_size(stream), 315);
	EXPECT_EQ(stream_tell(stream), 315);

	stream_deallocate(stream);

	memory_deallocate(backing_store);

	return 0;
}

static void
test_bufferstream_declare(void) {
	ADD_TEST(bufferstream, null);
	ADD_TEST(bufferstream, zero);
	ADD_TEST(bufferstream, null_grow);
	ADD_TEST(bufferstream, zero_grow);
	ADD_TEST(bufferstream, zero_nogrow);
	ADD_TEST(bufferstream, sized_grow);
	ADD_TEST(bufferstream, sized_nogrow);
}

static test_suite_t test_bufferstream_suite = {test_bufferstream_application,
                                               test_bufferstream_memory_system,
                                               test_bufferstream_config,
                                               test_bufferstream_declare,
                                               test_bufferstream_initialize,
                                               test_bufferstream_finalize,
                                               0};

#if BUILD_MONOLITHIC

int
test_bufferstream_run(void);

int
test_bufferstream_run(void) {
	test_suite = test_bufferstream_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_bufferstream_suite;
}

#endif
