/* main.c  -  Foundation library test  -  Public Domain  -  2013 Mattias Jansson
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
test_library_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation library tests"));
	app.short_name = string_const(STRING_CONST("test_library"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_library_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_library_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_library_initialize(void) {
	return 0;
}

static void
test_library_finalize(void) {
}

DECLARE_TEST(library, lookup) {
	object_t lib = 0;
	object_t otherlib = 0;
	void* symbol = 0;
	string_const_t libraryname;
	string_const_t symbolname;

#if FOUNDATION_PLATFORM_WINDOWS
	libraryname = string_const(STRING_CONST("kernel32"));
	symbolname = string_const(STRING_CONST("ExitProcess"));
#elif FOUNDATION_PLATFORM_APPLE
	libraryname = string_const(STRING_CONST("dl"));
	symbolname = string_const(STRING_CONST("dlsym"));
#elif FOUNDATION_PLATFORM_BSD
	libraryname = string_const(STRING_CONST("execinfo"));
	symbolname = string_const(STRING_CONST("backtrace"));
#elif FOUNDATION_PLATFORM_POSIX
	libraryname = string_const(STRING_CONST("dl"));
	symbolname = string_const(STRING_CONST("dlsym"));
#else
	libraryname = string_const(STRING_CONST("somelib"));
	symbolname = string_const(STRING_CONST("somesym"));
#endif

	lib = library_load(STRING_ARGS(libraryname));
	EXPECT_NE(lib, 0);

	otherlib = library_load(STRING_ARGS(libraryname));
	EXPECT_EQ(lib, otherlib);

	library_release(otherlib);
	library_release(0);
	otherlib = 0;

	log_set_suppress(0, ERRORLEVEL_WARNING);
	EXPECT_EQ(library_load(STRING_CONST("this_library_should_not_exist")), 0);
	log_set_suppress(0, ERRORLEVEL_DEBUG);

	EXPECT_TRUE(library_valid(lib));
	EXPECT_FALSE(library_valid(0));

	symbol = library_symbol(lib, STRING_ARGS(symbolname));
	EXPECT_NE(symbol, 0);

	EXPECT_EQ(library_symbol(0, STRING_ARGS(symbolname)), 0);

	library_release(lib);
	EXPECT_EQ(library_symbol(lib, STRING_ARGS(symbolname)), 0);

	EXPECT_FALSE(library_valid(lib));

	return 0;
}

static void
test_library_declare(void) {
	ADD_TEST(library, lookup);
}

static test_suite_t test_library_suite = {test_library_application,
                                          test_library_memory_system,
                                          test_library_config,
                                          test_library_declare,
                                          test_library_initialize,
                                          test_library_finalize,
                                          0};

#if BUILD_MONOLITHIC

int
test_library_run(void);

int
test_library_run(void) {
	test_suite = test_library_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_library_suite;
}

#endif
