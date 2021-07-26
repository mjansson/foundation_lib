/* main.c  -  Foundation time test  -  Public Domain  -  2013 Mattias Jansson
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
#include <mock/mock.h>

#if FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#endif

static application_t
test_process_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation process tests"));
	app.short_name = string_const(STRING_CONST("test_process"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_process_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_process_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_process_initialize(void) {
	const string_const_t* cmdline = environment_command_line();
	if (string_array_find(cmdline, array_size(cmdline), STRING_CONST("wait for kill")) < 0)
		return 0;

	while (true)
		thread_sleep(100);
}

static void
test_process_finalize(void) {
}

DECLARE_TEST(process, spawn) {
	process_t* proc;
	stream_t* in;
	stream_t* out;
	int exit_code, ret, lines_count;
	bool found_expected;
	bool found_file;
	char line_buffer[512];
#if FOUNDATION_PLATFORM_WINDOWS
	string_const_t prog = environment_variable(STRING_CONST("comspec"));
	string_const_t args[] = {string_const(STRING_CONST("/C")), string_const(STRING_CONST("dir")), string_null()};
#elif FOUNDATION_PLATFORM_POSIX
	string_const_t prog = string_const(STRING_CONST("/bin/ls"));
	string_const_t args[] = {string_const(STRING_CONST("-1")), string_const(STRING_CONST("-la")), string_null()};
#else
	string_const_t prog = string_const(STRING_CONST("notimplemented"));
	string_const_t args[] = {string_const(STRING_CONST("")), string_const(STRING_CONST("")), string_null()};
#endif
	string_t tmp_path;
	string_t full_path;
	stream_t* tmp_file;
	string_const_t fname;
	string_const_t file_name = string_const(STRING_CONST("test-file"));

	if ((system_platform() == PLATFORM_IOS) || (system_platform() == PLATFORM_ANDROID))
		return 0;

	fname = string_from_uint_static((uint32_t)random32(), false, 0, '0');
	tmp_path = path_allocate_concat_varg(STRING_ARGS(environment_temporary_directory()),
	                                     STRING_CONST("path with space"), STRING_ARGS(fname), nullptr);

	EXPECT_TRUE(fs_make_directory(STRING_ARGS(tmp_path)));

	full_path = path_allocate_concat(STRING_ARGS(tmp_path), STRING_ARGS(file_name));
	tmp_file = fs_open_file(STRING_ARGS(full_path), STREAM_CREATE | STREAM_OUT);
	EXPECT_NE(tmp_file, 0);
	stream_deallocate(tmp_file);

	args[2] = string_to_const(tmp_path);

	proc = process_allocate();

	process_set_working_directory(proc, STRING_CONST("/"));
	process_set_executable_path(proc, STRING_ARGS(prog));
	process_set_arguments(proc, args, sizeof(args) / sizeof(args[0]));
	process_set_flags(proc, PROCESS_DETACHED | PROCESS_CONSOLE | PROCESS_STDSTREAMS);
	process_set_verb(proc, STRING_CONST("open"));

	ret = process_spawn(proc);
	EXPECT_INTEQ(ret, PROCESS_STILL_ACTIVE);

	out = process_stdout(proc);
	in = process_stdin(proc);

	EXPECT_NE(out, 0);
	EXPECT_NE(in, 0);

	stream_write_string(in, STRING_CONST("testing"));

	found_expected = false;
	found_file = false;
	lines_count = 0;
	do {
		string_t fline = stream_read_line_buffer(out, line_buffer, 512, '\n');
		string_const_t line = string_strip(STRING_ARGS(fline), STRING_CONST("\n\r"));
		if (line.length) {
			++lines_count;
#if FOUNDATION_PLATFORM_WINDOWS
			if ((string_find_string(STRING_ARGS(line), STRING_CONST("File(s)"), 0) != STRING_NPOS) &&
			    (string_find_string(STRING_ARGS(line), STRING_CONST("bytes"), 0) != STRING_NPOS))
				found_expected = true;
#else
			if ((string_find_string(STRING_ARGS(line), STRING_CONST("drwx"), 0) != STRING_NPOS) &&
			    (string_find_string(STRING_ARGS(line), STRING_CONST(".."), 0) != STRING_NPOS))
				found_expected = true;
#endif
			if (string_find_string(STRING_ARGS(line), STRING_ARGS(file_name), 0) != STRING_NPOS)
				found_file = true;
			log_debugf(HASH_TEST, STRING_CONST("%.*s"), STRING_FORMAT(fline));
		}
	} while (!stream_eos(out));

	EXPECT_INTGE(lines_count, 4);
	EXPECT_TRUE(found_expected);
	EXPECT_TRUE(found_file);

	do {
		exit_code = process_wait(proc);
	} while (exit_code == PROCESS_STILL_ACTIVE);

	EXPECT_EQ(exit_code, 0);

#if FOUNDATION_PLATFORM_WINDOWS

	// PROCESS_WINDOWS_USE_SHELLEXECUTE

#endif

#if FOUNDATION_PLATFORM_MACOS

	process_finalize(proc);
	process_initialize(proc);

	args[0] = string_to_const(tmp_path);

	process_set_working_directory(proc, STRING_CONST("/"));
	process_set_executable_path(proc, STRING_CONST("/System/Library/CoreServices/Finder.app"));
	process_set_flags(proc, PROCESS_DETACHED | PROCESS_MACOS_USE_OPENAPPLICATION);
	process_set_arguments(proc, args, 1);

	ret = process_spawn(proc);
	EXPECT_INTEQ(ret, PROCESS_STILL_ACTIVE);

#endif

	process_deallocate(proc);

	process_set_exit_code(-1);
	EXPECT_EQ(process_exit_code(), -1);
	process_set_exit_code(0);

	string_deallocate(tmp_path.str);
	string_deallocate(full_path.str);

	return 0;
}

DECLARE_TEST(process, kill) {
	process_t* proc;
	string_const_t args[] = {string_const(STRING_CONST("wait for kill")),
	                         string_const(STRING_CONST("another arg")),
	                         string_const(STRING_CONST("1234")),
	                         string_const(STRING_CONST("--")),
	                         string_const(STRING_CONST("----")),
	                         string_const(STRING_CONST("foo")),
	                         string_const(STRING_CONST("bar")),
	                         string_const(STRING_CONST("--foo--")),
	                         string_const(STRING_CONST("- -- - -- -")),
	                         string_const(STRING_CONST("qwerty")),
	                         string_const(STRING_CONST("_!\"\"'")),
	                         string_const(STRING_CONST("   ")),
	                         string_const(STRING_CONST("+-")),
	                         string_const(STRING_CONST(" --")),
	                         string_const(STRING_CONST("^	")),
	                         string_const(STRING_CONST("\\")),
	                         string_const(STRING_CONST("#$|")),
	                         string_const(STRING_CONST("<< >>")),
	                         string_const(STRING_CONST(">/dev/null")),
	                         string_const(STRING_CONST("lastarg")),
	                         string_null()};
	int ret;

	if ((system_platform() == PLATFORM_IOS) || (system_platform() == PLATFORM_ANDROID))
		return 0;

	proc = process_allocate();

	process_set_working_directory(proc, STRING_ARGS(environment_current_working_directory()));
	process_set_executable_path(proc, STRING_ARGS(environment_executable_path()));
	process_set_arguments(proc, args, sizeof(args) / sizeof(args[0]));
	process_set_flags(proc, PROCESS_DETACHED | PROCESS_STDSTREAMS);

	ret = process_spawn(proc);
	EXPECT_INTEQ(ret, PROCESS_STILL_ACTIVE);
	EXPECT_INTEQ(process_wait(proc), PROCESS_STILL_ACTIVE);

	EXPECT_NE(process_stdin(proc), nullptr);
	EXPECT_NE(process_stdout(proc), nullptr);
	EXPECT_NE(process_stderr(proc), nullptr);

	thread_sleep(500);

	EXPECT_TRUE(process_kill(proc));

	thread_sleep(500);

	ret = process_wait(proc);
	EXPECT_INTEQ(ret, PROCESS_TERMINATED_SIGNAL);

	process_set_working_directory(proc, STRING_ARGS(environment_current_working_directory()));
	process_set_executable_path(proc, STRING_CONST("bad path to executable"));
	process_set_arguments(proc, args, sizeof(args) / sizeof(args[0]));
	process_set_flags(proc, PROCESS_ATTACHED);

	log_enable_stdout(false);
	ret = process_spawn(proc);
#if FOUNDATION_PLATFORM_WINDOWS
	EXPECT_INTEQ(ret, PROCESS_INVALID_ARGS);
#else
	EXPECT_INTEQ(ret, PROCESS_EXIT_FAILURE);
#endif

	log_enable_stdout(true);

	EXPECT_FALSE(process_kill(proc));

	process_deallocate(proc);

	return 0;
}

DECLARE_TEST(process, failure) {
	process_t proc;

	process_initialize(&proc);
	EXPECT_EQ(process_spawn(&proc), PROCESS_INVALID_ARGS);
	process_finalize(&proc);

	process_deallocate(0);

	string_const_t args[] = {string_const(STRING_CONST("wait for kill")), string_null()};

	process_initialize(&proc);
	process_set_working_directory(&proc, STRING_ARGS(environment_current_working_directory()));
	process_set_executable_path(&proc, STRING_ARGS(environment_executable_path()));
	process_set_arguments(&proc, args, sizeof(args) / sizeof(args[0]));
	process_set_flags(&proc, PROCESS_DETACHED | PROCESS_STDSTREAMS);

	error_level_t last_log_suppress = log_suppress(0);
	log_set_suppress(0, ERRORLEVEL_ERROR);

#if FOUNDATION_PLATFORM_POSIX && !FOUNDATION_PLATFORM_ANDROID && !FOUNDATION_PLATFORM_IOS
	int ret;

	fork_mock(-1, ENOMEM);
	ret = process_spawn(&proc);
	fork_unmock();
	EXPECT_INTEQ(ret, PROCESS_SYSTEM_CALL_FAILED);

	fork_mock(0, 0);
	execv_mock(0, EINVAL);
	dup2_mock(0, 0);
	jmp_buf exit_target;
	ret = setjmp(exit_target);
	if (ret == 0) {
		exit_mock(exit_target);
		process_spawn(&proc);
	}
	exit_unmock();
	dup2_unmock();
	execv_unmock();
	fork_unmock();
	EXPECT_INTEQ(ret, PROCESS_EXIT_FAILURE);

	ret = process_spawn(&proc);
	thread_sleep(500);
	waitpid_mock(-1, EINTR);
	EXPECT_INTEQ(process_wait(&proc), PROCESS_WAIT_INTERRUPTED);
	waitpid_mock(-1, ECHILD);
	EXPECT_INTEQ(process_wait(&proc), PROCESS_WAIT_FAILED);
	waitpid_unmock();

	EXPECT_TRUE(process_kill(&proc));

	thread_sleep(500);

	ret = process_wait(&proc);
	EXPECT_INTEQ(ret, PROCESS_TERMINATED_SIGNAL);

#endif

	log_set_suppress(0, last_log_suppress);

	process_finalize(&proc);

	return 0;
}

static void
test_process_declare(void) {
	ADD_TEST(process, spawn);
	ADD_TEST(process, kill);
	ADD_TEST(process, failure);
}

static test_suite_t test_process_suite = {test_process_application,
                                          test_process_memory_system,
                                          test_process_config,
                                          test_process_declare,
                                          test_process_initialize,
                                          test_process_finalize,
                                          0};

#if BUILD_MONOLITHIC

int
test_process_run(void);

int
test_process_run(void) {
	test_suite = test_process_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_process_suite;
}

#endif
