/* environment.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#include <foundation/internal.h>

static string_t env_executable_name;
static string_t env_executable_dir;
static string_t env_executable_path;
static string_t env_initial_working_dir;
static string_t env_current_working_dir;
static string_t env_app_dir;
static string_t env_temp_dir;
static bool env_temp_dir_local;

#if FOUNDATION_PLATFORM_WINDOWS
static string_t env_var;
#include <foundation/windows.h>
#elif FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#endif

#if FOUNDATION_PLATFORM_ANDROID
#include <foundation/android.h>
#endif

#if FOUNDATION_PLATFORM_APPLE
#include <foundation/apple.h>

extern void
environment_ns_command_line(string_t** argv);

extern string_t
environment_ns_home_directory(char*, size_t);

extern string_t
environment_ns_temporary_directory(char*, size_t);

string_t
environment_ns_bundle_directory(char* buffer, size_t capacity);

extern string_t
environment_ns_current_working_directory(char* buffer, size_t capacity);

extern bool
environment_ns_set_current_working_directory(const char* buffer, size_t length);

#endif

#if FOUNDATION_PLATFORM_BSD
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

static application_t env_app;
static string_t* env_argv;

#if FOUNDATION_PLATFORM_BSD
static int env_main_argc;
static const char* const* env_main_argv;
#endif

static void
environment_clean_temporary_directory(bool recreate);

void
environment_main_args(int argc, const char* const* argv) {
#if FOUNDATION_PLATFORM_BSD
	env_main_argc = argc;
	env_main_argv = argv;
#else
	FOUNDATION_UNUSED(argc);
	FOUNDATION_UNUSED(argv);
#endif
}

static void
environment_set_executable_paths(char* executable_path, size_t length) {
	size_t last_path = string_rfind(executable_path, length, '/', STRING_NPOS);
	if (last_path != STRING_NPOS) {
		env_executable_dir = string_clone(executable_path, last_path);
		env_executable_name = string_clone(executable_path + last_path + 1, length - (last_path + 1));
	} else {
		env_executable_dir = (string_t){0, 0};
		env_executable_name = string_clone(executable_path, length);
	}
#if FOUNDATION_PLATFORM_WINDOWS
	if ((env_executable_name.length > 4) &&
	    (string_ends_with(STRING_ARGS(env_executable_name), STRING_CONST(".exe")) ||
	     string_ends_with(STRING_ARGS(env_executable_name), STRING_CONST(".Exe")) ||
	     string_ends_with(STRING_ARGS(env_executable_name), STRING_CONST(".EXE")))) {
		env_executable_name.str[env_executable_name.length - 4] = 0;
		env_executable_name.length -= 4;
	}
#endif
	env_executable_path = string_clone(executable_path, length);
}

int
internal_environment_initialize(const application_t application) {
	string_const_t working_dir;
	char buffer[BUILD_MAX_PATHLEN];

#if FOUNDATION_PLATFORM_WINDOWS
	int ia;
	int args_count = 0;
	wchar_t module_filename[BUILD_MAX_PATHLEN];
	LPWSTR* arg_list = CommandLineToArgvW(GetCommandLineW(), &args_count);
	if (!arg_list)
		return -1;

	for (ia = 0; ia < args_count; ++ia) {
		array_push(env_argv, string_allocate_from_wstring(arg_list[ia], wstring_length(arg_list[ia])));
	}

	LocalFree(arg_list);

	if (GetModuleFileNameW(0, module_filename, BUILD_MAX_PATHLEN)) {
		string_t exe_path =
		    string_convert_utf16(buffer, sizeof(buffer), (uint16_t*)module_filename, wstring_length(module_filename));
		exe_path = path_absolute(exe_path.str, exe_path.length, BUILD_MAX_PATHLEN);

		environment_set_executable_paths(exe_path.str, exe_path.length);
	} else {
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to get module filename"));
		return -1;
	}

#elif FOUNDATION_PLATFORM_APPLE

	environment_ns_command_line(&env_argv);

	// TODO: Read executable name from system, not command line (might be set to anything)
	string_t exe_path = string_copy(buffer, sizeof(buffer), env_argv[0].str, env_argv[0].length);
	exe_path = path_absolute(exe_path.str, exe_path.length, sizeof(buffer));
	environment_set_executable_paths(exe_path.str, exe_path.length);

#if FOUNDATION_PLATFORM_IOS
	string_t localpath = string_thread_buffer();
	string_t bundle_dir = environment_bundle_path(STRING_ARGS(localpath));
	environment_set_current_working_directory(STRING_ARGS(bundle_dir));
#endif

#elif FOUNDATION_PLATFORM_ANDROID

	string_const_t dir_name;
	string_t exe_path;
	char exelink[BUILD_MAX_PATHLEN] = {0};
	ssize_t exelength;
	string_const_t exe_name;

	stream_t* cmdline = fs_open_file(STRING_CONST("/proc/self/cmdline"), STREAM_IN | STREAM_BINARY);
	if (!cmdline) {
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/cmdline"));
		return -1;
	}

	while (true) {
		string_t arg = stream_read_string(cmdline);
		if (!arg.length) {
			string_deallocate(arg.str);
			break;
		}
		array_push(environment_argv, arg);
	}
	stream_deallocate(cmdline);

	dir_name = path_directory_name(android_app()->activity->internalDataPath,
	                               string_length(android_app()->activity->internalDataPath));
	exe_path = path_concat(buffer, sizeof(buffer), STRING_ARGS(dir_name), STRING_CONST("lib"));

	// This will return something like "app_process" since we're just a dynamic
	// library that gets invoked by a launcher process
	exelength = readlink("/proc/self/exe", exelink, sizeof(exelink));
	if (exelength < 0) {
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/exe link"));
		return -1;
	}

	exe_name = path_file_name(exelink, (size_t)exelength);
	exe_path = path_append(exe_path.str, exe_path.length, sizeof(exelink), exe_name.str, exe_name.length);

	environment_set_executable_paths(exe_path.str, exe_path.length);

#elif FOUNDATION_PLATFORM_BSD

	for (int ia = 0; ia < environment_main_argc; ++ia)
		array_push(environment_argv, string_clone(environment_main_argv[ia], string_length(environment_main_argv[ia])));

	int callarg[4];
	size_t size = sizeof(buffer);
	callarg[0] = CTL_KERN;
	callarg[1] = KERN_PROC;
	callarg[2] = KERN_PROC_PATHNAME;
	callarg[3] = -1;
	sysctl(callarg, 4, buffer, &size, 0, 0);

	string_t exe_path = path_clean(buffer, size ? size - 1 : 0, BUILD_MAX_PATHLEN);
	exe_path = path_absolute(STRING_ARGS(exe_path), BUILD_MAX_PATHLEN);

	environment_set_executable_paths(STRING_ARGS(exe_path));

#elif FOUNDATION_PLATFORM_POSIX

	stream_t* cmdline = fs_open_file(STRING_CONST("/proc/self/cmdline"), STREAM_IN | STREAM_BINARY);
	if (!cmdline) {
		log_error(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/cmdline"));
		return -1;
	}

	while (true) {
		string_t arg = stream_read_string(cmdline);
		if (!arg.length) {
			string_deallocate(arg.str);
			break;
		}
		array_push(env_argv, arg);
	}
	stream_deallocate(cmdline);

	ssize_t exelength = readlink("/proc/self/exe", buffer, sizeof(buffer));
	if (exelength < 0) {
		int err = errno;
		string_const_t errmsg = system_error_message(err);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/exe link: %.*s (%d)"),
		           STRING_FORMAT(errmsg), err);
		return -1;
	}

	string_t exe_path = path_clean(buffer, (size_t)exelength, sizeof(buffer));
	exe_path = path_absolute(STRING_ARGS(exe_path), sizeof(buffer));

	environment_set_executable_paths(exe_path.str, exe_path.length);

#else
#error Not implemented
#endif

	env_app = application;

	if (uuid_is_null(env_app.instance))
		env_app.instance = uuid_generate_random();

	working_dir = environment_current_working_directory();
	env_initial_working_dir = string_clone(working_dir.str, working_dir.length);

	environment_clean_temporary_directory(true);

	return 0;
}

void
internal_environment_finalize(void) {
	environment_clean_temporary_directory(false);

	string_array_deallocate(env_argv);

#if FOUNDATION_PLATFORM_WINDOWS
	string_deallocate(env_var.str);
#endif
	string_deallocate(env_executable_name.str);
	string_deallocate(env_executable_dir.str);
	string_deallocate(env_executable_path.str);
	string_deallocate(env_initial_working_dir.str);
	string_deallocate(env_current_working_dir.str);
	string_deallocate(env_app_dir.str);
	string_deallocate(env_temp_dir.str);

	env_executable_name = env_executable_dir = env_executable_path = env_initial_working_dir = env_current_working_dir =
	    env_app_dir = env_temp_dir = (string_t){0, 0};
}

const string_const_t*
environment_command_line(void) {
	return (const string_const_t*)env_argv;
}

string_const_t
environment_executable_name(void) {
	return string_to_const(env_executable_name);
}

string_const_t
environment_executable_directory(void) {
	return string_to_const(env_executable_dir);
}

string_const_t
environment_executable_path(void) {
	return string_to_const(env_executable_path);
}

string_const_t
environment_initial_working_directory(void) {
	return string_to_const(env_initial_working_dir);
}

string_const_t
environment_current_working_directory(void) {
	if (env_current_working_dir.str)
		return string_to_const(env_current_working_dir);
#if FOUNDATION_PLATFORM_WINDOWS
	{
		string_t localpath;
		wchar_t wdbuffer[BUILD_MAX_PATHLEN];
		wchar_t* wd = wdbuffer;
		DWORD ret = GetCurrentDirectoryW(BUILD_MAX_PATHLEN, wd);
		if (ret > BUILD_MAX_PATHLEN) {
			wd = memory_allocate(0, ret * sizeof(wchar_t), 0, MEMORY_TEMPORARY);
			ret = GetCurrentDirectoryW(BUILD_MAX_PATHLEN, wd);
		}
		localpath = string_allocate_from_wstring(wd, ret);
		if (wd != wdbuffer)
			memory_deallocate(wd);
		env_current_working_dir = path_clean(STRING_ARGS_CAPACITY(localpath));
	}
#elif FOUNDATION_PLATFORM_APPLE
	string_t localpath = string_thread_buffer();
	size_t local_capacity = localpath.length;
	localpath = environment_ns_current_working_directory(localpath.str, local_capacity);
	localpath = path_clean(localpath.str, localpath.length, local_capacity);
	if ((localpath.length > 1) && (localpath.str[localpath.length - 1] == '/'))
		localpath.str[--localpath.length] = 0;
	env_current_working_dir = string_clone(STRING_ARGS(localpath));
#elif FOUNDATION_PLATFORM_POSIX
	string_t localpath = string_thread_buffer();
	if (!getcwd(localpath.str, localpath.length)) {
		int err = errno;
		string_const_t errmsg = system_error_message(err);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to get cwd: %.*s (%d)"), STRING_FORMAT(errmsg), err);
		return string_const(0, 0);
	}
	localpath = path_clean(localpath.str, string_length(localpath.str), localpath.length);
	if ((localpath.length > 1) && (localpath.str[localpath.length - 1] == '/'))
		localpath.str[--localpath.length] = 0;
	env_current_working_dir = string_clone(STRING_ARGS(localpath));
#else
#error Not implemented
#endif
	return string_to_const(env_current_working_dir);
}

bool
environment_set_current_working_directory(const char* path, size_t length) {
	bool result = true;
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t* wpath = wstring_allocate_from_string(path, length);
		if (!SetCurrentDirectoryW(wpath)) {
			log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to set working directory: %ls"), wpath);
			result = false;
		}
		wstring_deallocate(wpath);
	}
#elif FOUNDATION_PLATFORM_APPLE
	result = environment_ns_set_current_working_directory(path, length);
#elif FOUNDATION_PLATFORM_POSIX
	string_t buffer = string_thread_buffer();
	string_t pathstr = string_copy(STRING_ARGS(buffer), path, length);
	if (chdir(pathstr.str) < 0) {
		int err = errno;
		string_const_t errmsg = system_error_message(err);
		log_warnf(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to set working directory to %.*s: %.*s (%d)"),
		          (int)length, path, STRING_FORMAT(errmsg), err);
		result = false;
	}
#else
#error Not implemented
#endif
	string_deallocate(env_current_working_dir.str);
	env_current_working_dir = (string_t){0, 0};
	return result;
}

string_const_t
environment_application_directory(void) {
	if (env_app_dir.str)
		return string_to_const(env_app_dir);
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t wpath[BUILD_MAX_PATHLEN];
		SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, wpath);
		char* pathbuf = memory_allocate(0, BUILD_MAX_PATHLEN, 0, MEMORY_TEMPORARY);
		string_t pathstr = string_convert_utf16(pathbuf, BUILD_MAX_PATHLEN, wpath, wstring_length(wpath));
		pathstr = path_clean(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN);
		if (env_app.company.length)
			pathstr = path_append(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN, STRING_ARGS(env_app.company));
		if (env_app.short_name.length)
			pathstr = path_append(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN, STRING_ARGS(env_app.short_name));
		env_app_dir = string_clone(STRING_ARGS(pathstr));
		memory_deallocate(pathbuf);
	}
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_BSD || FOUNDATION_PLATFORM_TIZEN
	{
		string_const_t env_home = environment_variable(STRING_CONST("HOME"));
		if (!env_home.length) {
			struct passwd* pw = getpwuid(getuid());
			env_home.str = pw->pw_dir;
			env_home.length = string_length(env_home.str);
		}
		char* pathbuf = memory_allocate(0, BUILD_MAX_PATHLEN, 0, MEMORY_TEMPORARY);
		string_t pathstr = string_copy(pathbuf, BUILD_MAX_PATHLEN, STRING_ARGS(env_home));
		if (env_app.company.length)
			pathstr = string_append_varg(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN, STRING_CONST("/."),
			                             STRING_ARGS(env_app.company), nullptr);
		if (env_app.short_name.length)
			pathstr = string_append_varg(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN, STRING_CONST("/."),
			                             STRING_ARGS(env_app.short_name), nullptr);
		env_app_dir = string_clone(STRING_ARGS(pathstr));
		memory_deallocate(pathbuf);
	}
#elif FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_MACOS
	{
		char* pathbuf = memory_allocate(0, BUILD_MAX_PATHLEN, 0, MEMORY_TEMPORARY);
		string_t pathstr = environment_ns_home_directory(pathbuf, BUILD_MAX_PATHLEN);
#if FOUNDATION_PLATFORM_MACOS
		if (!(environment_application()->flags & APPLICATION_UTILITY)) {
			char bundle_identifier[256];
			string_t bundle = environment_bundle_identifier(bundle_identifier, sizeof(bundle_identifier));
			pathstr = path_append_varg(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN,
			                           STRING_CONST("Library/Application Support"), STRING_ARGS(bundle), nullptr);
		} else {
			if (env_app.company.length)
				pathstr = string_append_varg(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN, STRING_CONST("/."),
				                             STRING_ARGS(env_app.company), nullptr);
			if (env_app.short_name.length)
				pathstr = string_append_varg(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN, STRING_CONST("/."),
				                             STRING_ARGS(env_app.short_name), nullptr);
		}
#endif
		env_app_dir = string_clone(STRING_ARGS(pathstr));
		memory_deallocate(pathbuf);
	}
#elif FOUNDATION_PLATFORM_ANDROID
	{
		const char* data_path = android_app()->activity->internalDataPath;
		env_app_dir = string_clone(data_path, string_length(data_path));
	}
#else
#error Not implemented
#endif
	return string_to_const(env_app_dir);
}

string_const_t
environment_temporary_directory(void) {
	if (env_temp_dir.str)
		return string_to_const(env_temp_dir);
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t wpath[BUILD_MAX_PATHLEN];
		GetTempPathW(BUILD_MAX_PATHLEN, wpath);
		env_temp_dir = string_allocate_from_wstring(wpath, wstring_length(wpath));
		env_temp_dir = path_clean(STRING_ARGS_CAPACITY(env_temp_dir));
		env_temp_dir = path_absolute(STRING_ARGS_CAPACITY(env_temp_dir));
	}
#elif FOUNDATION_PLATFORM_ANDROID
	// Use application internal data path, or if that fails, external data path
	struct android_app* app = android_app();
	const char* test_path[] = {app && app->activity ? app->activity->internalDataPath : 0,
	                           app && app->activity ? app->activity->externalDataPath : 0};
	char path[BUILD_MAX_PATHLEN];
	for (int itest = 0; !env_temp_dir.str && (itest < 2); ++itest) {
		if (test_path[itest] && test_path[itest][0]) {
			string_t pathstr = string_copy(path, sizeof(path), test_path[itest], string_length(test_path[itest]));
			pathstr = path_clean(STRING_ARGS(pathstr), sizeof(path));

			fs_make_directory(STRING_ARGS(pathstr));

			string_t temp_path = path_append(STRING_ARGS(pathstr), sizeof(path), STRING_CONST(".tmp-"));
			string_const_t uuidstr = string_from_uuid_static(uuid_generate_random());
			temp_path = string_append(STRING_ARGS(temp_path), sizeof(path), STRING_ARGS(uuidstr));

			stream_t* temp_stream = fs_open_file(STRING_ARGS(temp_path), STREAM_CREATE | STREAM_OUT | STREAM_BINARY);
			if (temp_stream) {
				stream_deallocate(temp_stream);

				env_temp_dir = path_append(STRING_ARGS(pathstr), sizeof(path), STRING_CONST(".tmp"));
				env_temp_dir = string_clone(STRING_ARGS(env_temp_dir));

				env_temp_dir_local = true;
			}
		}
	}
#else
#if FOUNDATION_PLATFORM_APPLE
	char* buffer = memory_allocate(HASH_STRING, BUILD_MAX_PATHLEN, 0, MEMORY_PERSISTENT);
	env_temp_dir = environment_ns_temporary_directory(buffer, BUILD_MAX_PATHLEN);
	env_temp_dir = string_clone(STRING_ARGS(env_temp_dir));
#if FOUNDATION_PLATFORM_IOS
	env_temp_dir_local = true;
#endif
#elif FOUNDATION_PLATFORM_POSIX
	env_temp_dir = string_clone(P_tmpdir, string_length(P_tmpdir));
#endif
#endif

#if !FOUNDATION_PLATFORM_ANDROID && !FOUNDATION_PLATFORM_IOS
	if (env_app.short_name.length) {
		size_t curlen = env_temp_dir.length;
		size_t cfglen = env_app.short_name.length;
		size_t totallen = curlen + cfglen + 40;
		if (totallen < BUILD_MAX_PATHLEN) {
			string_t modpath = string_allocate(0, totallen - 1);
			string_const_t uuidstr = string_from_uuid_static(env_app.instance);
			modpath = string_copy(modpath.str, totallen, STRING_ARGS(env_temp_dir));
			modpath = path_append_varg(STRING_ARGS(modpath), totallen, STRING_ARGS(env_app.short_name),
			                           STRING_ARGS(uuidstr), nullptr);
			string_deallocate(env_temp_dir.str);
			env_temp_dir = modpath;
			env_temp_dir_local = true;
		}
	}
#endif
#if FOUNDATION_PLATFORM_APPLE
	if (env_temp_dir.str == buffer)
		env_temp_dir = string_clone(STRING_ARGS(env_temp_dir));
	string_deallocate(buffer);
#endif
	if ((env_temp_dir.length > 1) && (env_temp_dir.str[env_temp_dir.length - 1] == '/')) {
		--env_temp_dir.length;
		env_temp_dir.str[env_temp_dir.length] = 0;
	}
	return string_to_const(env_temp_dir);
}

static void
environment_clean_temporary_directory(bool recreate) {
	string_const_t path = environment_temporary_directory();

	if (env_temp_dir_local && fs_is_directory(path.str, path.length)) {
		fs_remove_directory(path.str, path.length);
		if (recreate)
			fs_make_directory(path.str, path.length);
	}
}

string_const_t
environment_variable(const char* var, size_t length) {
	string_t buffer = string_thread_buffer();
	string_t varstr = string_copy(STRING_ARGS(buffer), var, length);
#if FOUNDATION_PLATFORM_WINDOWS
	unsigned int required;
	wchar_t* key = wstring_allocate_from_string(STRING_ARGS(varstr));
	wchar_t val[BUILD_MAX_PATHLEN];
	val[0] = 0;
	if ((required = GetEnvironmentVariableW(key, val, BUILD_MAX_PATHLEN)) > BUILD_MAX_PATHLEN) {
		wchar_t* val_local = memory_allocate(0, sizeof(wchar_t) * ((size_t)required + 2), 0, MEMORY_TEMPORARY);
		val_local[0] = 0;
		required = GetEnvironmentVariableW(key, val_local, required + 1);
		if (env_var.str)
			string_deallocate(env_var.str);
		env_var = string_allocate_from_wstring(val_local, required);
		memory_deallocate(val_local);
	} else {
		if (env_var.str)
			string_deallocate(env_var.str);
		env_var = string_allocate_from_wstring(val, required);
	}
	wstring_deallocate(key);
	return string_to_const(env_var);
#elif FOUNDATION_PLATFORM_POSIX
	const char* value = getenv(varstr.str);
	return string_const(value, value ? string_length(value) : 0);
#else
#error Not implemented
#endif
}

const application_t*
environment_application(void) {
	return &env_app;
}
