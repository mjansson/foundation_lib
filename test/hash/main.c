/* main.c  -  Foundation hash test  -  Public Domain  -  2013 Mattias Jansson
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
test_hash_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation hash tests"));
	app.short_name = string_const(STRING_CONST("test_hash"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_hash_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_hash_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	config.hash_store_size = 32 * 1024;
	return config;
}

static int
test_hash_initialize(void) {
	return 0;
}

static void
test_hash_finalize(void) {
}

DECLARE_TEST(hash, known) {
	EXPECT_EQ(hash(STRING_CONST("engine")), 0x39c8cc157cfd24f8ULL);
	EXPECT_EQ(hash(STRING_CONST("enable_remote_debugger")), 0xb760826929ca10a3ULL);
	EXPECT_EQ(hash(STRING_CONST("enable_profiler")), 0xaa75bf69e488ba1cULL);
	EXPECT_EQ(hash(STRING_CONST("cache_directory")), 0x3e7b4931a3841da8ULL);
	EXPECT_EQ(hash(STRING_CONST("server_address")), 0x64fcf494cf8072f5ULL);
	EXPECT_EQ(hash(STRING_CONST("server_port")), 0xdd32e17d082c2959ULL);
	return 0;
}

DECLARE_TEST(hash, store) {
#if BUILD_ENABLE_STATIC_HASH_DEBUG
	string_const_t foundation = hash_to_string(HASH_FOUNDATION);
	EXPECT_CONSTSTRINGEQ(foundation, string_const(STRING_CONST("foundation")));
#endif
	return 0;
}

DECLARE_TEST(hash, stability) {
	// TODO: Implement a proper test instead of this crap
	size_t i, j, k, len;
	hash_t lhash, rhash, rhashref;

#if FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
	size_t mult = 200;
	size_t passes = 64;
#else
	size_t mult = 1000;
	size_t passes = 128;
#endif

	for (i = 0; i < passes; ++i) {
		uint32_t lhs[129], rhs[129];
		len = i + 1;

		for (k = 0; k < len; ++k)
			lhs[k] = random32();

		lhash = hash(lhs, len * sizeof(uint32_t));
		EXPECT_NE(lhash, 0U);

		for (j = 0; j < 64 * mult; ++j) {
			for (k = 0; k < len; ++k)
				rhs[k] = random32();

			rhashref = hash(rhs, len * sizeof(uint32_t));
			rhash = hash(rhs, len * sizeof(uint32_t));

			EXPECT_EQ(rhashref, rhash);
			if (memcmp(lhs, rhs, len * sizeof(uint32_t)))
				EXPECT_NE(lhash, rhash);
			EXPECT_NE(rhash, 0U);
		}
	}

	for (i = 4; i < passes; ++i) {
		char lhs[130], rhs[130];
		len = i + 1;

		lhs[0] = 'f';
		lhs[1] = 'n';
		lhs[2] = 'd';
		lhs[3] = '_';
		rhs[0] = 'f';
		rhs[1] = 'n';
		rhs[2] = 'd';
		rhs[3] = '_';

		for (k = 4; k < len; ++k)
			lhs[k] = (char)random32_range(32, 128);
		lhs[len] = 0;

		lhash = hash(lhs, len);
		EXPECT_NE(lhash, 0U);

		for (j = 0; j < 128 * mult; ++j) {
			for (k = 4; k < len; ++k)
				rhs[k] = (char)random32_range(32, 128);
			rhs[len] = 0;

			rhashref = hash(rhs, len);
			rhash = hash(rhs, len);

			EXPECT_EQ(rhashref, rhash);
			if (!string_equal(lhs, len, rhs, len))
				EXPECT_NE(lhash, rhash);
			EXPECT_NE(rhash, 0U);
		}
	}
	return 0;
}

static void
test_hash_declare(void) {
	ADD_TEST(hash, known);
	ADD_TEST(hash, store);
	ADD_TEST(hash, stability);
}

static test_suite_t test_hash_suite = {test_hash_application,
                                       test_hash_memory_system,
                                       test_hash_config,
                                       test_hash_declare,
                                       test_hash_initialize,
                                       test_hash_finalize,
                                       0};

#if BUILD_MONOLITHIC

int
test_hash_run(void);

int
test_hash_run(void) {
	test_suite = test_hash_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_hash_suite;
}

#endif
