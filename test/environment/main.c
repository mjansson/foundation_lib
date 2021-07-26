/* main.c  -  Foundation environment test  -  Public Domain  -  2013 Mattias Jansson
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
test_environment_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation environment tests"));
	app.short_name = string_const(STRING_CONST("test_environment"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_environment_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_environment_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_environment_initialize(void) {
	return 0;
}

static void
test_environment_finalize(void) {
}

DECLARE_TEST(environment, builtin) {
	const string_const_t* cmdline = environment_command_line();

	EXPECT_GE(array_size(cmdline), 1);
#if !BUILD_MONOLITHIC
	EXPECT_NE_MSGFORMAT(string_find_string(STRING_ARGS(cmdline[0]), STRING_CONST("test-environment"), 0), STRING_NPOS,
	                    "Commandline: %.*s", (int)cmdline[0].length, cmdline[0].str);
	EXPECT_CONSTSTRINGEQ(environment_executable_name(), string_const(STRING_CONST("test-environment")));
#elif FOUNDATION_PLATFORM_ANDROID
	EXPECT_NE_MSGFORMAT(string_find_string(STRING_ARGS(cmdline[0]), STRING_CONST("com.maniccoder.foundation.test"), 0),
	                    STRING_NPOS, "Commandline: %.*s", (int)cmdline[0].length, cmdline[0].str);
#else
	EXPECT_NE_MSGFORMAT(string_find_string(STRING_ARGS(cmdline[0]), STRING_CONST("test-all"), 0), STRING_NPOS,
	                    "Commandline: %.*s", (int)cmdline[0].length, cmdline[0].str);
	EXPECT_CONSTSTRINGEQ(environment_executable_name(), string_const(STRING_CONST("test-all")));
#endif
	EXPECT_NE(environment_initial_working_directory().str, 0);
	EXPECT_NE(environment_initial_working_directory().length, 0);
	EXPECT_NE(environment_current_working_directory().str, 0);
	EXPECT_NE(environment_current_working_directory().length, 0);

	EXPECT_NE(environment_application_directory().str, 0);
	EXPECT_NE(environment_application_directory().length, 0);

	EXPECT_NE(environment_temporary_directory().str, 0);
	EXPECT_NE(environment_temporary_directory().length, 0);

	EXPECT_NE(environment_variable(STRING_CONST("PATH")).str, 0);
	EXPECT_NE(environment_variable(STRING_CONST("PATH")).length, 0);

	return 0;
}

DECLARE_TEST(environment, workingdir) {
	char buffer[BUILD_MAX_PATHLEN];
	string_const_t working_dir = environment_current_working_directory();
	string_const_t new_working_dir = path_directory_name(STRING_ARGS(working_dir));
	string_t working_dir_copy = string_clone(STRING_ARGS(working_dir));
	string_t new_working_dir_copy;

	if (string_equal(STRING_ARGS(working_dir), STRING_CONST("/"))) {
		string_t tmpwork = path_make_temporary(buffer, sizeof(buffer));
		new_working_dir = path_directory_name(STRING_ARGS(tmpwork));
	}

#if FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
	new_working_dir_copy = path_allocate_concat(STRING_ARGS(working_dir), STRING_CONST("config"));
#else
	new_working_dir_copy = string_clone(STRING_ARGS(new_working_dir));
#endif
	new_working_dir = string_to_const(new_working_dir_copy);

	EXPECT_CONSTSTRINGNE(working_dir, string_empty());
	EXPECT_CONSTSTRINGNE(working_dir, new_working_dir);

	EXPECT_TRUE(environment_set_current_working_directory(STRING_ARGS(new_working_dir)));
	EXPECT_CONSTSTRINGEQ(environment_current_working_directory(), new_working_dir);

	environment_set_current_working_directory(STRING_ARGS(working_dir_copy));
	EXPECT_CONSTSTRINGEQ(environment_current_working_directory(), string_const(STRING_ARGS(working_dir_copy)));
	{
		log_enable_stdout(false);
		bool ret = environment_set_current_working_directory(STRING_CONST("/invalid/path/which/does/not/exist"));
		log_enable_stdout(true);
		EXPECT_FALSE(ret);
	}
	EXPECT_CONSTSTRINGEQ(environment_current_working_directory(), string_const(STRING_ARGS(working_dir_copy)));

	string_deallocate(new_working_dir_copy.str);
	string_deallocate(working_dir_copy.str);

	return 0;
}

static void
test_environment_declare(void) {
	ADD_TEST(environment, builtin);
	ADD_TEST(environment, workingdir);
}

static test_suite_t test_environment_suite = {test_environment_application,
                                              test_environment_memory_system,
                                              test_environment_config,
                                              test_environment_declare,
                                              test_environment_initialize,
                                              test_environment_finalize,
                                              0};

#if BUILD_MONOLITHIC

int
test_environment_run(void);

int
test_environment_run(void) {
	test_suite = test_environment_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_environment_suite;
}

#endif
