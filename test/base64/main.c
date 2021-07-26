/* main.c  -  Foundation base64 test  -  Public Domain  -  2013 Mattias Jansson
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
test_base64_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation base64 tests"));
	app.short_name = string_const(STRING_CONST("test_base64"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_base64_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_base64_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_base64_initialize(void) {
	return 0;
}

static void
test_base64_finalize(void) {
}

DECLARE_TEST(base64, encode_decode) {
	char test_string[2048];
	uint64_t test_data64[128];
	uint64_t verify_data64[128];
	size_t written;
	char prev_value;
	char guard_value;
	char* test_data = (void*)test_data64;
	char* verify_data = (void*)verify_data64;

	for (written = 0; written < 128; ++written)
		test_data64[written] = random64();

	// Test encode/decode zero/one blocks
	{
		written = base64_encode(test_data, 1, test_string, 1);
		EXPECT_EQ(written, 1);
		EXPECT_EQ(test_string[0], 0);

		guard_value = test_data[0];
		written = base64_decode(test_string, 0, test_data, 1);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(guard_value, test_data[0]);

		guard_value = test_data[0];
		written = base64_decode(test_string, 1, test_data, 1);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(guard_value, test_data[0]);

		written = base64_encode(test_data, 1, test_string, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(guard_value, test_data[0]);

		written = base64_encode(test_data, 1, test_string, 5);
		EXPECT_EQ(written, 5);
		EXPECT_EQ(test_string[2], '=');
		EXPECT_EQ(test_string[3], '=');
		EXPECT_EQ(test_string[4], 0);

		prev_value = test_data[0];
		guard_value = test_data[1];
		test_data[0] = ~prev_value;
		written = base64_decode(test_string, 4, test_data, 1);
		EXPECT_EQ(written, 1);
		EXPECT_EQ(prev_value, test_data[0]);
		EXPECT_EQ(guard_value, test_data[1]);

		prev_value = test_data[0];
		guard_value = test_data[1];
		test_data[0] = ~prev_value;
		written = base64_decode(test_string, 0, test_data, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ((char)~(unsigned char)prev_value, test_data[0]);
		EXPECT_EQ(guard_value, test_data[1]);

		written = base64_decode(test_string, 0, test_data, 1);
		EXPECT_EQ(written, 0);
		EXPECT_EQ((char)~(unsigned char)prev_value, test_data[0]);
		EXPECT_EQ(guard_value, test_data[1]);

		written = base64_decode(test_string, string_length(test_string), test_data, 1);
		EXPECT_EQ(written, 1);
		EXPECT_EQ(prev_value, test_data[0]);
		EXPECT_EQ(guard_value, test_data[1]);
	}

	// Test encode/decode larger blocks
	{
		written = base64_encode(test_data, 32, test_string, 8);
		EXPECT_EQ(written, 5);
		EXPECT_EQ(test_string[4], 0);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 32, test_string, 9);
		EXPECT_EQ(written, 9);
		EXPECT_EQ(test_string[8], 0);
		EXPECT_EQ(test_string[0], prev_value);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 32, test_string, 10);
		EXPECT_EQ(written, 9);
		EXPECT_EQ(test_string[8], 0);
		EXPECT_EQ(test_string[0], prev_value);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 31, test_string, 45);
		EXPECT_EQ(written, 45);
		EXPECT_EQ(test_string[44], 0);
		EXPECT_EQ(test_string[0], prev_value);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 32, test_string, 45);
		EXPECT_EQ(written, 45);
		EXPECT_EQ(test_string[44], 0);
		EXPECT_EQ(test_string[0], prev_value);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 33, test_string, 45);
		EXPECT_EQ(written, 45);
		EXPECT_EQ(test_string[44], 0);
		EXPECT_EQ(test_string[0], prev_value);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 32, test_string, 64);
		EXPECT_EQ(written, 45);
		EXPECT_EQ(test_string[44], 0);
		EXPECT_EQ(test_string[0], prev_value);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 31, test_string, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(test_string[0], (char)~(unsigned char)prev_value);

		written = base64_encode(test_data, 31, test_string, 45);
		EXPECT_EQ(written, 45);
		EXPECT_EQ(test_string[44], 0);
		EXPECT_EQ(test_string[0], prev_value);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 44, verify_data, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(verify_data[0], 0);

		written = base64_decode(test_string, 44, verify_data, 31);
		EXPECT_EQ(written, 31);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 32, test_string, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(test_string[0], (char)~(unsigned char)prev_value);

		written = base64_encode(test_data, 32, test_string, 45);
		EXPECT_EQ(written, 45);
		EXPECT_EQ(test_string[44], 0);
		EXPECT_EQ(test_string[0], prev_value);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 44, verify_data, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(verify_data[0], 0);

		written = base64_decode(test_string, 44, verify_data, 32);
		EXPECT_EQ(written, 32);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 33, test_string, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(test_string[0], (char)~(unsigned char)prev_value);

		written = base64_encode(test_data, 33, test_string, 64);
		EXPECT_EQ(written, 45);
		EXPECT_EQ(string_length(test_string), 44);
		EXPECT_EQ(test_string[44], 0);
		EXPECT_EQ(test_string[0], prev_value);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 44, verify_data, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(verify_data[0], 0);

		written = base64_decode(test_string, 44, verify_data, 44);
		EXPECT_EQ(written, 33);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);
		EXPECT_EQ(verify_data[34], 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 44, verify_data, 33);
		EXPECT_EQ(written, 33);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 44, verify_data, 7);
		EXPECT_EQ(written, 7);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 8, verify_data, 33);
		EXPECT_EQ(written, 6);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 0, verify_data, 33);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(verify_data[0], 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, string_length(test_string), verify_data, 33);
		EXPECT_EQ(written, 33);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 0, verify_data, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(verify_data[0], 0);

		written = base64_decode(test_string, 0, verify_data, 33);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(verify_data[0], 0);

		written = base64_decode(test_string, string_length(test_string), verify_data, 33);
		EXPECT_EQ(written, 33);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode(test_data, 1024, test_string, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(test_string[0], (char)~(unsigned char)prev_value);

		prev_value = test_string[5];
		written = base64_encode(test_data, 1024, test_string, 1370);
		EXPECT_EQ(written, 1369);
		EXPECT_EQ(test_string[1368], 0);
		EXPECT_EQ(test_string[5], prev_value);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 1369, verify_data, 0);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(verify_data[0], 0);

		written = base64_decode(test_string, 1369, verify_data, 1024);
		EXPECT_EQ(written, 1024);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 1368, verify_data, 1024);
		EXPECT_EQ(written, 1024);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 982, verify_data, 519);
		EXPECT_EQ(written, 519);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 378, verify_data, 985);
		EXPECT_EQ(written, 283);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, 0, verify_data, 1024);
		EXPECT_EQ(written, 0);
		EXPECT_EQ(verify_data[0], 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, string_length(test_string), verify_data, 1024);
		EXPECT_EQ(written, 1024);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);

		memset(verify_data, 0, 1024);
		written = base64_decode(test_string, string_length(test_string), verify_data, 1);
		EXPECT_EQ(written, 1);
		EXPECT_EQ(memcmp(test_data, verify_data, written), 0);
	}

	return 0;
}

static void
test_base64_declare(void) {
	ADD_TEST(base64, encode_decode);
}

static test_suite_t test_base64_suite = {test_base64_application,
                                         test_base64_memory_system,
                                         test_base64_config,
                                         test_base64_declare,
                                         test_base64_initialize,
                                         test_base64_finalize,
                                         0};

#if BUILD_MONOLITHIC

int
test_base64_run(void);

int
test_base64_run(void) {
	test_suite = test_base64_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_base64_suite;
}

#endif
