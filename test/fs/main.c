/* main.c  -  Foundation filesystem test  -  Public Domain  -  2013 Mattias Jansson
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
test_fs_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation filesystem tests"));
	app.short_name = string_const(STRING_CONST("test_filesystem"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_fs_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_fs_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	config.fs_monitor_max = 1;
	return config;
}

static int
test_fs_initialize(void) {
	string_const_t tempdir = environment_temporary_directory();
	fs_make_directory(STRING_ARGS(tempdir));
	environment_set_current_working_directory(STRING_ARGS(tempdir));
	return 0;
}

static void
test_fs_finalize(void) {
}

static void
unterminate(char* str, size_t length) {
	str[length] = '>';
}

DECLARE_TEST(fs, directory) {
	char buf[BUILD_MAX_PATHLEN];
	string_t longpath;
	string_const_t fname;
	string_t testpath;
	string_const_t subpath;
	string_t testlocalpath;

	fname = string_from_uint_static(random64(), true, 0, 0);
	testpath = path_concat(buf, BUILD_MAX_PATHLEN, STRING_ARGS(environment_temporary_directory()), STRING_ARGS(fname));
	unterminate(STRING_ARGS(testpath));

	if (fs_is_file(STRING_ARGS(testpath)))
		fs_remove_file(STRING_ARGS(testpath));
	if (!fs_is_directory(STRING_ARGS(testpath)))
		fs_make_directory(STRING_ARGS(testpath));

	EXPECT_FALSE(fs_is_file(STRING_CONST("")));
	EXPECT_FALSE(fs_is_directory(STRING_CONST("")));

	EXPECT_TRUE(fs_is_directory(STRING_ARGS(testpath)));

	EXPECT_TRUE(fs_remove_directory(STRING_ARGS(testpath)));
	EXPECT_FALSE(fs_is_directory(STRING_ARGS(testpath)));

	EXPECT_FALSE(fs_remove_directory(STRING_ARGS(testpath)));

	EXPECT_FALSE(fs_make_directory(STRING_CONST("")));
	EXPECT_FALSE(fs_remove_file(STRING_CONST("")));

	testlocalpath = string_clone(STRING_CONST("local.path"));
	unterminate(STRING_ARGS(testlocalpath));

	if (!fs_is_directory(STRING_ARGS(testlocalpath)))
		EXPECT_TRUE(fs_make_directory(STRING_ARGS(testlocalpath)));
	EXPECT_TRUE(fs_is_directory(STRING_ARGS(testlocalpath)));

	EXPECT_TRUE(fs_remove_directory(STRING_ARGS(testlocalpath)));
	EXPECT_FALSE(fs_is_directory(STRING_ARGS(testlocalpath)));

	EXPECT_FALSE(fs_remove_directory(STRING_ARGS(testlocalpath)));
	string_deallocate(testlocalpath.str);

	fname = string_from_uint_static(random64(), true, 0, 0);
	longpath = path_append(STRING_ARGS(testpath), BUILD_MAX_PATHLEN, STRING_ARGS(fname));
	EXPECT_FALSE(fs_is_directory(STRING_ARGS(longpath)));
	unterminate(STRING_ARGS(longpath));

	EXPECT_TRUE(fs_make_directory(STRING_ARGS(longpath)));
	EXPECT_TRUE(fs_is_directory(STRING_ARGS(longpath)));

	subpath = path_directory_name(STRING_ARGS(longpath));

	EXPECT_TRUE(fs_remove_directory(STRING_ARGS(subpath)));
	EXPECT_FALSE(fs_is_directory(STRING_ARGS(subpath)));

	EXPECT_FALSE(fs_is_directory(STRING_ARGS(longpath)));

	log_enable_stdout(false);
	EXPECT_FALSE(fs_make_directory(STRING_CONST("/../@this[*]is{?}not:an~allowed;name")));
	log_enable_stdout(true);

	return 0;
}

DECLARE_TEST(fs, file) {
	char buf[BUILD_MAX_PATHLEN];
	char copybuf[BUILD_MAX_PATHLEN];
	char data[1024];
	string_const_t fname;
	string_t testpath;
	string_t copypath;
	string_t testlocalpath;
	stream_t* teststream;

	fname = string_from_uint_static(random64(), true, 0, 0);
	testpath = path_concat(buf, BUILD_MAX_PATHLEN, STRING_ARGS(environment_temporary_directory()), STRING_ARGS(fname));
	unterminate(STRING_ARGS(testpath));

	fname = string_from_uint_static(random64(), true, 0, 0);
	copypath =
	    path_concat(copybuf, BUILD_MAX_PATHLEN, STRING_ARGS(environment_temporary_directory()), STRING_ARGS(fname));
	unterminate(STRING_ARGS(copypath));

	if (!fs_is_directory(STRING_ARGS(environment_temporary_directory())))
		fs_make_directory(STRING_ARGS(environment_temporary_directory()));

	if (fs_is_directory(STRING_ARGS(testpath)))
		fs_remove_directory(STRING_ARGS(testpath));
	fs_remove_file(STRING_ARGS(testpath));

	if (fs_is_directory(STRING_ARGS(copypath)))
		fs_remove_directory(STRING_ARGS(copypath));
	fs_remove_file(STRING_ARGS(copypath));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN);
	EXPECT_EQ(teststream, 0);
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT);
	EXPECT_EQ(teststream, 0);
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN | STREAM_OUT);
	EXPECT_EQ(teststream, 0);
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN | STREAM_CREATE);
	EXPECT_NE(teststream, 0);
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));
	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(testpath));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_CREATE);
	EXPECT_NE(teststream, 0);
	memset(data, 0, sizeof(data));
	stream_write(teststream, data, sizeof(data));
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data));
	stream_deallocate(teststream);
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN);
	EXPECT_NE(teststream, 0);
	EXPECT_SIZEEQ(stream_tell(teststream), 0);
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data));
	EXPECT_SIZEEQ(stream_tell(teststream), 0);
	stream_deallocate(teststream);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_TRUNCATE);
	EXPECT_EQ(teststream, 0);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_ATEND);
	EXPECT_NE(teststream, 0);
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data));
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data));
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data));
	stream_write(teststream, data, sizeof(data));
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data) * 2);
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data) * 2);
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data) * 2);
	stream_deallocate(teststream);

	// Vefify truncate is ignored for read-only files
	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN | STREAM_TRUNCATE | STREAM_ATEND);
	EXPECT_NE(teststream, 0);
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data) * 2);
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data) * 2);
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data) * 2);
	stream_deallocate(teststream);

	EXPECT_TRUE(fs_remove_file(STRING_ARGS(testpath)));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN | STREAM_TRUNCATE);
	EXPECT_EQ(teststream, nullptr);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_TRUNCATE);
	EXPECT_EQ(teststream, nullptr);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_CREATE);
	EXPECT_NE(teststream, 0);
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));
	stream_write(teststream, data, sizeof(data));
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data));
	stream_deallocate(teststream);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_TRUNCATE);
	EXPECT_NE(teststream, 0);
	EXPECT_SIZEEQ(stream_tell(teststream), 0);
	EXPECT_SIZEEQ(stream_size(teststream), 0);
	EXPECT_SIZEEQ(stream_tell(teststream), 0);
	stream_deallocate(teststream);

	EXPECT_TRUE(fs_remove_file(STRING_ARGS(testpath)));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_CREATE);
	EXPECT_NE(teststream, 0);
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));
	stream_write(teststream, data, sizeof(data));
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data));
	stream_deallocate(teststream);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN | STREAM_OUT | STREAM_TRUNCATE);
	EXPECT_SIZEEQ(stream_tell(teststream), 0);
	EXPECT_SIZEEQ(stream_size(teststream), 0);
	EXPECT_SIZEEQ(stream_tell(teststream), 0);
	stream_write(teststream, data, sizeof(data));
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data));
	stream_deallocate(teststream);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_ATEND);
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data));
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data));
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data));
	stream_write(teststream, data, 1);
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data) + 1);
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data) + 1);
	EXPECT_SIZEEQ(stream_tell(teststream), sizeof(data) + 1);
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	stream_write(teststream, data, 1);
	EXPECT_SIZEEQ(stream_tell(teststream), 1);
	EXPECT_SIZEEQ(stream_size(teststream), sizeof(data) + 1);
	EXPECT_SIZEEQ(stream_tell(teststream), 1);
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_SIZEEQ(stream_available_read(teststream), sizeof(data) + 1);
	stream_deallocate(teststream);

	EXPECT_TRUE(fs_remove_file(STRING_ARGS(testpath)));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	EXPECT_FALSE(fs_remove_file(STRING_ARGS(testpath)));
	EXPECT_FALSE(fs_remove_file(STRING_CONST("/this/path/should/not/exist")));

	testlocalpath = string_clone(STRING_CONST("test.local.file.path"));
	unterminate(STRING_ARGS(testlocalpath));
#if !FOUNDATION_PLATFORM_IOS
	teststream = fs_open_file(STRING_ARGS(testlocalpath), STREAM_OUT | STREAM_CREATE);
	EXPECT_NE(teststream, 0);
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testlocalpath)));
	stream_deallocate(teststream);
	EXPECT_TRUE(fs_remove_file(STRING_ARGS(testlocalpath)));
#endif
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testlocalpath)));

	EXPECT_FALSE(fs_remove_file(STRING_ARGS(testlocalpath)));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testlocalpath)));
	string_deallocate(testlocalpath.str);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN | STREAM_OUT | STREAM_CREATE);
	EXPECT_NE(teststream, 0);
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));
	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(testpath));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN);
	EXPECT_EQ(teststream, 0);
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_CREATE);
	EXPECT_NE(teststream, 0);
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));
	stream_deallocate(teststream);
	teststream = 0;

	EXPECT_TRUE(fs_copy_file(STRING_ARGS(testpath), STRING_ARGS(copypath)));
	EXPECT_TRUE(fs_is_file(STRING_ARGS(copypath)));

	fs_remove_file(STRING_ARGS(copypath));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(copypath)));

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT);
	EXPECT_NE(teststream, 0);
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));

	stream_write_string(teststream, STRING_CONST("testing testing"));
	stream_deallocate(teststream);
	teststream = 0;

	EXPECT_TRUE(fs_copy_file(STRING_ARGS(testpath), STRING_ARGS(copypath)));
	EXPECT_TRUE(fs_is_file(STRING_ARGS(copypath)));

	fs_remove_file(STRING_ARGS(copypath));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(copypath)));

	// This will fail on POSIX if you have write access to filesystem root
	log_enable_stdout(false);
	EXPECT_FALSE(fs_copy_file(STRING_ARGS(testpath), STRING_CONST("/../@;:*this/:is/;not=?a-valid<*>name")));
	EXPECT_FALSE(
	    fs_copy_file(STRING_CONST("/does/not/exist/at/all"), STRING_CONST("/../@;:*this/:is/;not=?a-valid<*>name")));
	log_enable_stdout(true);

	fs_remove_file(STRING_ARGS(testpath));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));

	stream_deallocate(teststream);

	return 0;
}

DECLARE_TEST(fs, util) {
	char buf[BUILD_MAX_PATHLEN];
	tick_t systime = time_system();
	tick_t lastmod = 0;
	md5_t nullmd5;
	stream_t* teststream;
	stream_t* cloned;
	string_const_t fname = string_from_uint_static(random64(), true, 0, 0);
	string_t testpath =
	    path_concat(buf, BUILD_MAX_PATHLEN, STRING_ARGS(environment_temporary_directory()), STRING_ARGS(fname));
	unterminate(STRING_ARGS(testpath));

	if (!fs_is_directory(STRING_ARGS(environment_temporary_directory())))
		fs_make_directory(STRING_ARGS(environment_temporary_directory()));

	if (fs_is_directory(STRING_ARGS(testpath)))
		fs_remove_directory(STRING_ARGS(testpath));
	fs_remove_file(STRING_ARGS(testpath));

	EXPECT_EQ(fs_last_modified(STRING_ARGS(testpath)), 0);

	thread_sleep(2000);  // For fs time granularity, make sure at least one second passed since systime

	stream_deallocate(fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_CREATE));
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));
	EXPECT_GE(fs_last_modified(STRING_ARGS(testpath)), systime);

	fs_remove_file(STRING_ARGS(testpath));
	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));
	EXPECT_EQ(fs_last_modified(STRING_ARGS(testpath)), 0);

	stream_deallocate(fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_CREATE));
	EXPECT_TRUE(fs_is_file(STRING_ARGS(testpath)));
	EXPECT_GE(fs_last_modified(STRING_ARGS(testpath)), systime);

	lastmod = fs_last_modified(STRING_ARGS(testpath));

	thread_sleep(5000);

	EXPECT_EQ(fs_last_modified(STRING_ARGS(testpath)), lastmod);

	fs_touch(STRING_ARGS(testpath));
	EXPECT_GT(fs_last_modified(STRING_ARGS(testpath)), lastmod);
	EXPECT_EQ(fs_size(STRING_ARGS(testpath)), 0);

	lastmod = fs_last_modified(STRING_ARGS(testpath));

	md5_initialize(&nullmd5);
	md5_digest_finalize(&nullmd5);
	EXPECT_TRUE(uint128_equal(md5_get_digest_raw(&nullmd5), fs_md5(STRING_ARGS(testpath))));
	md5_finalize(&nullmd5);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_CREATE);
	lastmod = fs_last_modified(STRING_ARGS(testpath));

	thread_sleep(1500);
	stream_write_string(teststream, STRING_CONST("foobar barfoo"));
	stream_flush(teststream);
	EXPECT_EQ(stream_size(teststream), 13);
	stream_deallocate(teststream);

	EXPECT_GT(fs_last_modified(STRING_ARGS(testpath)), lastmod);
	EXPECT_EQ(fs_size(STRING_ARGS(testpath)), 13);

	lastmod = fs_last_modified(STRING_ARGS(testpath));

	md5_initialize(&nullmd5);
	md5_digest(&nullmd5, STRING_CONST("foobar barfoo"));
	md5_digest_finalize(&nullmd5);
	EXPECT_TRUE(uint128_equal(md5_get_digest_raw(&nullmd5), fs_md5(STRING_ARGS(testpath))));
	md5_finalize(&nullmd5);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_SYNC);

	stream_truncate(teststream, 100);
	EXPECT_EQ(stream_size(teststream), 13);
	EXPECT_EQ(stream_last_modified(teststream), lastmod);
	thread_sleep(1500);
	stream_truncate(teststream, 4);
	EXPECT_EQ(stream_size(teststream), 4);

	stream_deallocate(teststream);

	teststream = fs_open_file(STRING_ARGS(testpath), STREAM_IN);
	cloned = stream_clone(teststream);
	EXPECT_NE(cloned, nullptr);
	EXPECT_SIZEEQ(stream_size(cloned), 4U);
	EXPECT_CONSTSTRINGEQ(stream_path(teststream), stream_path(cloned));

	stream_deallocate(teststream);
	stream_deallocate(cloned);

	EXPECT_GT(fs_last_modified(STRING_ARGS(testpath)), lastmod);

	fs_remove_file(STRING_ARGS(testpath));

	EXPECT_FALSE(fs_is_file(STRING_ARGS(testpath)));
	EXPECT_EQ(fs_size(STRING_ARGS(testpath)), 0);
	EXPECT_TRUE(uint128_equal(uint128_null(), fs_md5(STRING_ARGS(testpath))));

	testpath = path_prepend(STRING_ARGS(testpath), sizeof(buf), STRING_CONST("http://"));
	EXPECT_EQ(fs_open_file(STRING_ARGS(testpath), STREAM_OUT | STREAM_CREATE), nullptr);

	return 0;
}

DECLARE_TEST(fs, query) {
	uint64_t subpathid = random64();
	uint64_t subfileid = random64();
	string_const_t fname;
	string_t filename;
	string_t testpath;
	string_t verifypath;
	string_t subtestpath;
	string_t filepath[8];
	string_t* subdirs;
	string_t* files;
	int ifp = 0;
	string_t subfilepath;
	char buf[32];

	fname = string_from_uint_static(random64(), true, 0, 0);
	testpath = path_allocate_concat(STRING_ARGS(environment_temporary_directory()), STRING_ARGS(fname));
	unterminate(STRING_ARGS(testpath));
	fname = string_from_uint_static(subpathid, true, 0, 0);
	subtestpath = path_allocate_concat(STRING_ARGS(testpath), STRING_ARGS(fname));
	unterminate(STRING_ARGS(subtestpath));

	if (fs_is_file(STRING_ARGS(testpath)))
		fs_remove_file(STRING_ARGS(testpath));
	if (!fs_is_directory(STRING_ARGS(testpath)))
		fs_make_directory(STRING_ARGS(testpath));
	if (!fs_is_directory(STRING_ARGS(subtestpath)))
		fs_make_directory(STRING_ARGS(subtestpath));

	for (ifp = 0; ifp < 8; ++ifp) {
		filename = string_from_int(buf + 1, 31, ifp, 0, 0);
		buf[0] = '.';
		--filename.str;
		++filename.length;

		fname = string_from_uint_static(random64(), true, 0, 0);
		filepath[ifp] = string_allocate_concat_varg(STRING_ARGS(testpath), STRING_CONST("/"), STRING_ARGS(fname),
		                                            STRING_ARGS(filename), nullptr);
		unterminate(STRING_ARGS(filepath[ifp]));
		stream_deallocate(fs_open_file(STRING_ARGS(filepath[ifp]), STREAM_OUT | STREAM_CREATE));
	}

	filename = string_from_uint(buf, 32, subfileid, true, 0, 0);
	filename = string_append(STRING_ARGS(filename), 32, STRING_CONST(".0"));
	subfilepath = path_allocate_concat(STRING_ARGS(subtestpath), STRING_ARGS(filename));
	unterminate(STRING_ARGS(subfilepath));
	stream_deallocate(fs_open_file(STRING_ARGS(subfilepath), STREAM_OUT | STREAM_CREATE));

	files = fs_files(STRING_ARGS(filepath[0]));
	EXPECT_EQ(array_size(files), 0);
	string_array_deallocate(files);

	subdirs = fs_subdirs(STRING_ARGS(subtestpath));
	EXPECT_EQ(array_size(subdirs), 0);
	string_array_deallocate(subdirs);

	files = fs_files(STRING_ARGS(testpath));
	EXPECT_EQ(array_size(files), 8);
	string_array_deallocate(files);

	subdirs = fs_subdirs(STRING_ARGS(testpath));
	EXPECT_EQ(array_size(subdirs), 1);
	string_array_deallocate(subdirs);

	files = fs_matching_files(STRING_ARGS(testpath), STRING_CONST("^.*$"), false);
	EXPECT_EQ(array_size(files), 8);
	string_array_deallocate(files);

	files = fs_matching_files(STRING_ARGS(testpath), STRING_CONST("^.*$"), true);
	EXPECT_EQ(array_size(files), 9);
	string_array_deallocate(files);

	files = fs_matching_files(STRING_ARGS(testpath), STRING_CONST("^.*\\.0$"), false);
	EXPECT_EQ(array_size(files), 1);
	string_array_deallocate(files);

	files = fs_matching_files(STRING_ARGS(testpath), STRING_CONST("^.*\\.0$"), true);
	EXPECT_EQ(array_size(files), 2);
	string_array_deallocate(files);

	files = fs_matching_files(STRING_ARGS(testpath), STRING_CONST("^.*\\.1$"), false);
	EXPECT_EQ(array_size(files), 1);
	string_array_deallocate(files);

	files = fs_matching_files(STRING_ARGS(testpath), STRING_CONST("^.*\\.1$"), true);
	EXPECT_EQ(array_size(files), 1);
	string_array_deallocate(files);

	files = fs_matching_files(STRING_ARGS(testpath), STRING_CONST("^.*\\..$"), true);
	EXPECT_EQ(array_size(files), 9);
	{
		fname = string_from_uint_static(subpathid, true, 0, 0);
		filename = string_from_uint(buf, 32, subfileid, true, 0, 0);
		filename = string_append(STRING_ARGS(filename), 32, STRING_CONST(".0"));
		verifypath = path_allocate_concat(STRING_ARGS(fname), STRING_ARGS(filename));
		unterminate(STRING_ARGS(verifypath));
		EXPECT_STRINGEQ(files[8], string_const(STRING_ARGS(verifypath)));
		string_deallocate(verifypath.str);
	}
	string_array_deallocate(files);

	fs_remove_directory(STRING_ARGS(testpath));

	string_array_deallocate(subdirs);
	string_array_deallocate(files);
	string_deallocate(subfilepath.str);
	for (ifp = 0; ifp < 8; ++ifp)
		string_deallocate(filepath[ifp].str);
	string_deallocate(subtestpath.str);
	string_deallocate(testpath.str);
	return 0;
}

DECLARE_TEST(fs, event) {
	event_stream_t* stream;
	event_block_t* block;
	event_t* event;
	string_const_t payload;

	char pathstr[] = "/some/path/to/some/file/being/tested";

	stream = fs_event_stream();
	EXPECT_NE(stream, 0);

	fs_event_post(FOUNDATIONEVENT_FILE_CREATED, STRING_CONST(pathstr));

	block = event_stream_process(stream);
	event = event_next(block, 0);
	payload = fs_event_path(event);
	EXPECT_NE(event, 0);

	EXPECT_EQ(event->id, FOUNDATIONEVENT_FILE_CREATED);
	EXPECT_CONSTSTRINGEQ(payload, string_const(pathstr, sizeof(pathstr) - 1));

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	return 0;
}

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_ANDROID && !FOUNDATION_PLATFORM_BSD

DECLARE_TEST(fs, monitor) {
	string_const_t fname;
	string_t testpath;
	string_t filetestpath;
	string_t subtestpath;
	string_t filesubtestpath;
	string_t presubtestpath;
	string_t prefilesubtestpath;
	size_t imon, monmax;

#define MULTICOUNT 16
	string_t multisubtestpath[MULTICOUNT];
	string_t multifilesubtestpath[MULTICOUNT][MULTICOUNT];
	bool multifilesubtestfound[MULTICOUNT][MULTICOUNT];
	int isub, ifilesub;
	unsigned int processed;

	stream_t* test_stream;

	event_stream_t* stream;
	event_block_t* block;
	event_t* event;

	fname = string_from_uint_static(random64(), true, 0, 0);
	testpath = path_allocate_concat(STRING_ARGS(environment_temporary_directory()), STRING_ARGS(fname));
	unterminate(STRING_ARGS(testpath));

	fname = string_from_uint_static(random64(), true, 0, 0);
	filetestpath = path_allocate_concat(STRING_ARGS(testpath), STRING_ARGS(fname));
	unterminate(STRING_ARGS(filetestpath));

	fname = string_from_uint_static(random64(), true, 0, 0);
	subtestpath = path_allocate_concat(STRING_ARGS(testpath), STRING_ARGS(fname));
	unterminate(STRING_ARGS(subtestpath));

	fname = string_from_uint_static(random64(), true, 0, 0);
	filesubtestpath = path_allocate_concat(STRING_ARGS(subtestpath), STRING_ARGS(fname));
	unterminate(STRING_ARGS(filesubtestpath));

	fname = string_from_uint_static(random64(), true, 0, 0);
	presubtestpath = path_allocate_concat(STRING_ARGS(testpath), STRING_ARGS(fname));
	unterminate(STRING_ARGS(presubtestpath));

	fname = string_from_uint_static(random64(), true, 0, 0);
	prefilesubtestpath = path_allocate_concat(STRING_ARGS(presubtestpath), STRING_ARGS(fname));
	unterminate(STRING_ARGS(prefilesubtestpath));

	for (isub = 0; isub < MULTICOUNT; ++isub) {
		fname = string_from_uint_static(random64(), false, 0, 0);
		multisubtestpath[isub] = path_allocate_concat(STRING_ARGS(testpath), STRING_ARGS(fname));
		for (ifilesub = 0; ifilesub < MULTICOUNT; ++ifilesub) {
			fname = string_from_uint_static(random64(), true, 0, 0);
			multifilesubtestpath[isub][ifilesub] =
			    path_allocate_concat(STRING_ARGS(multisubtestpath[isub]), STRING_ARGS(fname));
			unterminate(STRING_ARGS(multifilesubtestpath[isub][ifilesub]));
		}
	}

	stream = fs_event_stream();

	fs_remove_directory(STRING_ARGS(testpath));
	EXPECT_TRUE(fs_make_directory(STRING_ARGS(testpath)));
	EXPECT_TRUE(fs_make_directory(STRING_ARGS(presubtestpath)));

	stream_deallocate(fs_open_file(STRING_ARGS(filetestpath), STREAM_OUT | STREAM_CREATE));
	fs_remove_file(STRING_ARGS(filetestpath));

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	for (imon = 0, monmax = foundation_config().fs_monitor_max; imon <= monmax + 1; ++imon) {
		bool did_monitor = fs_monitor(STRING_ARGS(testpath));
#if FOUNDATION_PLATFORM_WINDOWS || FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_MACOS
		EXPECT_TRUE(did_monitor);
#else
		FOUNDATION_UNUSED(did_monitor);
#endif
	}
	log_enable_stdout(false);
	EXPECT_FALSE(fs_monitor(STRING_CONST("/this/should/fail/from/not/enough/monitors")));
	log_enable_stdout(true);
	thread_sleep(1000);

	test_stream = fs_open_file(STRING_ARGS(filetestpath), STREAM_OUT | STREAM_CREATE);
	stream_deallocate(test_stream);
	EXPECT_NE(test_stream, 0);
	thread_sleep(3000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_EQ(event->id, FOUNDATIONEVENT_FILE_CREATED);

	EXPECT_CONSTSTRINGEQ(fs_event_path(event), string_to_const(filetestpath));

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	test_stream = fs_open_file(STRING_ARGS(prefilesubtestpath), STREAM_OUT | STREAM_CREATE);
	stream_deallocate(test_stream);
	EXPECT_NE(test_stream, 0);
	thread_sleep(3000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_EQ(event->id, FOUNDATIONEVENT_FILE_CREATED);

	EXPECT_CONSTSTRINGEQ(fs_event_path(event), string_to_const(prefilesubtestpath));

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	test_stream = fs_open_file(STRING_ARGS(filetestpath), STREAM_IN | STREAM_OUT | STREAM_CREATE);
	stream_write_string(test_stream, STRING_ARGS(filetestpath));
	stream_deallocate(test_stream);
	thread_sleep(3000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_EQ(event->id, FOUNDATIONEVENT_FILE_MODIFIED);

	EXPECT_CONSTSTRINGEQ(fs_event_path(event), string_to_const(filetestpath));

	event = event_next(block, event);
	EXPECT_EQ_MSGFORMAT(event, 0, "event not null (%d : %" PRIsize ")", event->id, event->payload[0]);

	fs_remove_file(STRING_ARGS(filetestpath));
	thread_sleep(3000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_EQ(event->id, FOUNDATIONEVENT_FILE_DELETED);

	EXPECT_CONSTSTRINGEQ(fs_event_path(event), string_to_const(filetestpath));

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	fs_make_directory(STRING_ARGS(subtestpath));
	thread_sleep(3000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	test_stream = fs_open_file(STRING_ARGS(filesubtestpath), STREAM_OUT | STREAM_CREATE);
	stream_deallocate(test_stream);
	EXPECT_NE(test_stream, 0);
	thread_sleep(3000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_EQ(event->id, FOUNDATIONEVENT_FILE_CREATED);

	EXPECT_CONSTSTRINGEQ(fs_event_path(event), string_to_const(filesubtestpath));

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	test_stream = fs_open_file(STRING_ARGS(filesubtestpath), STREAM_IN | STREAM_OUT | STREAM_CREATE);
	stream_write_string(test_stream, STRING_ARGS(filesubtestpath));
	stream_deallocate(test_stream);
	thread_sleep(3000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_EQ(event->id, FOUNDATIONEVENT_FILE_MODIFIED);

	EXPECT_CONSTSTRINGEQ(fs_event_path(event), string_to_const(filesubtestpath));

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	fs_remove_file(STRING_ARGS(filesubtestpath));
	thread_sleep(3000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_NE(event, 0);
	EXPECT_EQ(event->id, FOUNDATIONEVENT_FILE_DELETED);

	EXPECT_CONSTSTRINGEQ(fs_event_path(event), string_to_const(filesubtestpath));

	event = event_next(block, event);
	EXPECT_EQ(event, 0);

	for (isub = 0; isub < MULTICOUNT; ++isub) {
		fs_make_directory(STRING_ARGS(multisubtestpath[isub]));
		for (ifilesub = 0; ifilesub < MULTICOUNT; ++ifilesub) {
			test_stream =
			    fs_open_file(STRING_ARGS(multifilesubtestpath[isub][ifilesub]), STREAM_IN | STREAM_OUT | STREAM_CREATE);
			stream_deallocate(test_stream);
			multifilesubtestfound[isub][ifilesub] = false;
		}
	}
	thread_sleep(3000);

	do {
		thread_sleep(1000);
		block = event_stream_process(stream);
		event = 0;
		processed = 0;
		while ((event = event_next(block, event))) {
			bool found = false;
			char eventstr[256];
			string_const_t evtpath;

			evtpath = fs_event_path(event);

			string_format(eventstr, 256, STRING_CONST("event %d:%d:%d:%d:%.*s"), event->id, event->flags, event->serial,
			              event->size, STRING_FORMAT(evtpath));
			EXPECT_EQ_MSG(event->id, FOUNDATIONEVENT_FILE_CREATED, eventstr);

			for (isub = 0; isub < MULTICOUNT; ++isub) {
				for (ifilesub = 0; ifilesub < MULTICOUNT; ++ifilesub) {
					if (string_equal(STRING_ARGS(multifilesubtestpath[isub][ifilesub]), STRING_ARGS(evtpath))) {
						multifilesubtestfound[isub][ifilesub] = true;
						found = true;
						break;
					}
				}
			}

			EXPECT_TRUE(found);
			++processed;
		}
	} while (processed > 0);

	for (isub = 0; isub < MULTICOUNT; ++isub) {
		for (ifilesub = 0; ifilesub < MULTICOUNT; ++ifilesub) {
			EXPECT_TRUE(multifilesubtestfound[isub][ifilesub]);
		}
	}

	for (isub = 0; isub < MULTICOUNT; ++isub) {
		fs_remove_directory(STRING_ARGS(multisubtestpath[isub]));
		for (ifilesub = 0; ifilesub < MULTICOUNT; ++ifilesub) {
			multifilesubtestfound[isub][ifilesub] = false;
		}
	}
	thread_sleep(3000);

	do {
		thread_sleep(1000);
		block = event_stream_process(stream);
		event = 0;
		processed = 0;
		while ((event = event_next(block, event))) {
			bool found = false;
			char eventstr[256];
			string_const_t evtpath;

			evtpath = fs_event_path(event);
			string_format(eventstr, 256, STRING_CONST("event %d:%d:%d:%d:%.*s"), event->id, event->flags, event->serial,
			              event->size, STRING_FORMAT(evtpath));

			EXPECT_EQ_MSG(event->id, FOUNDATIONEVENT_FILE_DELETED, eventstr);

			for (isub = 0; isub < MULTICOUNT; ++isub) {
				for (ifilesub = 0; ifilesub < MULTICOUNT; ++ifilesub) {
					if (string_equal(STRING_ARGS(multifilesubtestpath[isub][ifilesub]), STRING_ARGS(evtpath))) {
						multifilesubtestfound[isub][ifilesub] = true;
						found = true;
						break;
					}
				}
			}

			EXPECT_TRUE(found);
			++processed;
		}
	} while (processed > 0);

	for (isub = 0; isub < MULTICOUNT; ++isub) {
		for (ifilesub = 0; ifilesub < MULTICOUNT; ++ifilesub) {
			EXPECT_TRUE(multifilesubtestfound[isub][ifilesub]);
		}
	}

	fs_unmonitor(STRING_ARGS(testpath));
	thread_sleep(1000);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	stream_deallocate(fs_open_file(STRING_ARGS(filetestpath), STREAM_OUT | STREAM_CREATE));
	thread_sleep(100);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	fs_remove_file(STRING_ARGS(filetestpath));
	thread_sleep(100);

	block = event_stream_process(stream);
	event = event_next(block, 0);
	EXPECT_EQ(event, 0);

	fs_remove_directory(STRING_ARGS(testpath));

	for (isub = 0; isub < MULTICOUNT; ++isub) {
		string_deallocate(multisubtestpath[isub].str);
		for (ifilesub = 0; ifilesub < MULTICOUNT; ++ifilesub)
			string_deallocate(multifilesubtestpath[isub][ifilesub].str);
	}

	string_deallocate(presubtestpath.str);
	string_deallocate(prefilesubtestpath.str);
	string_deallocate(subtestpath.str);
	string_deallocate(filesubtestpath.str);
	string_deallocate(testpath.str);
	string_deallocate(filetestpath.str);

	return 0;
}

#endif

static void
test_fs_declare(void) {
	ADD_TEST(fs, directory);
	ADD_TEST(fs, file);
	ADD_TEST(fs, util);
	ADD_TEST(fs, query);
	ADD_TEST(fs, event);
#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_ANDROID && !FOUNDATION_PLATFORM_BSD
	ADD_TEST(fs, monitor);
#endif
}

static test_suite_t test_fs_suite = {test_fs_application,
                                     test_fs_memory_system,
                                     test_fs_config,
                                     test_fs_declare,
                                     test_fs_initialize,
                                     test_fs_finalize,
                                     0};

#if BUILD_MONOLITHIC

int
test_fs_run(void);

int
test_fs_run(void) {
	test_suite = test_fs_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_fs_suite;
}

#endif
